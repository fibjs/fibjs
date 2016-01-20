/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlCDATASection_base_H_
#define _XmlCDATASection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlText.h"

namespace fibjs
{

class XmlText_base;

class XmlCDATASection_base : public XmlText_base
{
    DECLARE_CLASS(XmlCDATASection_base);

public:

};

}


namespace fibjs
{
    inline ClassInfo& XmlCDATASection_base::class_info()
    {
        static ClassData s_cd = 
        { 
            "XmlCDATASection", NULL, NULL, 
            0, NULL, 0, NULL, 0, NULL, NULL, NULL,
            &XmlText_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


}

#endif

