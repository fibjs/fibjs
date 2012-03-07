#if !defined(h_fb88639f_85c9_481a_a3a0_f25ae8bac24c)
#define h_fb88639f_85c9_481a_a3a0_f25ae8bac24c

#include <log4cpp/LoggingEvent.hh>

namespace log4cpp
{
   class LOG4CPP_EXPORT TriggeringEventEvaluator
   {
      public:
         virtual bool eval(const LoggingEvent& event) const = 0;
         virtual ~TriggeringEventEvaluator() {}
   };
}

#endif // h_fb88639f_85c9_481a_a3a0_f25ae8bac24c
