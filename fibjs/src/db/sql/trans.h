#include "ifs/DbConnection.h"

namespace fibjs {

inline result_t _trans(DbConnection_base* db, v8::Local<v8::Function> func, bool& retVal)
{
    result_t hr = 0;
    retVal = false;

    hr = db->ac_begin();
    if (hr < 0)
        return hr;

    db->leave();
    v8::Local<v8::Value> result = func->Call(db->wrap(), 0, NULL);
    db->enter();

    if (result.IsEmpty()) {
        db->ac_rollback();
        return CALL_E_JAVASCRIPT;
    }

    if (result->IsFalse())
        return db->ac_rollback();
    else {
        retVal = true;
        return db->ac_commit();
    }
}
}
