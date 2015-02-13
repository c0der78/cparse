#include <cparse/parse.h>
#include <string>
#include "client.h"

using namespace std;

namespace cparse
{

    string cparse_app_id_;
    string cparse_api_key_;
    string cparse_facebook_app_id_;
    bool cparse_offline_messages_;
    bool cparse_error_messages_;
    ClientInterface *cparse_client_interface_ = new CURLClientInterface();

    const char *const Parse::VERSION = "1.0";

    Parse::Parse()
    {

    }

    Parse::Parse(const Parse &other)
    {

    }

    Parse::Parse(Parse &&other)
    {}

    Parse::~Parse()
    {}

    Parse &Parse::operator=(const Parse &other)
    {
        return *this;
    }

    Parse &Parse::operator=(Parse && other)
    {
        return *this;
    }

    void Parse::set_client_interface(ClientInterface *interface)
    {
        if (cparse_client_interface_)
        {
            delete cparse_client_interface_;
        }
        cparse_client_interface_ = interface;
    }

    void Parse::set_application_id(const string &appId)
    {
        cparse_app_id_ = appId;
    }

    void Parse::set_api_key(const string &apiKey)
    {
        cparse_api_key_ = apiKey;
    }

    void Parse::set_facebook_application_id(const string &appId)
    {
        cparse_facebook_app_id_ = appId;
    }

    bool Parse::has_facebook_application_id()
    {
        return !cparse_facebook_app_id_.empty();
    }

    void Parse::offline_messages_enabled(bool value)
    {
        cparse_offline_messages_ = value;
    }

    void Parse::error_messages_enabled(bool value)
    {
        cparse_error_messages_ = value;
    }

}
