/*
 * object.cpp
 *
 *  Created on: May 21, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/Buffer.h"

namespace fibjs {

v8::Local<v8::Object> object_base::wrap(Isolate* isolate, v8::Local<v8::Object> o)
{
    if (!m_isolate)
        m_isolate = isolate;
    v8::Isolate* v8_isolate = isolate->m_isolate;

    if (!(m_isJSObject & JSOBJECT_JSHANDLE)) {
        if (o.IsEmpty())
            o = Classinfo().CreateInstance(isolate);
        handle_.Reset(v8_isolate, o);
        o->SetAlignedPointerInInternalField(0, this);
        v8_isolate->AdjustAmountOfExternalAllocatedMemory(m_nExtMemory);

        internalRef();
        handle_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);

        m_isJSObject |= JSOBJECT_JSHANDLE;
    } else
        o = v8::Local<v8::Object>::New(v8_isolate, handle_);

    return o;
}

void* object_base::unwrap(v8::Local<v8::Value> o)
{
    if (o.IsEmpty() || !o->IsObject())
        return NULL;
    return get_instance_pointer(o.As<v8::Object>(), Buffer_base::class_info().getInstanceType());
}

}
