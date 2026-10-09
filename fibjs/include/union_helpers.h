/*
 * union_helpers.h
 *
 * The conversion helpers for the parameter-position unions of the IDL
 * (`Handler|Handler[]|Function|Object|String`, `Integer|FileHandle`,
 * `FileHandle|String|Integer`, `UrlObject|String|Object`, `X|Object`, ...).
 *
 * A union parameter arrives as a `std::variant`: the binding converts the
 * JavaScript value into one alternative, the implementation gets back the
 * object it works with through the helper of the matching family. Each helper
 * makes the same selection the class's own `load()` makes, so the two entry
 * points share one conversion.
 *
 * Two forms exist:
 *
 * - the plain form (no AsyncEvent*) is for synchronous entry points; it reads
 *   the isolate and must only be used where V8 is available.
 *
 * - the async-aware form (with AsyncEvent*) keeps the phase discipline inside
 *   the helper. The sync phase converts every alternative whose conversion
 *   touches V8 or the current isolate: the callbacks/routing maps, the array
 *   form (Chain::append wraps the handlers and sets JS private properties)
 *   and the string form (HttpFileHandler::create uses the blocking ac_stat
 *   helper, HttpRepeater relies on holder()). The converted handler is carried
 *   in `ac->m_ctx` (the parameter slot; `m_ctxo` is the callback-style
 *   return-value slot, see AsyncCallBack::check_result), then CALL_E_NOSYNC is
 *   returned. The async phase takes the prepared slot, or uses the handler
 *   class alternative directly (no conversion at all) - so a `cc_` caller keeps
 *   working with it. The async phase never calls a `_new`: those are the JS
 *   constructors (and the underlying construction of the array/string forms
 *   needs V8 or the current isolate even when built directly). Call sites read:
 *
 *       result_t hr = xxx_from_union(v, out, ac);
 *       if (hr < 0)
 *           return hr;   // CALL_E_NOSYNC family -> hand off; other codes -> error
 *
 *   The exception is `filehandle_from_union`: both alternatives are C++-safe
 *   (the integer opens a FileHandle on the descriptor), so no sync phase is
 *   needed at all and it stays a plain helper.
 *
 * The union mechanism itself: plans/idl-union-types-2026-10-02.md,
 * plans/param-ctor-completion-2026-10-04.md §7-§9.
 * The phase discipline: plans/async-phase-discipline-audit-2026-10-05.md.
 */

#pragma once

#include "AsyncCall.h"
#include "ifs/Handler.h"
#include "ifs/FileHandle.h"
#include "ifs/UrlObject.h"

namespace fibjs {

// Handler|Handler[]|Function|Object|String: the class alternative is used as
// it is, every other alternative runs through the Handler constructor it
// stands for (the array of handlers, the callback, the routing map, the
// address string).
template <typename Variant>
result_t handler_from_union(Variant& hdlr, obj_ptr<Handler_base>& retVal)
{
    return std::visit([&](auto&& value) -> result_t {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, obj_ptr<Handler_base>>) {
            retVal = value;
            return 0;
        } else if constexpr (std::is_same_v<T, std::vector<obj_ptr<Handler_base>>>
            || std::is_same_v<T, v8::Local<v8::Function>>
            || std::is_same_v<T, v8::Local<v8::Object>>
            || std::is_same_v<T, exlib::string>) {
            return Handler_base::_new(value, retVal);
        } else {
            return CALL_E_TYPEMISMATCH;
        }
    }, hdlr);
}

// Async-aware form: the sync phase converts every alternative whose conversion
// touches V8 or the current isolate - the callbacks/routing maps, the array
// form (Chain::append wraps handlers) and the string form (ac_stat/holder
// based construction) - and carries the handler in m_ctx. The class
// alternative (`obj_ptr<Handler_base>`) is left to the async phase: it needs
// no conversion at all, so a cc_ caller can pass it directly.
//
// The continuation is *borrowed* (reference, not by value): the helper only
// reads the phase and writes the ctx slot, so the caller keeps ownership and
// hands the handle on to the machine it builds afterwards.
template <typename Variant>
result_t handler_from_union(Variant& hdlr, obj_ptr<Handler_base>& retVal, AsyncHandle& ac)
{
    if (ac.isSync()) {
        result_t hr = std::visit([&](auto&& value) -> result_t {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, obj_ptr<Handler_base>>)
                return 0; // already a C++ object: left to the async phase
            else
                return Handler_base::_new(value, retVal);
        }, hdlr);
        if (hr < 0)
            return hr;

        if (retVal != NULL) {
            ac.ctxv().resize(1);
            ac.ctxv()[0] = retVal;
        }

        return CALL_E_NOSYNC;
    }

    // async / cc_: the slot prepared by the sync phase comes first
    if (ac.ctxv().size() > 0 && ac.ctxv()[0].object() != NULL) {
        retVal = Handler_base::getInstance(ac.ctxv()[0].object());
        if (retVal != NULL)
            return 0;
    }

    // otherwise only the class alternative is usable. No *_new is called here:
    // those are the JS constructors, and the array/string forms need V8 or the
    // current isolate even when built directly (Chain::append wraps handlers,
    // HttpRepeater's constructor calls holder()) - a cc_ caller must not reach
    // them, so they are reported instead of entered off the JS thread.
    if (std::holds_alternative<obj_ptr<Handler_base>>(hdlr)) {
        retVal = std::get<obj_ptr<Handler_base>>(hdlr);
        return 0;
    }

    return Runtime::setError(CALL_E_TYPEMISMATCH,
        "the handler union was not prepared: this entry requires the synchronous phase.");
}

// Unmigrated callers pass the raw continuation; borrowing it into a handle for
// the call keeps them working (a borrowed handle never deletes).
template <typename Variant>
result_t handler_from_union(Variant& hdlr, obj_ptr<Handler_base>& retVal, AsyncEvent* ac)
{
    AsyncHandle h(ac);
    return handler_from_union(hdlr, retVal, h);
}

// Integer|FileHandle: the integer opens a FileHandle on that descriptor, the
// handle is used as it is. No V8 state is touched, so this one is safe in an
// async phase.
template <typename Variant>
result_t filehandle_from_union(Variant& fd, obj_ptr<FileHandle_base>& retVal)
{
    return std::visit([&](auto&& value) -> result_t {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, int32_t>) {
            return FileHandle_base::_new(value, retVal);
        } else if constexpr (std::is_same_v<T, obj_ptr<FileHandle_base>>) {
            retVal = value;
            return 0;
        } else {
            return CALL_E_TYPEMISMATCH;
        }
    }, fd);
}

// UrlObject|String|Object: the UrlObject is used as it is, a string is parsed
// into one, a components object is loaded by the UrlObject constructor.
template <typename Variant>
result_t urlobject_from_union(Variant& url, obj_ptr<UrlObject_base>& retVal)
{
    return std::visit([&](auto&& value) -> result_t {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, obj_ptr<UrlObject_base>>) {
            retVal = value;
            return 0;
        } else if constexpr (std::is_same_v<T, exlib::string>) {
            return UrlObject_base::_new(value, exlib::string(""), retVal);
        } else if constexpr (std::is_same_v<T, v8::Local<v8::Object>>) {
            return UrlObject_base::load(value, retVal);
        } else {
            return CALL_E_TYPEMISMATCH;
        }
    }, url);
}

// HttpCookie|Object, RTCIceCandidate|Object, ...: the class instance is used as
// it is, the object alternative is resolved through the class constructor (the
// same selection the class's `load()` would make).
template <typename Base, typename Variant>
result_t ctor_object_from_union(Variant& v, obj_ptr<Base>& retVal)
{
    return std::visit([&](auto&& value) -> result_t {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, obj_ptr<Base>>) {
            retVal = value;
            return 0;
        } else if constexpr (std::is_same_v<T, v8::Local<v8::Object>>) {
            return Base::_new(value, retVal);
        } else {
            return CALL_E_TYPEMISMATCH;
        }
    }, v);
}

// Async-aware form: the object alternative reads JS properties, so it stays in
// the sync phase and travels in m_ctx; the class alternative needs no
// conversion, so it is resolved in the async phase and a cc_ caller can pass
// it directly. No `_new` is called in the async phase. Like the handler form
// above, the continuation is borrowed: the caller keeps ownership.
template <typename Base, typename Variant>
result_t ctor_object_from_union(Variant& v, obj_ptr<Base>& retVal, AsyncHandle& ac)
{
    if (ac.isSync()) {
        result_t hr = std::visit([&](auto&& value) -> result_t {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, obj_ptr<Base>>)
                return 0; // already a C++ object: left to the async phase
            else if constexpr (std::is_same_v<T, v8::Local<v8::Object>>)
                return Base::_new(value, retVal);
            else
                return CALL_E_TYPEMISMATCH;
        }, v);
        if (hr < 0)
            return hr;

        if (retVal != NULL) {
            ac.ctxv().resize(1);
            ac.ctxv()[0] = retVal;
        }

        return CALL_E_NOSYNC;
    }

    // async / cc_: the slot prepared by the sync phase comes first
    if (ac.ctxv().size() > 0 && ac.ctxv()[0].object() != NULL) {
        retVal = Base::getInstance(ac.ctxv()[0].object());
        if (retVal != NULL)
            return 0;
    }

    if (std::holds_alternative<obj_ptr<Base>>(v)) {
        retVal = std::get<obj_ptr<Base>>(v);
        return 0;
    }

    return Runtime::setError(CALL_E_TYPEMISMATCH,
        "the object union was not prepared: this entry requires the synchronous phase.");
}

// Raw-continuation bridge, same rule as handler_from_union above.
template <typename Base, typename Variant>
result_t ctor_object_from_union(Variant& v, obj_ptr<Base>& retVal, AsyncEvent* ac)
{
    AsyncHandle h(ac);
    return ctor_object_from_union(v, retVal, h);
}

}
