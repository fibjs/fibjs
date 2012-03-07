#if !defined(h_738a42b1_1502_4483_948a_a69e7bbbee6a)
#define h_738a42b1_1502_4483_948a_a69e7bbbee6a

#include <string>
#include <map>
#include <memory>
#include "Portability.hh"
#include "Appender.hh"
#include "FactoryParams.hh"

namespace log4cpp
{
   class LOG4CPP_EXPORT AppendersFactory
   {
      public:
         typedef FactoryParams params_t;
         typedef std::auto_ptr<Appender> (*create_function_t)(const params_t& params);

         static AppendersFactory& getInstance();
         void registerCreator(const std::string& class_name, create_function_t create_function);
         std::auto_ptr<Appender> create(const std::string& class_name, const params_t& params);
         bool registered(const std::string& class_name) const;

      private:
         AppendersFactory(){};

         typedef std::map<std::string, create_function_t> creators_t;
         typedef creators_t::const_iterator const_iterator;

         creators_t creators_;
   };
}

#endif // h_738a42b1_1502_4483_948a_a69e7bbbee6a
