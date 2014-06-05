#ifndef CPARSE_CLIENT_INTERFACE_H_
#define CPARSE_CLIENT_INTERFACE_H_

#include <map>

using namespace std;

namespace cparse
{
    namespace http
    {
        typedef enum
        {
            GET, POST, PUT, DELETE
        } method;
    }

    class ClientInterface
    {
    public:
        virtual ~ClientInterface() {}
        virtual int request(http::method method, const string &url, map<string, string> headers, const string &data, string &response) = 0;
    };
}

#endif
