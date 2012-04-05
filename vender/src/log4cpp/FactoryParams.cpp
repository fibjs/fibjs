#include <log4cpp/FactoryParams.hh>
#include <stdexcept>

namespace log4cpp
{
   const std::string& FactoryParams::operator[](const std::string& v) const
   {
      const_iterator i = storage_.find(v);
      if (i != storage_.end())
         return i->second;

      throw std::invalid_argument("There is no parameter '" + v + "'");
   }
   
   FactoryParams::const_iterator FactoryParams::find(const std::string& v) const
   {
      return storage_.find(v);
   }
}

