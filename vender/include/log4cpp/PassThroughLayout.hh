#if !defined(h_8e4861a3_f607_479c_ac2d_0b2d81b4c36c)
#define h_8e4861a3_f607_479c_ac2d_0b2d81b4c36c

#include <log4cpp/Layout.hh>

namespace log4cpp
{
   class PassThroughLayout : public Layout
   {
      public:
         virtual std::string format(const LoggingEvent& event) { return event.message; }
   };
}

#endif // h_8e4861a3_f607_479c_ac2d_0b2d81b4c36c
