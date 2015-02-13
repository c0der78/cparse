#ifndef ARG3_CPARSE_TYPE_BASE_H_
#define ARG3_CPARSE_TYPE_BASE_H_

#include "../json.h"

namespace cparse
{
    namespace type
    {
        class ParseType
        {
        public:
            virtual JSON toJSON() const = 0;
            virtual void fromJSON(const JSON &attributes) = 0;
        };
    }
}

#endif
