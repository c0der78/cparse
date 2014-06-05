#ifndef ARG3_CPARSE_OP_DECREMENT_H_
#define ARG3_CPARSE_OP_DECREMENT_H_

#include "../json.h"

namespace cparse
{
    namespace op
    {
        class Decrement
        {
        public:
            Decrement();
            Decrement(int value);
            Decrement(const Decrement &value);
            Decrement(Decrement &&value);
            virtual ~Decrement();
            Decrement &operator=(const Decrement &value);
            Decrement &operator=(Decrement && value);

            JSON toJSON() const;

            int getAmount() const;
            void setAmount(int value);

        private:
            int amount_;
        };
    }
}

#endif
