/*
 * HierarchyMaintainer.hh
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4CPP_HIERARCHYMAINTAINER_HH
#define _LOG4CPP_HIERARCHYMAINTAINER_HH

#include <log4cpp/Portability.hh>
#include <string>
#include <map>
#include <vector>
#include <log4cpp/Category.hh>
#include <log4cpp/threading/Threading.hh>

namespace log4cpp {

    /**
     * HierarchyMaintainer is an internal log4cpp class. It is responsible
     * for maintaining the hierarchy of Categories. Applications should
     * not have to use this class directly.
     **/
    class HierarchyMaintainer {
        friend class Log4cppCleanup;

        public:
        typedef std::map<std::string, Category*> CategoryMap;
        typedef void (*shutdown_fun_ptr)();
  
        static HierarchyMaintainer& getDefaultMaintainer();

        HierarchyMaintainer();
        virtual ~HierarchyMaintainer();
        virtual Category* getExistingInstance(const std::string& name);
        virtual Category& getInstance(const std::string& name);
        virtual std::vector<Category*>* getCurrentCategories() const;
        virtual void shutdown();
        void register_shutdown_handler(shutdown_fun_ptr handler);
        virtual void deleteAllCategories();

        protected:
        virtual Category* _getExistingInstance(const std::string& name);
        virtual Category& _getInstance(const std::string& name);
        CategoryMap _categoryMap;
        mutable threading::Mutex _categoryMutex;

        private:
        typedef std::vector<shutdown_fun_ptr> handlers_t;
     
        static HierarchyMaintainer* _defaultMaintainer;
        handlers_t handlers_;
    };        
}

#endif // _LOG4CPP_HIERARCHYMAINTAINER_HH
