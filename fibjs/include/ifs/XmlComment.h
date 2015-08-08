/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlComment_base_H_
#define _XmlComment_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlCharacterData.h"

namespace fibjs
{

class XmlCharacterData_base;

class XmlComment_base : public XmlCharacterData_base
{
    DECLARE_CLASS(XmlComment_base);

public:

};

}


namespace fibjs
{
    inline ClassInfo& XmlComment_base::class_info()
    {
        static ClassData s_cd = 
        { 
            "XmlComment", NULL, 
            0, NULL, 0, NULL, 0, NULL, NULL, NULL,
            &XmlCharacterData_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


}

#endif

