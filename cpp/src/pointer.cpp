#include <cparse/type/pointer.h>
#include <cparse/object.h>
#include "protocol.h"

namespace cparse
{

    namespace type
    {

        Pointer::Pointer(const Object &obj) : className_(obj.className()), objectId_(obj.id())
        {
        }

        Pointer::Pointer()
        {
        }

        Pointer::Pointer(const JSON &obj)
        {
            fromJSON(obj);
        }


        Pointer::~Pointer()
        {}

        Pointer::Pointer(const Pointer &value) : className_(value.className_), objectId_(value.objectId_)
        {}

        Pointer::Pointer(Pointer &&value) : className_(std::move(value.className_)), objectId_(std::move(value.objectId_))
        {}

        Pointer &Pointer::operator=(const Pointer &a)
        {
            if (this != &a)
            {
                className_ = a.className_;
                objectId_ = a.objectId_;
            }
            return *this;
        }

        Pointer &Pointer::operator=(Pointer && a)
        {
            if (this != &a)
            {
                className_ = std::move(a.className_);
                objectId_ = std::move(a.objectId_);
            }
            return *this;
        }


        void Pointer::fromJSON(const JSON &obj)
        {
            if (obj.contains(protocol::KEY_CLASS_NAME))
                className_ = obj.get_string(protocol::KEY_CLASS_NAME);

            if (obj.contains(protocol::KEY_OBJECT_ID))
                objectId_ = obj.get_string(protocol::KEY_OBJECT_ID);
        }

        JSON Pointer::toJSON() const
        {
            JSON value;

            value.set_string(protocol::KEY_TYPE, protocol::TYPE_POINTER);

            value.set_string(protocol::KEY_CLASS_NAME, className_);

            value.set_string(protocol::KEY_OBJECT_ID, objectId_);

            return value;
        }

        string Pointer::id() const
        {
            return objectId_;
        }

        string Pointer::className() const
        {
            return className_;
        }

        bool Pointer::operator==(const Pointer &other) const
        {
            return id() == other.id();
        }

        bool Pointer::operator!=(const Pointer &other) const
        {
            return !Pointer::operator==(other);
        }

        bool Pointer::operator==(const Object &other) const
        {
            return !other.isNew() && id() == other.id();
        }

        bool Pointer::operator!=(const Object &other) const
        {
            return !Pointer::operator==(other);
        }

    }
}