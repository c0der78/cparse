#ifndef ARG3_CPARSE_TYPE_DATE_H_
#define ARG3_CPARSE_TYPE_DATE_H_

#include <ctime>
#include <string>
#include "parsetype.h"

using namespace std;

namespace cparse
{
    namespace type
    {
        class Date : public ParseType
        {
        public:
            Date(const string &date);
            Date(time_t value);
            Date();
            Date(const JSON &obj);
            virtual ~Date();
            Date(const Date &value);
            Date(Date &&value);
            Date &operator=(const Date &value);
            Date &operator=(Date && value);

            JSON toJSON() const;
            void fromJSON(const JSON &attributes);

            time_t getTimestamp() const;

            string toString() const;

            void fromString(const string &value);

        protected:
            static const char *const FORMAT;
        private:
            time_t value_;
        };
    }
}

#endif
