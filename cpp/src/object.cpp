#include <cparse/object.h>
#include <cparse/exception.h>
#include "util.h"
#include <cparse/user.h>
#include "protocol.h"
#include "client.h"

using namespace std;

namespace cparse
{
    time_t datetime(const std::string &s, const std::string &format)
    {
        struct tm tp;

        if (!strptime(s.c_str(), format.c_str(), &tp))
            return 0;

        return mktime(&tp);
    }

    time_t datetime(const string &s)
    {
        return datetime(s, "%FT%T%z");
    }
    bool validate_class_name(const string &value)
    {

        /* check first char is an alpha char */
        auto c = value.begin();

        if (c == value.end() || !isalpha(*c)) return false;

        /* rest of the string has to be alpha numeric */
        while (++c != value.end())
        {
            if (!isalnum(*c))
                return false;
        }
        return true;
    }

    Object *Object::create(const string &className)
    {
        Object *obj = new Object(className);

        return obj;
    }

    Object *Object::create(const string &className, const JSON &attributes)
    {
        Object *obj = new Object(className);

        obj->merge(attributes);

        return obj;
    }

    Object *Object::createWithoutData(const string &className, const string &objectId)
    {
        Object *obj = new Object(className);

        obj->objectId_ = objectId;

        return obj;
    }

    Object::Object(const string &className) : className_(className),
        createdAt_(0),
        objectId_(),
        updatedAt_(0),
        attributes_(),
        dataAvailable_(false)
    {
        if (!validate_class_name(className_))
            throw Exception("invalid class name");
    }

    Object::Object(const Object &other) : className_(other.className_),
        createdAt_(other.createdAt_),
        objectId_(other.objectId_),
        updatedAt_(other.updatedAt_),
        attributes_(other.attributes_),
        dataAvailable_(other.dataAvailable_)
    {
        copy_fetched(other);
    }

    Object::Object(Object &&other) : className_(std::move(other.className_)),
        createdAt_(other.createdAt_),
        objectId_(std::move(other.objectId_)),
        updatedAt_(other.updatedAt_),
        attributes_(std::move(other.attributes_)),
        dataAvailable_(other.dataAvailable_),
        fetched_(std::move(other.fetched_))
    {
    }

    Object::~Object()
    {
        for (auto &e : fetched_)
        {
            if (e.second)
            {
                delete e.second;
            }
        }
    }

    void Object::copy_fetched(const Object &obj)
    {
        for (auto &e : obj.fetched_)
        {
            if (fetched_[e.first] != NULL)
            {
                delete fetched_[e.first];
            }
            fetched_[e.first] = new Object(*e.second);
        }
    }

    Object &Object::operator=(const Object &other)
    {
        if (this != &other)
        {
            className_ = other.className_;
            createdAt_ = other.createdAt_;
            objectId_ = other.objectId_;
            updatedAt_ = other.updatedAt_;
            attributes_ = other.attributes_;
            dataAvailable_ = other.dataAvailable_;

            copy_fetched(other);
        }

        return *this;
    }

    Object &Object::operator=(Object && other)
    {
        if (this != &other)
        {
            className_ = std::move(other.className_);
            createdAt_ = other.createdAt_;
            objectId_ = std::move(other.objectId_);
            updatedAt_ = other.updatedAt_;
            attributes_ = std::move(other.attributes_);
            dataAvailable_ = other.dataAvailable_;
            fetched_ = std::move(other.fetched_);
        }

        return *this;
    }

    bool Object::isNew() const
    {
        return objectId_.empty();
    }

    void Object::merge(JSON attributes)
    {
        attributes.remove(protocol::KEY_CLASS_NAME);

        if (attributes.contains(protocol::KEY_OBJECT_ID))
        {
            objectId_ = attributes.remove(protocol::KEY_OBJECT_ID);
        }

        if (attributes.contains(protocol::KEY_CREATED_AT))
        {
            createdAt_ = datetime(attributes.remove(protocol::KEY_CREATED_AT));
        }

        if (attributes.contains(protocol::KEY_UPDATED_AT))
        {
            updatedAt_ = datetime(attributes.remove(protocol::KEY_UPDATED_AT));
        }

        for (auto &a : attributes)
        {
            set(a.first, a.second);
        }
    }

    bool Object::is_valid() const
    {
        return !objectId_.empty();
    }

    string Object::id() const
    {
        return objectId_;
    }

    time_t Object::createdAt() const
    {
        return createdAt_;
    }

    time_t Object::updatedAt() const
    {
        return updatedAt_;
    }

    JSON Object::get(const string &key) const
    {
        if (!attributes_.contains(key))
            throw Exception(key + " not found.");

        return attributes_.get(key);
    }

    int32_t Object::getInt(const string &key) const
    {
        return get(key).to_int();
    }

    int64_t Object::getInt64(const string &key) const
    {
        return get(key).to_int64();
    }

    double Object::getDouble(const string &key) const
    {
        return get(key).to_double();
    }

    string Object::getString(const string &key) const
    {
        return get(key).to_string();
    }

    Object *Object::getObject(const string &key)
    {
        if (fetched_.find(key) != fetched_.end())
            return fetched_[key];

        JSON val = get(key);

        if (!val.contains(protocol::KEY_TYPE) || val.get_string(protocol::KEY_TYPE) != protocol::TYPE_POINTER)
            return NULL;

        fetched_[key] = Object::create(val.get_string(protocol::KEY_CLASS_NAME), val);

        return fetched_[key];
    }

    User *Object::getUser(const string &key)
    {
        return static_cast<User *>(fetched_[key]);
    }

    void Object::set(const string &key, const JSON &value)
    {
        attributes_.set(key, value);
    }

    void Object::setInt(const string &key, int32_t value)
    {
        attributes_.set_int(key, value);
    }

    void Object::setInt64(const string &key, int64_t value)
    {
        attributes_.set_int64(key, value);
    }

    void Object::setDouble(const string &key, double value)
    {
        attributes_.set_double(key, value);
    }

    void Object::setString(const string &key, const string &value)
    {
        attributes_.set_string(key, value);
    }

    void Object::setArray(const string &key, const JSONArray &value)
    {
        attributes_.set_array(key, value);
    }

    void Object::setObject(const string &key, const Object &obj)
    {
        // create the pointer for this object
        JSON value;
        value.set_string(protocol::KEY_TYPE, protocol::TYPE_POINTER);
        value.set_string(protocol::KEY_OBJECT_ID, obj.objectId_);
        value.set_string(protocol::KEY_CLASS_NAME, obj.className_);
        attributes_.set(key, value);

        // create the fetched object
        if (fetched_[key] != NULL)
        {
            delete fetched_[key];
        }
        fetched_[key] = new Object(obj);
    }
    void Object::remove(const string &key)
    {
        attributes_.remove(key);
    }

    bool Object::contains(const string &key) const
    {
        return attributes_.contains(key);
    }

    string Object::className() const
    {
        return className_;
    }

    bool Object::save()
    {
        JSON response;
        Client client;
        char buf[BUFSIZ + 1] = {0};

        client.setPayload(attributes_.to_string());

        try
        {
            /* build the request based on the id */
            if (objectId_.empty())
            {
                snprintf(buf, BUFSIZ, "%s/%s", protocol::OBJECTS_PATH, className_.c_str());
                client.post(buf);
            }
            else
            {
                snprintf(buf, BUFSIZ, "%s/%s/%s", protocol::OBJECTS_PATH, className_.c_str(), objectId_.c_str());
                client.put(buf);
            }

            response = client.getJSONResponse();
        }
        catch (const exception &e)
        {
            return false;
        }

        /* merge the result with the object */
        merge(response);

        dataAvailable_ = true;

        return true;
    }

    std::thread Object::saveInBackground(std::function<void(Object *)> callback)
    {
        return std::thread([&]()
        {
            if (save() && callback != nullptr)
                callback(this);
        });
    }

    bool Object::refresh()
    {
        if (objectId_.empty())
        {
            return false;
        }

        Client client;
        JSON response;
        char buf[BUFSIZ + 1] = {0};

        try
        {
            snprintf(buf, BUFSIZ, "%s/%s/%s", protocol::OBJECTS_PATH, className_.c_str(), objectId_.c_str());

            client.get(buf);

            response = client.getJSONResponse();
        }
        catch (const exception &e)
        {
            return false;
        }

        merge(response);

        return (dataAvailable_ = true);
    }

    bool Object::fetch()
    {
        if (objectId_.empty())
        {
            return false;
        }

        if (!contains(protocol::KEY_TYPE))
        {
            return false;
        }

        string type = getString(protocol::KEY_TYPE);

        if (type != protocol::TYPE_POINTER)
        {
            return false;
        }

        Client client;
        char buf[BUFSIZ + 1] = {0};
        JSON response;

        try
        {
            snprintf(buf, BUFSIZ, "%s/%s/%s", protocol::OBJECTS_PATH, className_.c_str(), objectId_.c_str());

            client.get(buf);

            response = client.getJSONResponse();
        }
        catch (const exception &e)
        {

            return false;
        }

        merge(response);

        return (dataAvailable_ = true);

    }

    std::thread Object::fetchInBackground(std::function<void(Object *)> callback)
    {
        return std::thread([&]()
        {
            if (fetch() && callback != nullptr)
                callback(this);
        });
    }

    bool Object::saveAll(std::vector<Object> objects)
    {
        bool success = true;

        for (auto &obj : objects)
        {
            success = success && obj.save();
        }

        return success;
    }

    std::thread Object::saveAllInBackground(std::vector<Object> objects, std::function<void()> callback)
    {
        return std::thread([&]()
        {
            if (saveAll(objects) && callback != nullptr)
                callback();
        });
    }

    bool Object::de1ete()
    {
        Client client;
        char buf[BUFSIZ + 1] = {0};

        if (objectId_.empty())
        {
            return false;
        }

        /* do the deed */
        try
        {
            snprintf(buf, BUFSIZ, "%s/%s/%s", protocol::OBJECTS_PATH, className_.c_str(), objectId_.c_str());
            client.de1ete(buf);
        }
        catch (const exception &e)
        {
            return false;
        }

        return true;
    }

    std::thread Object::destroyInBackground(std::function<void(Object *)> callback)
    {
        return std::thread([&]()
        {
            if (de1ete() && callback != nullptr)
                callback(this);
        });
    }

    bool Object::isDataAvailable() const
    {
        return dataAvailable_;
    }

    type::Pointer Object::toPointer() const
    {
        if (isNew())
            throw Exception("object uninitialized");

        return type::Pointer(*this);
    }

    bool Object::operator==(const Object &other) const
    {
        if (isNew())
        {
            return other.isNew() && this == &other;
        }

        return objectId_ == other.objectId_;
    }

    bool Object::operator!=(const Object &other) const
    {
        return !Object::operator==(other);
    }

    bool Object::operator==(const type::Pointer &other) const
    {
        if (isNew()) return false;

        return objectId_ == other.id();
    }

    bool Object::operator!=(const type::Pointer &other) const
    {
        return !Object::operator==(other);
    }

}

