#ifndef ARG3_CPARSE_CLIENT_H
#define ARG3_CPARSE_CLIENT_H

#include <cparse/json.h>
#include <cparse/clientinterface.h>
#include <map>

using namespace std;

namespace cparse
{
    class CURLClientInterface : public ClientInterface
    {
    public:
        int request(http::method method, const string &url, map<string, string> headers, const string &data, string &response);
    };

    class Client
    {
    public:
        Client();
        Client(ClientInterface *iterface);
        virtual ~Client();
        Client(const Client &c);
        Client(Client &&c);
        Client &operator=(const Client &c);
        Client &operator=(Client && c);

        JSON getJSONResponse() const;

        void post(const string &path);
        void put(const string &path);
        void get(const string &path);
        void de1ete(const string &path);

        void addHeader(const string &name, const string &value);

        void setPayload(const string &data);
        string getPayload() const;
    protected:
        string buildUrl(const string &path);
    private:
        ClientInterface *interface_;
        map<string, string> headers_;
        string response_;
        string payload_;
        int responseCode_;
    };
}

#endif
