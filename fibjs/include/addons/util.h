#pragma once

namespace node {

class PersistentToLocal {
public:
    // If persistent.IsWeak() == false, then do not call persistent.Reset()
    // while the returned Local<T> is still in scope, it will destroy the
    // reference to the object.
    template <class TypeName>
    static inline v8::Local<TypeName> Default(
        v8::Isolate* isolate,
        const v8::PersistentBase<TypeName>& persistent)
    {
        if (persistent.IsWeak()) {
            return PersistentToLocal::Weak(isolate, persistent);
        } else {
            return PersistentToLocal::Strong(persistent);
        }
    }

    // Unchecked conversion from a non-weak Persistent<T> to Local<T>,
    // use with care!
    //
    // Do not call persistent.Reset() while the returned Local<T> is still in
    // scope, it will destroy the reference to the object.
    template <class TypeName>
    static inline v8::Local<TypeName> Strong(
        const v8::PersistentBase<TypeName>& persistent)
    {
        DCHECK(!persistent.IsWeak());
        return *reinterpret_cast<v8::Local<TypeName>*>(
            const_cast<v8::PersistentBase<TypeName>*>(&persistent));
    }

    template <class TypeName>
    static inline v8::Local<TypeName> Weak(
        v8::Isolate* isolate,
        const v8::PersistentBase<TypeName>& persistent)
    {
        return v8::Local<TypeName>::New(isolate, persistent);
    }
};

}
