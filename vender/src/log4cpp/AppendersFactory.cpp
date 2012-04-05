#include <log4cpp/AppendersFactory.hh>
#include <stdexcept>

namespace log4cpp
{
   static AppendersFactory* appenders_factory_ = 0;

   std::auto_ptr<Appender> create_file_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_roll_file_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_idsa_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_nt_event_log_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_remote_syslog_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_syslog_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_win32_debug_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_abort_appender(const FactoryParams&);
   std::auto_ptr<Appender> create_smtp_appender(const FactoryParams&);

   AppendersFactory& AppendersFactory::getInstance()
   {
      if (!appenders_factory_)
      {
         std::auto_ptr<AppendersFactory> af(new AppendersFactory);
         
         af->registerCreator("file", &create_file_appender);
         af->registerCreator("roll file", &create_roll_file_appender);
         af->registerCreator("remote syslog", &create_remote_syslog_appender);
         af->registerCreator("abort", &create_abort_appender);

#if defined(LOG4CPP_HAVE_LIBIDSA)
         af->registerCreator("idsa", &create_idsa_appender);
#endif

#if defined(LOG4CPP_HAVE_SYSLOG)
         af->registerCreator("syslog", &create_syslog_appender);
#endif

#if defined(WIN32)
         af->registerCreator("win32 debug", &create_win32_debug_appender);
         af->registerCreator("nt event log", &create_nt_event_log_appender);
#endif

#if defined(LOG4CPP_HAVE_BOOST)
#include <boost/version.hpp>
#if BOOST_VERSION >= 103500
         af->registerCreator("smtp", &create_smtp_appender);
#endif // LOG4CPP_HAVE_BOOST
#endif // BOOST_VERSION >= 103500

         appenders_factory_ = af.release();
      }

      return *appenders_factory_;
   }

   void AppendersFactory::registerCreator(const std::string& class_name, create_function_t create_function)
   {
      const_iterator i = creators_.find(class_name);
      if (i != creators_.end())
         throw std::invalid_argument("Appender creator for type name '" + class_name + "' allready registered");

      creators_[class_name] = create_function;
   }

   std::auto_ptr<Appender> AppendersFactory::create(const std::string& class_name, const params_t& params)
   {
      const_iterator i = creators_.find(class_name);
      if (i == creators_.end())
         throw std::invalid_argument("There is no appender with type name '" + class_name + "'");

      return (*i->second)(params);
   }

   bool AppendersFactory::registered(const std::string& class_name) const
   {
      return creators_.end() != creators_.find(class_name);
   }
}
