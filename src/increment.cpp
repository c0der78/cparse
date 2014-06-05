#include <cparse/op/increment.h>
#include "protocol.h"

namespace cparse
{
    namespace op
    {
        Increment::Increment()
        {}

        Increment::Increment(int value) : amount_(value)
        {}

        Increment::~Increment()
        {}

        Increment::Increment(const Increment &value) : amount_(value.amount_)
        {}

        Increment::Increment(Increment &&value) : amount_(value.amount_)
        {}

        Increment &Increment::operator=(const Increment &a)
        {
            if (this != &a)
            {
                amount_ = a.amount_;
            }
            return *this;
        }

        Increment &Increment::operator=(Increment && a)
        {
            if (this != &a)
            {
                amount_ = a.amount_;
            }
            return *this;
        }

        int Increment::getAmount() const
        {
            return amount_;
        }

        void Increment::setAmount(int value)
        {
            amount_ = value;
        }

        JSON Increment::toJSON() const
        {
            JSON value;

            value.set_string(protocol::KEY_OP, protocol::KEY_INCREMENT);

            value.set_int(protocol::KEY_AMOUNT, amount_);

            return value;
        }
    }
}