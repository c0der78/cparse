#ifndef ARG3_CPARSE_TYPE_FILE_H_
#define ARG3_CPARSE_TYPE_FILE_H_

#include <string>
#include "../json.h"

using namespace std;

namespace cparse
{
    namespace type
    {
        class File
        {
        public:
            typedef string ContentType;

            File();
            File(const JSON &obj);
            File(const string &fileName, const ContentType &content, const string &contentType);
            File(const File &value);
            File(File &&value);
            virtual ~File();
            File &operator=(const File &value);
            File &operator=(File && value);

            JSON toJSON() const;
            void fromJSON(const JSON &attributes);

            string getLocaleFileName() const;
            string getParseFileName() const;
            string getContentType() const;
            ContentType getContents() const;
            string getUrl() const;

            void setLocaleFileName(const string &fileName);
            void setContentType(const string &contentType);
            void setContents(const ContentType &value);

            bool save();

        private:
            string localFileName_;
            string parseFileName_;
            string contentType_;
            ContentType body_;
            string url_;
        };
    }
}
#endif
