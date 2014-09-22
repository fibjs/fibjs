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

namespace fibjs
{

class XmlElement;

class XmlAttr: public XmlAttr_base
{
public:
    XmlAttr(XmlElement *owner, const char *name, const char *value):
        m_owner(owner), m_name(name), m_localName(m_name), m_value(value)
    {
    }

    XmlAttr(XmlElement *owner, const char *namespaceURI, const char *qualifiedName,
            const char *value):
        m_owner(owner), m_name(qualifiedName), m_namespaceURI(namespaceURI), m_value(value)
    {
        const char *p = qstrchr(qualifiedName, ':');
        if (!p)
            m_localName = m_name;
        else
        {
            m_prefix.assign(qualifiedName, p - qualifiedName);
            m_localName.assign(p + 1);
        }
    }

    XmlAttr(const XmlAttr &from):
        m_owner(NULL), m_name(from.m_name), m_localName(from.m_localName),
        m_prefix(from.m_prefix), m_namespaceURI(from.m_namespaceURI), m_value(from.m_value)
    {
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlAttr_base
    virtual result_t get_localName(std::string &retVal);
    virtual result_t get_value(std::string &retVal);
    virtual result_t set_value(const char *newVal);
    virtual result_t get_name(std::string &retVal);
    virtual result_t get_namespaceURI(std::string &retVal);
    virtual result_t get_prefix(std::string &retVal);
    virtual result_t set_prefix(const char *newVal);

public:
    bool check(const char *namespaceURI, const char *localName)
    {
        return !qstrcmp(m_namespaceURI.c_str(), namespaceURI) &&
               !qstrcmp(m_localName.c_str(), localName);
    }

    bool check(const char *name)
    {
        return !qstrcmp(m_name.c_str(), name);
    }

    bool check(const XmlAttr *from)
    {
        if (m_namespaceURI.empty() && from->m_namespaceURI.empty())
            return check(from->m_name.c_str());
        return check(from->m_namespaceURI.c_str(), from->m_localName.c_str());
    }

    bool check_namespaceURI(const char *namespaceURI)
    {
        return !qstrcmp(m_namespaceURI.c_str(), "http://www.w3.org/2000/xmlns/") &&
               !qstrcmp(m_prefix.c_str(), "xmlns") &&
               !qstrcmp(m_value.c_str(), namespaceURI);
    }

    bool check_prefix(const char *prefix)
    {
        return !qstrcmp(m_namespaceURI.c_str(), "http://www.w3.org/2000/xmlns/") &&
               !qstrcmp(m_prefix.c_str(), "xmlns") &&
               !qstrcmp(m_localName.c_str(), prefix);
    }

    void fix_prefix();

public:
    XmlElement *m_owner;

private:
    std::string m_name;
    std::string m_localName;
    std::string m_prefix;
    std::string m_namespaceURI;
    std::string m_value;
};

} /* namespace fibjs */
#endif /* XMLATTR_H_ */
