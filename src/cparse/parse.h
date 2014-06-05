#ifndef ARG3_CPARSE_PARSE_H
#define ARG3_CPARSE_PARSE_H

#include <string>

namespace cparse
{
    class ClientInterface;

    class Parse
    {
    public:
        static const char *const VERSION;

        static void set_client_interface(ClientInterface *interface);

        static void set_application_id(const std::string &appId);

        static void set_api_key(const std::string &apiKey);

        static void set_facebook_application_id(const std::string &appId);

        static bool has_facebook_application_id();

        static void offline_messages_enabled(bool value);

        static void error_messages_enabled(bool value);
    private:
        Parse();
        Parse(const Parse &other);
        Parse(Parse &&other);
        ~Parse();
        Parse &operator=(const Parse &other);
        Parse &operator=(Parse && other);
    };

}

#endif
