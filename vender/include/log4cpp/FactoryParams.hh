#if !defined(h_3e645482_ae6a_43e5_8f81_abbc4200212d)
#define h_3e645482_ae6a_43e5_8f81_abbc4200212d

#include <map>
#include <string>
#include <sstream>
#include <stdexcept>
#include "Portability.hh"

namespace log4cpp
{
   class FactoryParams;
   namespace details
   {
      class base_validator_data
      {
         public:
            base_validator_data(const char* tag, const FactoryParams* params) : tag_(tag), params_(params){}

         protected:
            const char* tag_;
            const FactoryParams* params_;

            template<typename T>
            void assign(const std::string& param_value, T& value) const
            {
               assign_impl(param_value, value);
            }

            template<typename T>
            void assign_impl(const std::string& param_value, T& value) const
            {
               std::stringstream s;
               s << param_value;
               s >> value;
            }

            void assign_impl(const std::string& param_value, std::string& value) const
            {
               value = param_value;
            }

            void throw_error(const char* param_name) const
            {
               std::stringstream s;
               s << "Property '" << param_name << "' required to configure " << tag_;
               throw std::runtime_error(s.str());
            }
      };

      class optional_params_validator;
      class required_params_validator : public base_validator_data
      {
         public:
            required_params_validator(const char* tag, const FactoryParams* params) : base_validator_data(tag, params) {}

            template<typename T>
            optional_params_validator optional(const char* param, T& value) const;
            
            template<typename T>
            const required_params_validator& operator()(const char* param, T& value) const;
      };
      
      class optional_params_validator : public base_validator_data
      {
         public:
            optional_params_validator(const char* tag, const FactoryParams* params) : base_validator_data(tag, params) {}

            template<typename T>
            required_params_validator required(const char* param, T& value) const { required_params_validator v(tag_, params_); v(param, value); return v; }

            template<typename T>
            const optional_params_validator& operator()(const char* param, T& value) const;
      };

      template<typename T>
      optional_params_validator required_params_validator::optional(const char* param, T& value) const { optional_params_validator v(tag_, params_); v(param, value); return v; }

      class parameter_validator : public base_validator_data
      {
         public:
            parameter_validator(const char* tag, const FactoryParams* params) : base_validator_data(tag, params) {}

            template<typename T>
            required_params_validator required(const char* param, T& value) const { required_params_validator v(tag_, params_); v(param, value); return v; }

            template<typename T>
            optional_params_validator optional(const char* param, T& value) const { optional_params_validator v(tag_, params_); v(param, value); return v; }
      };
   }

   class LOG4CPP_EXPORT FactoryParams
   {
         typedef std::map<std::string, std::string> storage_t;
		 
		 		 storage_t storage_;
      
      public:
         typedef storage_t::const_iterator const_iterator;

         const std::string& operator[](const std::string& v) const;
         std::string& operator[](const std::string& v) { return storage_[v]; }
         details::parameter_validator get_for(const char* tag) const { return details::parameter_validator(tag, this); }
         const_iterator find(const std::string& t) const;
         const_iterator begin() const { return storage_.begin(); }
         const_iterator end() const { return storage_.end(); }

      private:
         /*typedef std::map<std::string, std::string> storage_t;

         storage_t storage_; */
   };

   namespace details
   {
      template<typename T>
      const required_params_validator& required_params_validator::operator()(const char* param, T& value) const
      {
         FactoryParams::const_iterator i = params_->find(param);
         if (i != params_->end())
            assign(i->second, value);
         else
            throw_error(param);

         return *this;
      }
      
      template<typename T>
      const optional_params_validator& optional_params_validator::operator()(const char* param, T& value) const
      {
         FactoryParams::const_iterator i = params_->find(param);
         if (i != params_->end())
            assign(i->second, value);

         return *this;
      }
   }
}

#endif // h_3e645482_ae6a_43e5_8f81_abbc4200212d
