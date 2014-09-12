/*
 * XmlCDATASection.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlCDATASection.h"
#include "XmlNodeImpl.h"
#include "XmlDataImpl.h"

#ifndef XMLCDATASECTION_H_
#define XMLCDATASECTION_H_

namespace fibjs
{

class XmlCDATASection: public XmlCDATASection_base, public XmlNodeImpl
{
public:
    XmlCDATASection(XmlDocument_base *document, const char *data):
        XmlNodeImpl(document, this, xml_base::_CDATA_SECTION_NODE), m_data(data)
    {
    }

    XmlCDATASection(XmlDocument_base *document, std::string &data):
        XmlNodeImpl(document, this, xml_base::_CDATA_SECTION_NODE), m_data(data)
    {
    }

public:
    // XmlNode_base
    virtual result_t get_nodeName(std::string &retVal);
    virtual result_t get_nodeValue(std::string &retVal);
    virtual result_t set_nodeValue(const char *newVal);
    virtual result_t get_nodeType(int32_t &retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base> &retVal);
    virtual result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t hasChildNodes(bool &retVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal);
    virtual result_t normalize();

public:
    // XmlCharacterData_base
    virtual result_t get_data(std::string &retVal);
    virtual result_t set_data(const char *newVal);
    virtual result_t get_length(int32_t &retVal);
    virtual result_t substringData(int32_t offset, int32_t count, std::string &retVal);
    virtual result_t appendData(const char *arg);
    virtual result_t insertData(int32_t offset, const char *arg);
    virtual result_t deleteData(int32_t offset, int32_t count);
    virtual result_t replaceData(int32_t offset, int32_t count, const char *arg);

public:
    // XmlText_base
    virtual result_t splitText(int32_t offset, obj_ptr<XmlText_base> &retVal);

private:
    XmlDataImpl m_data;
};

} /* namespace fibjs */
#endif /* XMLCDATASECTION_H_ */
