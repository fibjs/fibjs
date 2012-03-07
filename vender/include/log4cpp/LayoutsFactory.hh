#if !defined(h_409ac787_0acf_47ff_ac15_3e9024d40315)
#define h_409ac787_0acf_47ff_ac15_3e9024d40315

#include <string>
#include <map>
#include <memory>
#include "Portability.hh"
#include "Layout.hh"
#include "FactoryParams.hh"

namespace log4cpp
{
   class LOG4CPP_EXPORT LayoutsFactory
   {
      public:
         typedef FactoryParams params_t;
         typedef std::auto_ptr<Layout> (*create_function_t)(const params_t& params);

         static LayoutsFactory& getInstance();
         void registerCreator(const std::string& class_name, create_function_t create_function);
         std::auto_ptr<Layout> create(const std::string& class_name, const params_t& params);
         bool registed(const std::string& class_name) const;

      private:
         LayoutsFactory(){};

         typedef std::map<std::string, create_function_t> creators_t;
         typedef creators_t::const_iterator const_iterator;

         creators_t creators_;
   };
}


#endif // h_409ac787_0acf_47ff_ac15_3e9024d40315
