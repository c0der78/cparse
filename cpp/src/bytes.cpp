#include <cparse/type/bytes.h>
#include "protocol.h"

namespace cparse
{

    namespace base64
    {
        static const string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";


        static inline bool is_base64(unsigned char c)
        {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

        string encode(unsigned char const *bytes_to_encode, size_t in_len)
        {
            string ret;
            int i = 0;
            int j = 0;
            unsigned char char_array_3[3];
            unsigned char char_array_4[4];

            while (in_len--)
            {
                char_array_3[i++] = *(bytes_to_encode++);
                if (i == 3)
                {
                    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                    char_array_4[3] = char_array_3[2] & 0x3f;

                    for (i = 0; (i < 4) ; i++)
                        ret += base64_chars[char_array_4[i]];
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 3; j++)
                    char_array_3[j] = '\0';

                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (j = 0; (j < i + 1); j++)
                    ret += base64_chars[char_array_4[j]];

                while ((i++ < 3))
                    ret += '=';

            }

            return ret;

        }

        string encode(const vector<uint8_t> &value)
        {
            return encode(value.data(), value.size());
        }

        vector<uint8_t> decode(const string &encoded_string)
        {
            size_t in_len = encoded_string.size();
            size_t i = 0;
            size_t j = 0;
            int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            vector<uint8_t> ret;

            while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
            {
                char_array_4[i++] = encoded_string[in_];
                in_++;
                if (i == 4)
                {
                    for (i = 0; i < 4; i++)
                        char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));

                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                    for (i = 0; (i < 3); i++)
                        ret.push_back(char_array_3[i]);
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 4; j++)
                    char_array_4[j] = 0;

                for (j = 0; j < 4; j++)
                    char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
            }

            return ret;
        }
    }

    namespace type
    {
        Bytes::Bytes(const JSON &value)
        {
            fromJSON(value);
        }

        Bytes::Bytes(const Type &data) : value_(data)
        {

        }

        Bytes::Bytes()
        {
        }

        Bytes::~Bytes()
        {}

        Bytes::Bytes(const Bytes &value) : value_(value.value_)
        {}

        Bytes::Bytes(Bytes &&value) : value_(std::move(value.value_))
        {}

        Bytes &Bytes::operator=(const Bytes &a)
        {
            if (this != &a)
            {
                value_ = a.value_;
            }
            return *this;
        }

        Bytes &Bytes::operator=(Bytes && a)
        {
            if (this != &a)
            {
                value_ = std::move(a.value_);
            }
            return *this;
        }

        void Bytes::fromJSON(const JSON &attributes)
        {
            if (attributes.contains("base64"))
                value_ = base64::decode(attributes.get_string("base64"));
        }

        JSON Bytes::toJSON() const
        {
            JSON value;

            value.set_string(protocol::KEY_TYPE, protocol::TYPE_BYTES);

            value.set_string("base64", base64::encode(value_));

            return value;
        }

        Bytes::Type Bytes::getData() const
        {
            return value_;
        }

        void Bytes::setData(const Type &value)
        {
            value_ = value;
        }
    }
}