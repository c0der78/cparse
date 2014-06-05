#ifndef ARG3_CPARSE_USER_H
#define ARG3_CPARSE_USER_H

#include <string>
#include "object.h"

using namespace std;

namespace cparse
{
    class User : public Object
    {
    public:
        static User *currentUser();
        static void logout();
        static void enableAutomaticUser();
        static User *authenticate(const string &username, const string &password);
        User();
        User(const User &other);
        User(User &&other);
        virtual ~User();
        User &operator=(const User &other);
        User &operator=(User && other);
        string getUsername() const;
        void setUsername( const string &value);
        string getEmail() const;
        void setEmail(const string &value);
        string sessionToken() const;
        void setPassword(const string &value);
        bool isNew() const;
    protected:
        virtual void merge(JSON value);
    private:
        static User *currentUser_;

        string username_;
        string password_;
        string email_;
        string sessionToken_;
        bool isNew_;
        static bool automaticUser_;

    };
}

#endif
