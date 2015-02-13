#include <cparse/type/date.h>
#include <cparse/exception.h>
#include "protocol.h"

namespace cparse
{
    namespace type
    {
        void Date::fromString(const std::string &s)
        {
            struct tm tp;

            if (!strptime(s.c_str(), FORMAT, &tp))
                throw Exception("unable to convert time string");

            value_ = mktime(&tp);
        }

        const char *const Date::FORMAT = "%FT%T%z";

        Date::Date() : value_(time(0))
        {}

        Date::Date(time_t value) : value_(value)
        {}

        Date::Date(const std::string &value)
        {
            fromString(value);
        }

        Date::Date(const JSON &obj)
        {
            fromJSON(obj);
        }

        Date::~Date()
        {}

        Date::Date(const Date &value) : value_(value.value_)
        {}

        Date::Date(Date &&value) : value_(std::move(value.value_))
        {}

        Date &Date::operator=(const Date &a)
        {
            if (this != &a)
            {
                value_ = a.value_;
            }
            return *this;
        }

        Date &Date::operator=(Date && a)
        {
            if (this != &a)
            {
                value_ = std::move(a.value_);
            }
            return *this;
        }


        void Date::fromJSON(const JSON &obj)
        {
            if (obj.contains("iso"))
                fromString(obj.get_string("iso"));
        }

        JSON Date::toJSON() const
        {
            JSON value;

            value.set_string(protocol::KEY_TYPE, protocol::TYPE_DATE);

            value.set_string("iso", toString());

            return value;
        }

        time_t Date::getTimestamp() const
        {
            return value_;
        }

        std::string Date::toString() const
        {
            char buf[BUFSIZ + 1] = {0};

            strftime(buf, BUFSIZ, FORMAT, localtime(&value_));

            return buf;
        }
    }
}