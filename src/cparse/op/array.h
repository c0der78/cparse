#ifndef ARG3_CPARSE_OP_ARRAY_H_
#define ARG3_CPARSE_OP_ARRAY_H_

#include <string>
#include "../json.h"

namespace cparse
{
    namespace op
    {
        class Array
        {
        public:
            Array(const string &operation, const JSONArray &objects);

            Array();

            Array(const Array &value);

            Array(Array &&value);

            virtual ~Array();

            Array &operator=(const Array &value);

            Array &operator=(Array && value);

            JSON toJSON() const;

            void setOperation(const string &value);
            void setObjects(const JSONArray &value);

        private:
            string operation_;
            JSONArray objects_;
        };
    }
}

#endif
