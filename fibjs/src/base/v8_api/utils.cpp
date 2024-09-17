/*
 * utils.cpp
 *
 *  Created on: Sep 16, 2024
 *      Author: lion
 */

#include "src/base/bit-field.h"

#include "v8-internal.h"
#include "v8/src/handles/handles.h"
#include "v8/src/logging/counters.h"

#define private public
#include "v8/src/objects/shared-function-info.h"
#undef private

#include "v8.h"
#include "exlib/include/qstring.h"
#include "v8/src/api/api-inl.h"
#include "v8_api.h"

using namespace v8;

namespace fibjs {

bool isFrozen(Isolate* isolate, Local<Object> object)
{
    i::Isolate* _isolate = reinterpret_cast<i::Isolate*>(isolate);
    auto obj = Utils::OpenHandle(*object);
    Maybe<bool> test = i::JSReceiver::TestIntegrityLevel(_isolate, obj, i::FROZEN);
    return test.FromMaybe(false);
}

void setAsyncFunctoin(Local<Function> func)
{
    i::Handle<i::Object> obj = Utils::OpenHandle(*func);
    i::Handle<i::JSFunction> _func = i::Cast<i::JSFunction>(obj);
    _func->shared()->set_kind(i::FunctionKind::kAsyncFunction);
}

void initImportMeta(Isolate* isolate, Local<Module> module)
{
    i::Isolate* _isolate = reinterpret_cast<i::Isolate*>(isolate);
    i::Handle<i::Object> obj = Utils::OpenHandle(*module);
    i::Handle<i::SourceTextModule> _module = i::Cast<i::SourceTextModule>(obj);
    i::SourceTextModule::GetImportMeta(_isolate, _module);
}

void InvokeApiInterruptCallbacks(Isolate* isolate)
{
    i::Isolate* v8_isolate = (i::Isolate*)isolate;
    v8_isolate->InvokeApiInterruptCallbacks();
}

V8FrameInfo save_fi(Isolate* isolate)
{
    i::Isolate* v8_isolate = (i::Isolate*)isolate;
    V8FrameInfo fi;

    fi.entry_fp = (void*)*v8_isolate->c_entry_fp_address();
    fi.handle = (void*)*v8_isolate->handler_address();

    return fi;
}

}