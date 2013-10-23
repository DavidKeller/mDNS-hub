#ifndef OUTPUT_SOCKET_HPP
#define OUTPUT_SOCKET_HPP

#include <stdint.h>
#include <tr1/memory>
#include <string>
#include <vector>
#include <netinet/in.h>

#include "message.hpp"

class output_socket
{
public:
    explicit
    output_socket( ::sockaddr const& listening_address );

    ~output_socket
        ( void );

    void
    send_to
        ( message const& data
        , ::sockaddr const& address );

private:
    output_socket
        ( output_socket const& );

    output_socket &
    operator=
        ( output_socket const& );

private:
    int fd_;
};

typedef std::tr1::shared_ptr< output_socket > output_socket_ptr;

#endif // OUTPUT_SOCKET_HPP

