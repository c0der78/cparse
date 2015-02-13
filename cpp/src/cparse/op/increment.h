#ifndef ARG3_CPARSE_OP_INCREMENT_H_
#define ARG3_CPARSE_OP_INCREMENT_H_

#include "../json.h"

namespace cparse
{
    namespace op
    {
        class Increment
        {
        public:
            Increment();
            Increment(int value);
            Increment(const Increment &value);
            Increment(Increment &&value);
            virtual ~Increment();
            Increment &operator=(const Increment &value);
            Increment &operator=(Increment && value);

            JSON toJSON() const;

            int getAmount() const;
            void setAmount(int value);

        private:
            int amount_;
        };
    }
}

#endif
