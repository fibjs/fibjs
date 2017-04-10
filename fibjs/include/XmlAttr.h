/*
 * XmlAttr.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlAttr.h"
#include "XmlNodeImpl.h"

#ifndef XMLATTR_H_
#define XMLATTR_H_

namespace fibjs {

class XmlElement;

class XmlAttr : public XmlAttr_base {
public:
    XmlAttr(XmlElement* owner, exlib::string name, exlib::string value)
        : m_owner(owner)
        , m_name(name)
        , m_localName(m_name)
        , m_value(value)
    {
    }

    XmlAttr(XmlElement* owner, exlib::string namespaceURI, exlib::string qualifiedName,
        exlib::string value)
        : m_owner(owner)
        , m_name(qualifiedName)
        , m_namespaceURI(namespaceURI)
        , m_value(value)
    {
        const char* c_str = qualifiedName.c_str();
        const char* p = qstrchr(c_str, ':');
        if (!p)
            m_localName = m_name;
        else {
            m_prefix.assign(c_str, p - c_str);
            m_localName.assign(p + 1);
        }
    }

    XmlAttr(const XmlAttr& from)
        : m_owner(NULL)
        , m_name(from.m_name)
        , m_localName(from.m_localName)
        , m_prefix(from.m_prefix)
        , m_namespaceURI(from.m_namespaceURI)
        , m_value(from.m_value)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlAttr_base
    virtual result_t get_localName(exlib::string& retVal);
    virtual result_t get_value(exlib::string& retVal);
    virtual result_t set_value(exlib::string newVal);
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_namespaceURI(exlib::string& retVal);
    virtual result_t get_prefix(exlib::string& retVal);
    virtual result_t set_prefix(exlib::string newVal);
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);

public:
    bool check(exlib::string namespaceURI, exlib::string localName)
    {
        return (m_namespaceURI == namespaceURI) && (m_localName == localName);
    }

    bool check(exlib::string name)
    {
        return (m_name == name);
    }

    bool check(const XmlAttr* from)
    {
        if (m_namespaceURI.empty() && from->m_namespaceURI.empty())
            return check(from->m_name);
        return check(from->m_namespaceURI, from->m_localName);
    }

    bool check_namespaceURI(exlib::string namespaceURI)
    {
        return (m_namespaceURI == "http://www.w3.org/2000/xmlns/") && (m_prefix == "xmlns") && (m_value == namespaceURI);
    }

    bool check_prefix(exlib::string prefix)
    {
        return (m_namespaceURI == "http://www.w3.org/2000/xmlns/") && (m_prefix == "xmlns") && (m_localName == prefix);
    }

    void fix_prefix();

public:
    XmlElement* m_owner;

private:
    exlib::string m_name;
    exlib::string m_localName;
    exlib::string m_prefix;
    exlib::string m_namespaceURI;
    exlib::string m_value;
};

} /* namespace fibjs */
#endif /* XMLATTR_H_ */
