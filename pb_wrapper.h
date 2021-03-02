#ifndef INCLUDE_PB_WRAPPER_H
#define INCLUDE_PB_WRAPPER_H
#include <pb.h>

#include <exception>
#include <stdio.h>
#include <string>

/*
template<class T>
struct NanoPBSerializer
{
    NanoPBSerializer(uint8_t* buf_, size_t size_)
        : buf{buf_}, size{size_}
    {
    }

    template<class Tfields>
    bool write(T const& a, Tfields fields)
    {
        ostream = pb_ostream_from_buffer(buf, size);
        return pb_encode(&ostream, fields, &a);
    }

    template<class Tfields>
    bool read(T& result, Tfields fields)
    {
        istream = pb_istream_from_buffer(buf, size);
        return pb_decode(&istream, fields, &result);
    }
    pb_byte_t* buf;
    size_t size;
    pb_ostream_t ostream;
    pb_istream_t istream;
};
*/


class SerializeException : public std::exception
{
public:
    SerializeException(const std::string& msg) : m_msg(msg) {}

   virtual const char* what() const throw () 
   {
        return m_msg.c_str();
   }

   const std::string m_msg;
};

template<class T, class Tfields>
std::string NanoPBSerialize(T const& a, Tfields fields)
{
    size_t size;
    pb_get_encoded_size(&size, fields, &a);
    std::string result(size, 0);
    static_assert(sizeof(uint8_t) == sizeof(char));
    pb_ostream_t ostream = pb_ostream_from_buffer(
            reinterpret_cast<pb_byte_t*>(&result[0]),
            size);
    if (!pb_encode(&ostream, fields, &a))
    {
        throw SerializeException(std::string("Encoding failed: ") + PB_GET_ERROR(&ostream) + "\n");
    }
    return result;
}

template<class T, class Tfields>
T NanoPBParse(std::string serialized, Tfields fields)
{
    T result;
    pb_istream_t istream = pb_istream_from_buffer(
            reinterpret_cast<pb_byte_t const*>(serialized.data()),
            serialized.size());
    if (!pb_decode(&istream, fields, &result))
    {
        throw SerializeException(std::string("Decoding failed: ") + PB_GET_ERROR(&istream) + "\n");
    }
    return result;
}

#endif
