#include "client.h"
#include "protocol.h"
#include <cparse/exception.h>
#include <cparse/user.h>
#include <cparse/parse.h>
#include <cparse/clientinterface.h>
#include <curl/curl.h>

namespace cparse
{

    extern string cparse_app_id_;
    extern string cparse_api_key_;
    extern ClientInterface *cparse_client_interface_;

    static size_t curl_append_response_callback(void *ptr, size_t size, size_t nmemb, string *s)
    {
        if (s == NULL) return 0;

        const size_t new_len = size * nmemb;

        char buf[new_len + 1];

        memcpy(buf, ptr, size * nmemb);

        buf[new_len] = '\0';

        s->append(buf);

        return new_len;
    }

    int CURLClientInterface::request(http::method method, const string &url, map<string, string> http_headers, const string &payload, string &response)
    {
        struct curl_slist *headers = NULL;
        char buf[BUFSIZ + 1] = {0};
        int responseCode;

        CURL *curl_ = curl_easy_init();

        if (curl_ == NULL)
        {
            throw Exception("unable to initialize request");
        }

        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, curl_append_response_callback);

        switch (method)
        {
        case http::GET:
            curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
            break;
        case http::POST:
            curl_easy_setopt(curl_, CURLOPT_POST, 1L);
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, payload.c_str());
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, payload.size());
            break;
        case http::PUT:
            curl_easy_setopt(curl_, CURLOPT_PUT, 1L);
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, payload.c_str());
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, payload.size());
        case http::DELETE:
            curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
            break;
        }

        for (auto & h : http_headers)
        {
            snprintf(buf, BUFSIZ, "%s: %s", h.first.c_str(), h.second.c_str());

            headers = curl_slist_append(headers, buf);
        }

        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl_);

        if (res != CURLE_OK)
        {
            curl_easy_cleanup(curl_);

            throw Exception(curl_easy_strerror(res));
        }

        curl_easy_getinfo (curl_, CURLINFO_RESPONSE_CODE, &responseCode);

        curl_easy_cleanup(curl_);

        return responseCode;
    }

    Client::Client() : Client(cparse_client_interface_)
    {}

    Client::Client(ClientInterface *interface) : interface_(interface)
    {
        char version[BUFSIZ + 1] = {0};

        headers_[protocol::HEADER_APP_ID] = cparse_app_id_;

        headers_[protocol::HEADER_API_KEY] = cparse_api_key_;

        headers_["Content-Type"] = "application/json";

        User *user = User::currentUser();

        if (user != NULL)
        {
            headers_[protocol::HEADER_SESSION_TOKEN] = user->sessionToken();
        }

        snprintf(version, BUFSIZ, "libcparse-%s", Parse::VERSION);

        headers_["User-Agent"] = version;
    }

    void Client::addHeader(const string &name, const string &value)
    {
        headers_[name] = value;
    }

    string Client::buildUrl(const string &path)
    {

        char buf[BUFSIZ + 1] = {0};

        snprintf(buf, BUFSIZ, "https://%s/%s/%s", protocol::HOST, protocol::VERSION, path.c_str());

        /*setSecure(true);

        setHost(protocol::HOST);

        setVersion(protocol::VERSION);*/

        return buf;
    }

    Client::~Client()
    {}

    Client::Client(const Client &c) : interface_(c.interface_), headers_(c.headers_)
    {}

    Client::Client(Client &&c) : interface_(c.interface_), headers_(std::move(c.headers_))
    {}

    Client &Client::operator=(const Client &c)
    {
        if (this != &c)
        {
            interface_ = c.interface_;
            headers_ = c.headers_;
        }
        return *this;
    }

    Client &Client::operator=(Client && c)
    {
        if (this != &c)
        {
            interface_ = c.interface_;
            headers_ = std::move(c.headers_);
        }
        return *this;
    }

    void Client::setPayload(const string &data)
    {
        payload_ = data;
    }

    string Client::getPayload() const
    {
        return payload_;
    }

    void Client::post(const string &path)
    {
        responseCode_ = interface_->request(http::POST, buildUrl(path), headers_, payload_, response_);
    }

    void Client::put(const string &path)
    {
        responseCode_ = interface_->request(http::PUT, buildUrl(path), headers_, payload_, response_);
    }

    void Client::get(const string &path)
    {
        responseCode_ = interface_->request(http::GET, buildUrl(path), headers_, payload_, response_);
    }

    void Client::de1ete(const string &path)
    {
        responseCode_ = interface_->request(http::DELETE, buildUrl(path), headers_, payload_, response_);
    }

    JSON Client::getJSONResponse() const
    {
        JSON obj;

        if (!obj.parse(response_))
            throw Exception("reponse is not a valid response");

        if (obj.contains("error"))
        {
            throw Exception(obj.get_string("error"));
        }

        return obj;
    }
}