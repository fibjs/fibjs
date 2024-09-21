/*
 * microtask-queue.cpp
 *
 *  Created on: Sep 16, 2024
 *      Author: lion
 */

#include "v8-internal.h"
#include "v8-microtask-queue.h"
#include "v8/src/base/macros.h"

#define private public
#include "v8/src/execution/microtask-queue.h"
#undef private

#include "v8.h"
#include "libplatform/libplatform.h"
#include "exlib/include/qstring.h"
#include "v8/src/api/api-inl.h"
#include "v8/src/objects/promise-inl.h"
#include "v8_api.h"

#include "AsyncCall.h"
#include "Isolate.h"
#include "Fiber.h"

using namespace v8;

namespace fibjs {

intptr_t RunMicrotaskSize(v8::Isolate* isolate)
{
    i::Isolate* _isolate = reinterpret_cast<i::Isolate*>(isolate);
    i::MicrotaskQueue* queue = _isolate->default_microtask_queue();
    return queue->size();
}

void Isolate::PerformMicrotaskCheckpoint()
{
    i::Isolate* _isolate = reinterpret_cast<i::Isolate*>(m_isolate);
    i::MicrotaskQueue* queue = _isolate->default_microtask_queue();
    if (queue->size_ > 0) {
        for (intptr_t i = 0, p = 0; i < queue->size_; i++) {
            i::Address _task = queue->ring_buffer_[(i + queue->start_) % queue->capacity_];
            sync([addr = api_internal::GlobalizeReference(_isolate, _task), _isolate]() -> int {
                JSFiber::EnterJsScope s;

                std::unique_ptr<i::MicrotaskQueue> queue = i::MicrotaskQueue::New(_isolate);
                queue->EnqueueMicrotask(i::Cast<i::Microtask>(i::Tagged<i::Object>(*addr)));

                std::optional<v8::MicrotasksScope> microtasks_scope;
                queue->RunMicrotasks(_isolate);
                _isolate->ClearKeptObjects();

                api_internal::DisposeGlobal(addr);

                return 0;
            });
        }

        queue->size_ = 0;
    }
}

void Isolate::RunMicrotasks()
{
    bool not_in_task = false;
    if (m_intask.compare_exchange_strong(not_in_task, true)) {
        if ((RunMicrotaskSize(m_isolate) > 0 || m_isolate->HasPendingBackgroundTasks())) {
            m_intask = true;
            sync([this]() -> int {
                {
                    PerformMicrotaskCheckpoint();
                    while (v8::platform::PumpMessageLoop(g_default_platform, m_isolate,
                        m_isolate->HasPendingBackgroundTasks()
                            ? v8::platform::MessageLoopBehavior::kWaitForWork
                            : platform::MessageLoopBehavior::kDoNotWait))
                        PerformMicrotaskCheckpoint();
                }

                m_intask = false;

                return 0;
            });
        } else
            m_intask = false;
    }
}

}