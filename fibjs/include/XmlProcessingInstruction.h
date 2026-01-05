/*
 * XmlProcessingInstruction.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/XmlProcessingInstruction.h"
#include "XmlNodeMixin.h"

namespace fibjs {

class XmlProcessingInstruction : public XmlNodeMixin<XmlProcessingInstruction, XmlProcessingInstruction_base> {
public:
    XmlProcessingInstruction(XmlDocument_base* document, exlib::string target, exlib::string data)
        : XmlNodeMixin<XmlProcessingInstruction, XmlProcessingInstruction_base>(document, this, xml_base::C_PROCESSING_INSTRUCTION_NODE)
        , m_target(target)
        , m_data(data)
    {
    }

    XmlProcessingInstruction(const XmlProcessingInstruction& from)
        : XmlNodeMixin<XmlProcessingInstruction, XmlProcessingInstruction_base>(from.m_document, this, xml_base::C_PROCESSING_INSTRUCTION_NODE)
        , m_target(from.m_target)
        , m_data(from.m_data)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base - custom implementations
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);

public:
    // XmlProcessingInstruction_base
    virtual result_t get_target(exlib::string& retVal);
    virtual result_t get_data(exlib::string& retVal);
    virtual result_t set_data(exlib::string newVal);

private:
    exlib::string m_target;
    exlib::string m_data;
};

} /* namespace fibjs */
