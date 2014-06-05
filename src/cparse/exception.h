#ifndef ARG3_CPARSE_EXCEPTION_H
#define ARG3_CPARSE_EXCEPTION_H

#include <exception>
#include <string>

namespace cparse
{
    class Exception : public std::logic_error
    {
    public:
        Exception(const std::string &what) : std::logic_error(what)
        {}

        Exception(const Exception &e) : std::logic_error(e)
        {}

        Exception(Exception &&e) : std::logic_error(std::move(e))
        {}

        virtual ~Exception() {}

        Exception &operator=(const Exception &e)
        {
            std::logic_error::operator=(e);
            return *this;
        }

        Exception &operator=(Exception && e)
        {
            std::logic_error::operator=(std::move(e));
            return *this;
        }
    };
}

#endif
