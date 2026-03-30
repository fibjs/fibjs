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

void Isolate::RunMicrotasks(MicrotaskCheckpointReason reason)
{
    bool allow_same_turn_reentry = (reason == MicrotaskCheckpointReason::kJsScopeLeave);

    if (m_module_evaluating > 0 && allow_same_turn_reentry)
        return;

    i::Isolate* _isolate = reinterpret_cast<i::Isolate*>(m_isolate);
    i::MicrotaskQueue* queue = _isolate->default_microtask_queue();

    do {
        // Dispatch tasks from index 1 onwards to fibers first
        for (intptr_t i = allow_same_turn_reentry ? 1 : 0; i < queue->size_; i++) {
            i::Address _task = queue->ring_buffer_[(i + queue->start_) % queue->capacity_];
            sync_urgent([addr = api_internal::GlobalizeReference(_isolate, _task), _isolate]() -> int {
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

        if (allow_same_turn_reentry) {
            // Run the first microtask directly in the current context
            if (queue->size_ > 0) {
                queue->size_ = 1;
                queue->RunMicrotasks(_isolate);
            }
        } else
            queue->size_ = 0;

    } while (m_isolate->HasPendingBackgroundTasks()
        && v8::platform::PumpMessageLoop(g_default_platform, m_isolate, v8::platform::MessageLoopBehavior::kWaitForWork));
}
}
