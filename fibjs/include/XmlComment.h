/*
 * XmlComment.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/XmlComment.h"
#include "XmlNodeMixin.h"
#include "XmlDataImpl.h"

namespace fibjs {

class XmlComment : public XmlNodeMixin<XmlComment, XmlComment_base> {
public:
    XmlComment(XmlDocument_base* document, exlib::string data)
        : XmlNodeMixin<XmlComment, XmlComment_base>(document, xml_base::C_COMMENT_NODE)
        , m_data(data)
    {
    }

    XmlComment(const XmlComment& from)
        : XmlNodeMixin<XmlComment, XmlComment_base>(from)
        , m_data(from.m_data)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base - only override methods with custom implementations
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);

public:
    // XmlCharacterData_base
    virtual result_t get_data(exlib::string& retVal);
    virtual result_t set_data(exlib::string newVal);
    virtual result_t get_length(int32_t& retVal);
    virtual result_t substringData(int32_t offset, int32_t count, exlib::string& retVal);
    virtual result_t appendData(exlib::string arg);
    virtual result_t insertData(int32_t offset, exlib::string arg);
    virtual result_t deleteData(int32_t offset, int32_t count);
    virtual result_t replaceData(int32_t offset, int32_t count, exlib::string arg);

private:
    XmlDataImpl m_data;
};

} /* namespace fibjs */
