#include <cparse/type/file.h>
#include "client.h"
#include "protocol.h"

namespace cparse
{
    namespace type
    {
        File::File(const JSON &obj)
        {
            fromJSON(obj);
        }

        File::File(const string &fileName, const ContentType &content, const string &contentType) :
            localFileName_(fileName), contentType_(contentType), body_(content)
        {

        }

        File::File()
        {}

        File::~File()
        {}

        File::File(const File &value) : localFileName_(value.localFileName_), parseFileName_(value.parseFileName_),
            contentType_(value.contentType_), body_(value.body_), url_(value.url_)
        {}

        File::File(File &&value) : localFileName_(std::move(value.localFileName_)), parseFileName_(std::move(value.parseFileName_)),
            contentType_(std::move(value.contentType_)), body_(std::move(value.body_)), url_(std::move(value.url_))
        {}

        File &File::operator=(const File &a)
        {
            if (this != &a)
            {
                localFileName_ = a.localFileName_;
                parseFileName_ = a.parseFileName_;
                contentType_ = a.contentType_;
                body_ = a.body_;
                url_ = a.url_;
            }
            return *this;
        }

        File &File::operator=(File && a)
        {
            if (this != &a)
            {
                localFileName_ = std::move(a.localFileName_);
                parseFileName_ = std::move(a.parseFileName_);
                contentType_ = std::move(a.contentType_);
                body_ = std::move(a.body_);
                url_ = std::move(a.url_);
            }
            return *this;
        }

        void File::fromJSON(const JSON &obj)
        {
            if (obj.contains("local_filename"))
                localFileName_ = obj.get_string("local_filename");

            if (obj.contains("name"))
                parseFileName_ = obj.get_string("name");
            else if (obj.contains("parse_filename"))
                parseFileName_ = obj.get_string("parse_filename");

            if (obj.contains("content_type"))
                contentType_ = obj.get_string("content_type");

            if (obj.contains("url"))
                url_ = obj.get_string("url");

            if (obj.contains("body"))
                body_ = obj.get_string("body");
        }

        JSON File::toJSON() const
        {
            JSON value;

            value.set_string(protocol::KEY_TYPE, protocol::TYPE_FILE);

            value.set_string("name", parseFileName_);

            value.set_string("url", url_);

            return value;
        }

        string File::getLocaleFileName() const
        {
            return localFileName_;
        }
        string File::getParseFileName() const
        {
            return parseFileName_;
        }
        string File::getContentType() const
        {
            return contentType_;
        }
        File::ContentType File::getContents() const
        {
            return body_;
        }
        string File::getUrl() const
        {
            return url_;
        }

        void File::setLocaleFileName(const string &fileName)
        {
            localFileName_ = fileName;
        }
        void File::setContentType(const string &contentType)
        {
            contentType_ = contentType;
        }
        void File::setContents(const ContentType &value)
        {
            body_ = value;
        }

        bool File::save()
        {
            Client client;

            client.addHeader("Content-Type", contentType_);

            client.setPayload(body_);

            JSON response;

            try
            {
                client.post("files/" + localFileName_);

                response = client.getJSONResponse();
            }
            catch (const exception &e)
            {
                return false;
            }
            fromJSON(response);
            return true;
        }

    }
}