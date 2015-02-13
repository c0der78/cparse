#include <cparse/op/decrement.h>
#include "protocol.h"

namespace cparse
{
    namespace op
    {
        Decrement::Decrement()
        {}

        Decrement::Decrement(int value) : amount_(value)
        {}

        Decrement::~Decrement()
        {}

        Decrement::Decrement(const Decrement &value) : amount_(value.amount_)
        {}

        Decrement::Decrement(Decrement &&value) : amount_(value.amount_)
        {}

        Decrement &Decrement::operator=(const Decrement &a)
        {
            if (this != &a)
            {
                amount_ = a.amount_;
            }
            return *this;
        }

        Decrement &Decrement::operator=(Decrement && a)
        {
            if (this != &a)
            {
                amount_ = a.amount_;
            }
            return *this;
        }

        int Decrement::getAmount() const
        {
            return amount_;
        }

        void Decrement::setAmount(int value)
        {
            amount_ = value;
        }

        JSON Decrement::toJSON() const
        {
            JSON value;

            value.set_string(protocol::KEY_OP, protocol::KEY_INCREMENT);

            value.set_int(protocol::KEY_AMOUNT, amount_);

            return value;
        }
    }
}