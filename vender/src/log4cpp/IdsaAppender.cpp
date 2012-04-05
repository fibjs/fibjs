/*
 * IdsaAppender.cpp
 *
 * Copyright 2000, Marc Welz
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"

#ifdef LOG4CPP_HAVE_LIBIDSA

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log4cpp/IdsaAppender.hh>
#include <log4cpp/FactoryParams.hh>
#include <memory>

namespace log4cpp {

    IdsaAppender::IdsaAppender(const std::string& name, 
                               const std::string& idsaName) :
        AppenderSkeleton(name),
        _idsaName(idsaName)
    {
        _idsaConnection=NULL;
        open();
    }
    
    IdsaAppender::~IdsaAppender() {
        close();
    }

    void IdsaAppender::open() {
        _idsaConnection=idsa_open((char *)_idsaName.c_str(), NULL,
                                  IDSA_F_FAILOPEN);
    }

    void IdsaAppender::close() {
        idsa_close(_idsaConnection);
        _idsaConnection=NULL;
    }

    void IdsaAppender::_append(const LoggingEvent& event) {
        IDSA_EVENT *idsaEvent;
        
        idsaEvent = idsa_event(_idsaConnection);
        
        if (idsaEvent){
            
            idsa_name(idsaEvent,(char *)event.categoryName.c_str());
            idsa_scheme(idsaEvent,"log4cpp");
            
            idsa_add_integer(idsaEvent, "priority", event.priority);
            idsa_add_string(idsaEvent, "ndc", (char *)event.ndc.c_str());
            idsa_add_string(idsaEvent, "message", 
                            (char*)event.message.c_str());
            
            idsa_log(_idsaConnection,idsaEvent);
            // idsa_log does its own deallocation */
        }
    }

    bool IdsaAppender::reopen() {
        close();
        open();
        return true;
    }      

    bool IdsaAppender::requiresLayout() const {
        return false;
    }

    void IdsaAppender::setLayout(Layout* layout) {
        return;
    }

    std::auto_ptr<Appender> create_idsa_appender(const FactoryParams& params)
    {
       std::string name, idsa_name;
       params.get_for("idsa appender").required("name", name)("idsa_name", idsa_name);

       return std::auto_ptr<Appender>(new IdsaAppender(name, idsa_name));
    }
}

#endif // LOG4CPP_HAVE_LIBIDSA
