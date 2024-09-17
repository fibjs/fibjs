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
    class NativeContextScope {
    public:
        NativeContextScope(i::Tagged<i::NativeContext> context)
            : isolate_(context->GetIsolate())
        {
            i::HandleScopeImplementer* impl = isolate_->handle_scope_implementer();
            impl->EnterContext(context);
            impl->SaveContext(isolate_->context());
            isolate_->set_context(context);
        }

        ~NativeContextScope()
        {
            i::HandleScopeImplementer* impl = isolate_->handle_scope_implementer();
            impl->LeaveContext();
            isolate_->set_context(impl->RestoreContext());
        }

    public:
        i::Isolate* isolate_;
    };

    i::Isolate* _isolate = reinterpret_cast<i::Isolate*>(m_isolate);

    i::MicrotaskQueue* queue = _isolate->default_microtask_queue();
    if (queue->size_ > 0) {
        intptr_t size1 = 0;

        for (intptr_t i = 0, p = 0; i < queue->size_; i++) {
            i::Address _task = queue->ring_buffer_[(i + queue->start_) % queue->capacity_];
            int instance_type = i::Internals::GetInstanceType(_task);

            if (instance_type == i::PROMISE_REJECT_REACTION_JOB_TASK_TYPE
                || instance_type == i::PROMISE_FULFILL_REACTION_JOB_TASK_TYPE) {
                sync([addr = api_internal::GlobalizeReference(_isolate, _task)]() -> int {
                    i::Address _task = *addr;
                    api_internal::DisposeGlobal(addr);

                    auto _react_task = i::Cast<i::PromiseReactionJobTask>(i::Tagged<i::Object>(_task));
                    auto context = _react_task->context()->native_context();

                    NativeContextScope scope(context);
                    JSFiber::EnterJsScope s;

                    auto handler = i::direct_handle(_react_task->handler(), scope.isolate_);
                    auto argument = i::direct_handle(_react_task->argument(), scope.isolate_);
                    auto promise = i::direct_handle(_react_task->promise_or_capability(), scope.isolate_);

                    if (i::IsUndefined(*handler)) {
                        if (i::Internals::GetInstanceType(_task) == i::PROMISE_REJECT_REACTION_JOB_TASK_TYPE)
                            i::JSPromise::Reject(i::Cast<i::JSPromise>(promise), argument);
                        else
                            i::JSPromise::Resolve(i::Cast<i::JSPromise>(promise), argument);
                    } else
                        i::Execution::Call(scope.isolate_, handler, promise, 1, &argument);

                    return 0;
                });
            } else {
                queue->ring_buffer_[(p++ + queue->start_) % queue->capacity_] = _task;
                size1++;
            }
        }

        queue->size_ = size1;
        if (queue->size_)
            m_isolate->PerformMicrotaskCheckpoint();
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
                    JSFiber::EnterJsScope s;

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