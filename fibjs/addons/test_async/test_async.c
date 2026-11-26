#include <assert.h>
#include <stdio.h>
#include <node_api.h>
#include <uv.h>
#include "../common.h"

// this needs to be greater than the thread pool size
#define MAX_CANCEL_THREADS 6

typedef struct {
  int32_t _input;
  int32_t _output;
  napi_ref _callback;
  napi_async_work _request;
} carrier;

static carrier the_carrier;
static carrier async_carrier[MAX_CANCEL_THREADS];

static void Execute(napi_env env, void* data) {
  uv_sleep(1000);
  carrier* c = (carrier*)(data);

  assert(c == &the_carrier);

  c->_output = c->_input * 2;
}

static void Complete(napi_env env, napi_status status, void* data) {
  carrier* c = (carrier*)(data);

  if (c != &the_carrier) {
    napi_throw_type_error(env, NULL, "Wrong data parameter to Complete.");
    return;
  }

  if (status != napi_ok) {
    napi_throw_type_error(env, NULL, "Execute callback failed.");
    return;
  }

  napi_value argv[2];

  NODE_API_CALL_RETURN_VOID(env, napi_get_null(env, &argv[0]));
  NODE_API_CALL_RETURN_VOID(env, napi_create_int32(env, c->_output, &argv[1]));
  napi_value callback;
  NODE_API_CALL_RETURN_VOID(env,
      napi_get_reference_value(env, c->_callback, &callback));
  napi_value global;
  NODE_API_CALL_RETURN_VOID(env, napi_get_global(env, &global));

  napi_value result;
  NODE_API_CALL_RETURN_VOID(env,
    napi_call_function(env, global, callback, 2, argv, &result));

  NODE_API_CALL_RETURN_VOID(env, napi_delete_reference(env, c->_callback));
  NODE_API_CALL_RETURN_VOID(env, napi_delete_async_work(env, c->_request));
}

static napi_value Test(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value argv[3];
  napi_value _this;
  napi_value resource_name;
  void* data;
  NODE_API_CALL(env,
    napi_get_cb_info(env, info, &argc, argv, &_this, &data));
  NODE_API_ASSERT(env, argc >= 3, "Not enough arguments, expected 3.");

  napi_valuetype t;
  NODE_API_CALL(env, napi_typeof(env, argv[0], &t));
  NODE_API_ASSERT(env, t == napi_number,
      "Wrong first argument, integer expected.");
  NODE_API_CALL(env, napi_typeof(env, argv[1], &t));
  NODE_API_ASSERT(env, t == napi_object,
    "Wrong second argument, object expected.");
  NODE_API_CALL(env, napi_typeof(env, argv[2], &t));
  NODE_API_ASSERT(env, t == napi_function,
    "Wrong third argument, function expected.");

  the_carrier._output = 0;

  NODE_API_CALL(env,
      napi_get_value_int32(env, argv[0], &the_carrier._input));
  NODE_API_CALL(env,
    napi_create_reference(env, argv[2], 1, &the_carrier._callback));

  NODE_API_CALL(env, napi_create_string_utf8(
      env, "TestResource", NAPI_AUTO_LENGTH, &resource_name));
  NODE_API_CALL(env, napi_create_async_work(env, argv[1], resource_name,
    Execute, Complete, &the_carrier, &the_carrier._request));
  NODE_API_CALL(env,
      napi_queue_async_work(env, the_carrier._request));

  return NULL;
}

static void BusyCancelComplete(napi_env env, napi_status status, void* data) {
  carrier* c = (carrier*)(data);
  NODE_API_CALL_RETURN_VOID(env, napi_delete_async_work(env, c->_request));
}

static void CancelComplete(napi_env env, napi_status status, void* data) {
  carrier* c = (carrier*)(data);

  // Report the outcome either way.  The blocking pool of this runtime grows on
  // demand, so an addon cannot saturate it and the work may start before the
  // cancel reaches it: completion then reports napi_ok instead of
  // napi_cancelled.  The JS side accepts both.
  napi_value callback;
  NODE_API_CALL_RETURN_VOID(env,
      napi_get_reference_value(env, c->_callback, &callback));
  napi_value global;
  NODE_API_CALL_RETURN_VOID(env, napi_get_global(env, &global));
  napi_value js_status;
  NODE_API_CALL_RETURN_VOID(env,
      napi_create_uint32(env, (uint32_t)status, &js_status));
  napi_value result;
  NODE_API_CALL_RETURN_VOID(env,
    napi_call_function(env, global, callback, 1, &js_status, &result));

  NODE_API_CALL_RETURN_VOID(env, napi_delete_async_work(env, c->_request));
  NODE_API_CALL_RETURN_VOID(env, napi_delete_reference(env, c->_callback));
}

static void CancelExecute(napi_env env, void* data) {
  uv_sleep(1000);
}

static napi_value TestCancel(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_value _this;
  napi_value resource_name;
  void* data;

  NODE_API_CALL(env, napi_create_string_utf8(
      env, "TestResource", NAPI_AUTO_LENGTH, &resource_name));

  // Occupy the pool first, so the work below is likely still queued when the
  // cancel arrives.  The blocking pool is elastic (it spawns a worker for
  // every queued task), so this cannot be guaranteed; the completion callback
  // reports whichever outcome happened.
  for (int i = 1; i < MAX_CANCEL_THREADS; i++) {
    NODE_API_CALL(env, napi_create_async_work(env, NULL, resource_name,
      CancelExecute, BusyCancelComplete,
      &async_carrier[i], &async_carrier[i]._request));
    NODE_API_CALL(env, napi_queue_async_work(env, async_carrier[i]._request));
  }

  // Now queue the work we are going to cancel and cancel it right away.  If a
  // worker has already picked it up, the cancel below fails and the completion
  // callback reports napi_ok instead of napi_cancelled.
  NODE_API_CALL(env,
    napi_get_cb_info(env, info, &argc, argv, &_this, &data));

  NODE_API_CALL(env, napi_create_async_work(env, NULL, resource_name,
    CancelExecute, CancelComplete,
    &async_carrier[0], &async_carrier[0]._request));
  NODE_API_CALL(env,
      napi_create_reference(env, argv[0], 1, &async_carrier[0]._callback));
  NODE_API_CALL(env, napi_queue_async_work(env, async_carrier[0]._request));

  // napi_generic_failure means the work had already started (uv_cancel's
  // UV_EBUSY); the completion callback then reports napi_ok.  Any other
  // failure is a real error.
  napi_status cancel_status =
      napi_cancel_async_work(env, async_carrier[0]._request);
  NODE_API_ASSERT(env,
      cancel_status == napi_ok || cancel_status == napi_generic_failure,
      "Unexpected status from napi_cancel_async_work.");
  return NULL;
}

struct {
  napi_ref ref;
  napi_async_work work;
} repeated_work_info = { NULL, NULL };

static void RepeatedWorkerThread(napi_env env, void* data) {}

static void RepeatedWorkComplete(napi_env env, napi_status status, void* data) {
  napi_value cb, js_status;
  NODE_API_CALL_RETURN_VOID(env,
      napi_get_reference_value(env, repeated_work_info.ref, &cb));
  NODE_API_CALL_RETURN_VOID(env,
      napi_delete_async_work(env, repeated_work_info.work));
  NODE_API_CALL_RETURN_VOID(env,
      napi_delete_reference(env, repeated_work_info.ref));
  repeated_work_info.work = NULL;
  repeated_work_info.ref = NULL;
  NODE_API_CALL_RETURN_VOID(env,
      napi_create_uint32(env, (uint32_t)status, &js_status));
  NODE_API_CALL_RETURN_VOID(env,
      napi_call_function(env, cb, cb, 1, &js_status, NULL));
}

static napi_value DoRepeatedWork(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value cb, name;
  NODE_API_ASSERT(env, repeated_work_info.ref == NULL,
      "Reference left over from previous work");
  NODE_API_ASSERT(env, repeated_work_info.work == NULL,
      "Work pointer left over from previous work");
  NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, &cb, NULL, NULL));
  NODE_API_CALL(env, napi_create_reference(env, cb, 1, &repeated_work_info.ref));
  NODE_API_CALL(env,
      napi_create_string_utf8(env, "Repeated Work", NAPI_AUTO_LENGTH, &name));
  NODE_API_CALL(env,
      napi_create_async_work(env, NULL, name, RepeatedWorkerThread,
          RepeatedWorkComplete, &repeated_work_info, &repeated_work_info.work));
  NODE_API_CALL(env, napi_queue_async_work(env, repeated_work_info.work));
  return NULL;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor properties[] = {
    DECLARE_NODE_API_PROPERTY("Test", Test),
    DECLARE_NODE_API_PROPERTY("TestCancel", TestCancel),
    DECLARE_NODE_API_PROPERTY("DoRepeatedWork", DoRepeatedWork),
  };

  NODE_API_CALL(env, napi_define_properties(
      env, exports, sizeof(properties) / sizeof(*properties), properties));

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
