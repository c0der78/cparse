#include <cparse/user.h>
#include "client.h"
#include "protocol.h"

namespace cparse
{
    User *User::currentUser_ = NULL;

    bool User::automaticUser_ = false;

    User *User::currentUser()
    {
        if (currentUser_ == NULL && automaticUser_)
        {
            currentUser_ = new User();
        }
        return currentUser_;
    }

    void User::logout()
    {
        if (currentUser_)
        {
            delete currentUser_;
            currentUser_ = NULL;
        }
    }

    User *User::authenticate(const string &username, const string &password)
    {

        Client client;

        JSON body;

        body.set_string("username", username);

        body.set_string("password", password);

        client.setPayload(body.to_string());

        client.get("login");

        JSON response;

        try
        {
            response = client.getJSONResponse();
        }
        catch (const exception &e)
        {
            return NULL;
        }

        if (currentUser_ != NULL)
            delete currentUser_;

        currentUser_ = new User();

        currentUser_->setUsername(username);

        currentUser_->setPassword(password);

        currentUser_->merge(response);

        return currentUser_;
    }

    User::User() : Object(protocol::CLASS_USER)
    {

    }

    User::~User() {}

    User::User(const User &u) : Object(u), username_(u.username_), password_(u.password_),
        email_(u.email_), sessionToken_(u.sessionToken_), isNew_(u.isNew_)
    {}

    User::User(User &&u) : Object(std::move(u)), username_(std::move(u.username_)), password_(std::move(u.password_)),
        email_(std::move(u.email_)), sessionToken_(std::move(u.sessionToken_)), isNew_(u.isNew_)
    {}

    User &User::operator=(const User &u)
    {
        if (this != &u)
        {
            username_ = u.username_;
            password_ = u.password_;
            email_ = u.email_;
            sessionToken_ = u.sessionToken_;
            isNew_ = u.isNew_;
        }
        return *this;
    }

    User &User::operator=(User && u)
    {
        if (this != &u)
        {
            username_ = std::move(u.username_);
            password_ = std::move(u.password_);
            email_ = std::move(u.email_);
            sessionToken_ = std::move(u.sessionToken_);
            isNew_ = u.isNew_;
        }
        return *this;
    }

    void User::merge(JSON attributes)
    {
        Object::merge(attributes);

        if (attributes.contains(protocol::KEY_USER_SESSION_TOKEN))
        {
            sessionToken_ = attributes.remove(protocol::KEY_USER_SESSION_TOKEN);
        }

        if (attributes.contains(protocol::KEY_USER_EMAIL))
        {
            email_ = attributes.remove(protocol::KEY_USER_EMAIL);
        }
    }

    void User::enableAutomaticUser()
    {
        automaticUser_ = true;
    }

    string User::getUsername() const
    {
        return username_;
    }
    void User::setUsername( const string &value)
    {
        username_ = value;
    }
    string User::getEmail() const
    {
        return email_;
    }
    void User::setEmail(const string &value)
    {
        email_ = value;
    }
    string User::sessionToken() const
    {
        return sessionToken_;
    }
    void User::setPassword(const string &value)
    {
        password_ = value;
    }
    bool User::isNew() const
    {
        return isNew_;
    }
}