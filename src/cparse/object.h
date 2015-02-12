#ifndef ARG3_CPARSE_OBJECT_H
#define ARG3_CPARSE_OBJECT_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include "json.h"
#include "type/pointer.h"
#include <thread>

namespace cparse
{
    class Object;
    class User;
    class Pointer;

    class Object
    {
    public:

        static Object *create(const std::string &className);

        static Object *create(const std::string &className, const JSON &attributes);

        static Object *createWithoutData(const std::string &className, const std::string &objectId);

        static bool saveAll(std::vector<Object> objects);

        static std::thread saveAllInBackground(std::vector<Object> objects, std::function<void()> callback = nullptr);

        Object(const std::string &className);
        virtual ~Object();
        Object(const Object &other);
        Object(Object &&other);
        Object &operator=(const Object &other);
        Object &operator=(Object && other);

        std::string keys() const;

        bool is_valid() const;

        JSON get(const std::string &key) const;
        int32_t getInt(const std::string &key) const;
        int64_t getInt64(const std::string &key) const;
        double getDouble(const std::string &key) const;
        string getString(const std::string &key) const;
        Object *getObject(const std::string &key);
        User *getUser(const std::string &key);

        void set(const std::string &key, const JSON &value);
        void setInt(const std::string &key, int32_t value);
        void setInt64(const std::string &key, int64_t value);
        void setDouble(const std::string &key, double value);
        void setString(const std::string &key, const std::string &value);
        void setArray(const std::string &key, const JSONArray &value);
        void setObject(const std::string &key, const Object &obj);

        void remove(const std::string &key);
        bool contains(const std::string &key) const;

        bool save();
        bool fetch();
        bool de1ete();
        bool refresh();

        std::thread saveInBackground(std::function<void(Object *)> callback = nullptr);
        std::thread fetchInBackground(std::function<void(Object *)> callback = nullptr);
        std::thread destroyInBackground(std::function<void(Object *)> callback = nullptr);

        std::string id() const;

        time_t createdAt() const;

        time_t updatedAt() const;

        std::string className() const;

        bool isDataAvailable() const;

        bool isNew() const;

        type::Pointer toPointer() const;

        bool operator==(const Object &other) const;
        bool operator!=(const Object &other) const;

        bool operator==(const type::Pointer &other) const;
        bool operator!=(const type::Pointer &other) const;

    protected:
        virtual void merge(JSON attributes);
        virtual void copy_fetched(const Object &obj);
    private:
        std::string className_;
        time_t createdAt_;
        std::string objectId_;
        time_t updatedAt_;
        JSON attributes_;
        bool dataAvailable_;
        map<std::string, Object *> fetched_;
    };
}

#endif
