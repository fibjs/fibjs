/*
 * events.cpp
 *
 *  Created on: Nov 3, 2019
 *      Author: Richard
 */

#include "EventEmitter.h"

namespace fibjs {

DECLARE_MODULE_WITH_CONSTRUCTOR_EX(events, EventEmitter);

RootModule_events::RootModule_events()
{
    static ClassData::ClassMethod s_method[] = {
        { "on", JSTrigger::s_on, false },
        { "on", JSTrigger::s_on, true },
        { "addListener", JSTrigger::s_on, false },
        { "addListener", JSTrigger::s_on, true },
        { "addEventListener", JSTrigger::s_addEventListener, false },
        { "addEventListener", JSTrigger::s_addEventListener, true },
        { "prependListener", JSTrigger::s_prependListener, false },
        { "prependListener", JSTrigger::s_prependListener, true },
        { "once", JSTrigger::s_once, false },
        { "once", JSTrigger::s_once, true },
        { "prependOnceListener", JSTrigger::s_prependOnceListener, false },
        { "prependOnceListener", JSTrigger::s_prependOnceListener, true },
        { "off", JSTrigger::s_off, false },
        { "off", JSTrigger::s_off, true },
        { "removeListener", JSTrigger::s_off, false },
        { "removeListener", JSTrigger::s_off, true },
        { "removeEventListener", JSTrigger::s_removeEventListener, false },
        { "removeEventListener", JSTrigger::s_removeEventListener, true },
        { "removeAllListeners", JSTrigger::s_removeAllListeners, false },
        { "removeAllListeners", JSTrigger::s_removeAllListeners, true },
        { "setMaxListeners", JSTrigger::s_setMaxListeners, false },
        { "setMaxListeners", JSTrigger::s_setMaxListeners, true },
        { "getMaxListeners", JSTrigger::s_getMaxListeners, false },
        { "getMaxListeners", JSTrigger::s_getMaxListeners, true },
        { "listeners", JSTrigger::s_listeners, false },
        { "listeners", JSTrigger::s_listeners, true },
        { "listenerCount", JSTrigger::s_listenerCount, false },
        { "listenerCount", JSTrigger::s_listenerCount, true },
        { "eventNames", JSTrigger::s_eventNames, false },
        { "eventNames", JSTrigger::s_eventNames, true },
        { "emit", JSTrigger::s_emit, false },
        { "emit", JSTrigger::s_emit, true },
    };

    static ClassData::ClassProperty s_property[] = {
        { "defaultMaxListeners", JSTrigger::s_get_defaultMaxListeners, JSTrigger::s_set_defaultMaxListeners, true }
    };

    ClassData& cd = EventEmitter_base::class_info().data();

    cd.cor = JSTrigger::s__new;
    cd.mc = ARRAYSIZE(s_method);
    cd.cms = s_method;
    cd.pc = ARRAYSIZE(s_property);
    cd.cps = s_property;
}
}