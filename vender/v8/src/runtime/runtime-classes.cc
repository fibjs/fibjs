// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdlib.h>
#include <limits>

#include "src/v8.h"

#include "src/isolate-inl.h"
#include "src/runtime/runtime.h"
#include "src/runtime/runtime-utils.h"


namespace v8 {
namespace internal {


RUNTIME_FUNCTION(Runtime_ThrowNonMethodError) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 0);
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewReferenceError("non_method", HandleVector<Object>(NULL, 0)));
}


static Object* ThrowUnsupportedSuper(Isolate* isolate) {
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate,
      NewReferenceError("unsupported_super", HandleVector<Object>(NULL, 0)));
}


RUNTIME_FUNCTION(Runtime_ThrowUnsupportedSuperError) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 0);
  return ThrowUnsupportedSuper(isolate);
}


RUNTIME_FUNCTION(Runtime_ToMethod) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, fun, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  Handle<JSFunction> clone = JSFunction::CloneClosure(fun);
  Handle<Symbol> home_object_symbol(isolate->heap()->home_object_symbol());
  JSObject::SetOwnPropertyIgnoreAttributes(clone, home_object_symbol,
                                           home_object, DONT_ENUM).Assert();
  return *clone;
}


RUNTIME_FUNCTION(Runtime_HomeObjectSymbol) {
  DCHECK(args.length() == 0);
  return isolate->heap()->home_object_symbol();
}


RUNTIME_FUNCTION(Runtime_DefineClass) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 6);
  CONVERT_ARG_HANDLE_CHECKED(Object, name, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, super_class, 1);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, constructor, 2);
  CONVERT_ARG_HANDLE_CHECKED(Script, script, 3);
  CONVERT_SMI_ARG_CHECKED(start_position, 4);
  CONVERT_SMI_ARG_CHECKED(end_position, 5);

  Handle<Object> prototype_parent;
  Handle<Object> constructor_parent;

  if (super_class->IsTheHole()) {
    prototype_parent = isolate->initial_object_prototype();
  } else {
    if (super_class->IsNull()) {
      prototype_parent = isolate->factory()->null_value();
    } else if (super_class->IsSpecFunction()) {
      ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
          isolate, prototype_parent,
          Runtime::GetObjectProperty(isolate, super_class,
                                     isolate->factory()->prototype_string()));
      if (!prototype_parent->IsNull() && !prototype_parent->IsSpecObject()) {
        Handle<Object> args[1] = {prototype_parent};
        THROW_NEW_ERROR_RETURN_FAILURE(
            isolate, NewTypeError("prototype_parent_not_an_object",
                                  HandleVector(args, 1)));
      }
      constructor_parent = super_class;
    } else {
      // TODO(arv): Should be IsConstructor.
      Handle<Object> args[1] = {super_class};
      THROW_NEW_ERROR_RETURN_FAILURE(
          isolate,
          NewTypeError("extends_value_not_a_function", HandleVector(args, 1)));
    }
  }

  Handle<Map> map =
      isolate->factory()->NewMap(JS_OBJECT_TYPE, JSObject::kHeaderSize);
  map->SetPrototype(prototype_parent);
  map->set_constructor(*constructor);
  Handle<JSObject> prototype = isolate->factory()->NewJSObjectFromMap(map);

  Handle<String> name_string = name->IsString()
                                   ? Handle<String>::cast(name)
                                   : isolate->factory()->empty_string();
  constructor->shared()->set_name(*name_string);

  JSFunction::SetPrototype(constructor, prototype);
  PropertyAttributes attribs =
      static_cast<PropertyAttributes>(DONT_ENUM | DONT_DELETE | READ_ONLY);
  RETURN_FAILURE_ON_EXCEPTION(
      isolate, JSObject::SetOwnPropertyIgnoreAttributes(
                   constructor, isolate->factory()->prototype_string(),
                   prototype, attribs));

  // TODO(arv): Only do this conditionally.
  Handle<Symbol> home_object_symbol(isolate->heap()->home_object_symbol());
  RETURN_FAILURE_ON_EXCEPTION(
      isolate, JSObject::SetOwnPropertyIgnoreAttributes(
                   constructor, home_object_symbol, prototype, DONT_ENUM));

  if (!constructor_parent.is_null()) {
    RETURN_FAILURE_ON_EXCEPTION(
        isolate,
        JSObject::SetPrototype(constructor, constructor_parent, false));
  }

  JSObject::AddProperty(prototype, isolate->factory()->constructor_string(),
                        constructor, DONT_ENUM);

  // Install private properties that are used to construct the FunctionToString.
  RETURN_FAILURE_ON_EXCEPTION(
      isolate, Object::SetProperty(constructor,
                                   isolate->factory()->class_script_symbol(),
                                   script, STRICT));
  RETURN_FAILURE_ON_EXCEPTION(
      isolate,
      Object::SetProperty(
          constructor, isolate->factory()->class_start_position_symbol(),
          handle(Smi::FromInt(start_position), isolate), STRICT));
  RETURN_FAILURE_ON_EXCEPTION(
      isolate, Object::SetProperty(
                   constructor, isolate->factory()->class_end_position_symbol(),
                   handle(Smi::FromInt(end_position), isolate), STRICT));

  return *constructor;
}


RUNTIME_FUNCTION(Runtime_DefineClassMethod) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, function, 2);

  uint32_t index;
  if (name->AsArrayIndex(&index)) {
    RETURN_FAILURE_ON_EXCEPTION(
        isolate, JSObject::SetOwnElement(object, index, function, STRICT));
  } else {
    RETURN_FAILURE_ON_EXCEPTION(
        isolate,
        JSObject::SetOwnPropertyIgnoreAttributes(object, name, function, NONE));
  }
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_ClassGetSourceCode) {
  HandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, fun, 0);

  Handle<Object> script;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, script,
      Object::GetProperty(fun, isolate->factory()->class_script_symbol()));
  if (!script->IsScript()) {
    return isolate->heap()->undefined_value();
  }

  Handle<Symbol> start_position_symbol(
      isolate->heap()->class_start_position_symbol());
  Handle<Object> start_position;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, start_position, Object::GetProperty(fun, start_position_symbol));

  Handle<Symbol> end_position_symbol(
      isolate->heap()->class_end_position_symbol());
  Handle<Object> end_position;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, end_position, Object::GetProperty(fun, end_position_symbol));

  if (!start_position->IsSmi() || !end_position->IsSmi() ||
      !Handle<Script>::cast(script)->HasValidSource()) {
    return isolate->ThrowIllegalOperation();
  }

  Handle<String> source(String::cast(Handle<Script>::cast(script)->source()));
  return *isolate->factory()->NewSubString(
      source, Handle<Smi>::cast(start_position)->value(),
      Handle<Smi>::cast(end_position)->value());
}


static Object* LoadFromSuper(Isolate* isolate, Handle<Object> receiver,
                             Handle<JSObject> home_object, Handle<Name> name) {
  if (home_object->IsAccessCheckNeeded() &&
      !isolate->MayNamedAccess(home_object, name, v8::ACCESS_GET)) {
    isolate->ReportFailedAccessCheck(home_object, v8::ACCESS_GET);
    RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  }

  PrototypeIterator iter(isolate, home_object);
  Handle<Object> proto = PrototypeIterator::GetCurrent(iter);
  if (!proto->IsJSReceiver()) return isolate->heap()->undefined_value();

  LookupIterator it(receiver, name, Handle<JSReceiver>::cast(proto));
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, Object::GetProperty(&it));
  return *result;
}


static Object* LoadElementFromSuper(Isolate* isolate, Handle<Object> receiver,
                                    Handle<JSObject> home_object,
                                    uint32_t index) {
  if (home_object->IsAccessCheckNeeded() &&
      !isolate->MayIndexedAccess(home_object, index, v8::ACCESS_GET)) {
    isolate->ReportFailedAccessCheck(home_object, v8::ACCESS_GET);
    RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  }

  PrototypeIterator iter(isolate, home_object);
  Handle<Object> proto = PrototypeIterator::GetCurrent(iter);
  if (!proto->IsJSReceiver()) return isolate->heap()->undefined_value();

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Object::GetElementWithReceiver(isolate, proto, receiver, index));
  return *result;
}


RUNTIME_FUNCTION(Runtime_LoadFromSuper) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 2);

  return LoadFromSuper(isolate, receiver, home_object, name);
}


RUNTIME_FUNCTION(Runtime_LoadKeyedFromSuper) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 2);

  uint32_t index;
  if (key->ToArrayIndex(&index)) {
    return LoadElementFromSuper(isolate, receiver, home_object, index);
  }

  Handle<Name> name;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, name,
                                     Runtime::ToName(isolate, key));
  if (name->AsArrayIndex(&index)) {
    return LoadElementFromSuper(isolate, receiver, home_object, index);
  }
  return LoadFromSuper(isolate, receiver, home_object, name);
}


static Object* StoreToSuper(Isolate* isolate, Handle<JSObject> home_object,
                            Handle<Object> receiver, Handle<Name> name,
                            Handle<Object> value, StrictMode strict_mode) {
  if (home_object->IsAccessCheckNeeded() &&
      !isolate->MayNamedAccess(home_object, name, v8::ACCESS_SET)) {
    isolate->ReportFailedAccessCheck(home_object, v8::ACCESS_SET);
    RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  }

  PrototypeIterator iter(isolate, home_object);
  Handle<Object> proto = PrototypeIterator::GetCurrent(iter);
  if (!proto->IsJSReceiver()) return isolate->heap()->undefined_value();

  LookupIterator it(receiver, name, Handle<JSReceiver>::cast(proto));
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Object::SetProperty(&it, value, strict_mode,
                          Object::CERTAINLY_NOT_STORE_FROM_KEYED,
                          Object::SUPER_PROPERTY));
  return *result;
}


static Object* StoreElementToSuper(Isolate* isolate,
                                   Handle<JSObject> home_object,
                                   Handle<Object> receiver, uint32_t index,
                                   Handle<Object> value,
                                   StrictMode strict_mode) {
  if (home_object->IsAccessCheckNeeded() &&
      !isolate->MayIndexedAccess(home_object, index, v8::ACCESS_SET)) {
    isolate->ReportFailedAccessCheck(home_object, v8::ACCESS_SET);
    RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  }

  PrototypeIterator iter(isolate, home_object);
  Handle<Object> proto = PrototypeIterator::GetCurrent(iter);
  if (!proto->IsJSReceiver()) return isolate->heap()->undefined_value();

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Object::SetElementWithReceiver(isolate, proto, receiver, index, value,
                                     strict_mode));
  return *result;
}


RUNTIME_FUNCTION(Runtime_StoreToSuper_Strict) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 3);

  return StoreToSuper(isolate, home_object, receiver, name, value, STRICT);
}


RUNTIME_FUNCTION(Runtime_StoreToSuper_Sloppy) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 3);

  return StoreToSuper(isolate, home_object, receiver, name, value, SLOPPY);
}


static Object* StoreKeyedToSuper(Isolate* isolate, Handle<JSObject> home_object,
                                 Handle<Object> receiver, Handle<Object> key,
                                 Handle<Object> value, StrictMode strict_mode) {
  uint32_t index;

  if (key->ToArrayIndex(&index)) {
    return StoreElementToSuper(isolate, home_object, receiver, index, value,
                               strict_mode);
  }
  Handle<Name> name;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, name,
                                     Runtime::ToName(isolate, key));
  if (name->AsArrayIndex(&index)) {
    return StoreElementToSuper(isolate, home_object, receiver, index, value,
                               strict_mode);
  }
  return StoreToSuper(isolate, home_object, receiver, name, value, strict_mode);
}


RUNTIME_FUNCTION(Runtime_StoreKeyedToSuper_Strict) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 3);

  return StoreKeyedToSuper(isolate, home_object, receiver, key, value, STRICT);
}


RUNTIME_FUNCTION(Runtime_StoreKeyedToSuper_Sloppy) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, home_object, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 3);

  return StoreKeyedToSuper(isolate, home_object, receiver, key, value, SLOPPY);
}


RUNTIME_FUNCTION(Runtime_DefaultConstructorSuperCall) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 0);

  // Compute the frame holding the arguments.
  JavaScriptFrameIterator it(isolate);
  it.AdvanceToArgumentsFrame();
  JavaScriptFrame* frame = it.frame();

  Handle<JSFunction> function(frame->function(), isolate);
  Handle<Object> receiver(frame->receiver(), isolate);

  Handle<Object> proto_function;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, proto_function,
                                     Runtime::GetPrototype(isolate, function));

  // Get the actual number of provided arguments.
  const int argc = frame->ComputeParametersCount();

  // Loose upper bound to allow fuzzing. We'll most likely run out of
  // stack space before hitting this limit.
  static int kMaxArgc = 1000000;
  RUNTIME_ASSERT(argc >= 0 && argc <= kMaxArgc);

  // If there are too many arguments, allocate argv via malloc.
  const int argv_small_size = 10;
  Handle<Object> argv_small_buffer[argv_small_size];
  SmartArrayPointer<Handle<Object> > argv_large_buffer;
  Handle<Object>* argv = argv_small_buffer;
  if (argc > argv_small_size) {
    argv = new Handle<Object>[argc];
    if (argv == NULL) return isolate->StackOverflow();
    argv_large_buffer = SmartArrayPointer<Handle<Object> >(argv);
  }

  for (int i = 0; i < argc; ++i) {
    argv[i] = handle(frame->GetParameter(i), isolate);
  }

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Execution::Call(isolate, proto_function, receiver, argc, argv, false));
  return *result;
}
}
}  // namespace v8::internal
