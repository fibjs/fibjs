#if !defined(h_dd233b8b_5c59_4956_9393_6581c95f9779)
#define h_dd233b8b_5c59_4956_9393_6581c95f9779

#include <string>
#include <map>
#include <memory>
#include "Portability.hh"
#include "TriggeringEventEvaluator.hh"
#include "FactoryParams.hh"

namespace log4cpp
{
   class LOG4CPP_EXPORT TriggeringEventEvaluatorFactory
   {
      public:
         typedef FactoryParams params_t;
         typedef std::auto_ptr<TriggeringEventEvaluator> (*create_function_t)(const params_t& params);

         static TriggeringEventEvaluatorFactory& getInstance();
         void registerCreator(const std::string& class_name, create_function_t create_function);
         std::auto_ptr<TriggeringEventEvaluator> create(const std::string& class_name, const params_t& params);
         bool registered(const std::string& class_name) const;

      private:
         TriggeringEventEvaluatorFactory(){};

         typedef std::map<std::string, create_function_t> creators_t;
         typedef creators_t::const_iterator const_iterator;

         creators_t creators_;
   };
}

#endif // h_dd233b8b_5c59_4956_9393_6581c95f9779
