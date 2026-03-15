/*
 * DOMEvent.cpp
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#include "object.h"
#include "DOMEvent.h"

namespace fibjs {

result_t DOMEvent_base::_new(exlib::string type, v8::Local<v8::Object> eventInitDict,
    obj_ptr<DOMEvent_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(eventInitDict);
    v8::Local<v8::Context> ctx = isolate->context();

    bool bubbles = false;
    bool cancelable = false;
    bool composed = false;

    v8::Local<v8::Value> v;
    if (eventInitDict->Get(ctx, isolate->NewString("bubbles")).ToLocal(&v) && v->IsTrue())
        bubbles = true;
    if (eventInitDict->Get(ctx, isolate->NewString("cancelable")).ToLocal(&v) && v->IsTrue())
        cancelable = true;
    if (eventInitDict->Get(ctx, isolate->NewString("composed")).ToLocal(&v) && v->IsTrue())
        composed = true;

    obj_ptr<DOMEvent> ev = new DOMEvent(type, bubbles, cancelable, composed);
    ev->wrap(This);
    retVal = ev;
    return 0;
}

}
