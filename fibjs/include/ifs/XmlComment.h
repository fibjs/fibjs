/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/XmlCharacterData.h"

namespace fibjs {

class XmlCharacterData_base;

class XmlComment_base : public XmlCharacterData_base {
    DECLARE_CLASS(XmlComment_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

namespace fibjs {
inline ClassInfo& XmlComment_base::class_info()
{
    static ClassData s_cd = {
        "XmlComment", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &XmlCharacterData_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
