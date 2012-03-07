#if !defined(h_3491ecd0_3891_4902_b3ba_15b15d98ae49)
#define h_3491ecd0_3891_4902_b3ba_15b15d98ae49

#include <log4cpp/TriggeringEventEvaluator.hh>

namespace log4cpp
{
   class LOG4CPP_EXPORT LevelEvaluator : public TriggeringEventEvaluator
   {
      public:
         LevelEvaluator(Priority::Value level) : level_(level) {}
         virtual bool eval(const LoggingEvent& event) const { return event.priority <= level_; }
   
      private:
         Priority::Value level_;
   };
}

#endif // h_3491ecd0_3891_4902_b3ba_15b15d98ae49
