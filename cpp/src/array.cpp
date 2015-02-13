#include <cparse/op/array.h>

namespace cparse
{
    namespace op
    {
        Array::Array(const string &op, const JSONArray &objs)
            : operation_(op), objects_(objs)
        {}

        Array::Array()
        {}

        Array::~Array()
        {}

        Array::Array(const Array &value) : operation_(value.operation_), objects_(value.objects_)
        {}

        Array::Array(Array &&value) : operation_(std::move(value.operation_)), objects_(std::move(value.objects_))
        {}

        Array &Array::operator=(const Array &a)
        {
            if (this != &a)
            {
                operation_ = a.operation_;
                objects_ = a.objects_;
            }
            return *this;
        }

        Array &Array::operator=(Array && a)
        {
            if (this != &a)
            {
                operation_ = std::move(a.operation_);
                objects_ = std::move(a.objects_);
            }
            return *this;
        }

        void Array::setOperation(const string &value)
        {
            operation_ = value;
        }

        void Array::setObjects(const JSONArray &objs)
        {
            objects_ = objs;
        }
    }
}