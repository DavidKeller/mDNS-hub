#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <stdint.h>
#include <tr1/memory>
#include <string>
#include <vector>
#include <netinet/in.h>

class udp_socket
{
public:
    struct configuration;

public:
    explicit
    udp_socket( configuration const& conf );

    void
    send
        ( std::vector< uint8_t > const& buffer );

    void 
    receive 
        ( std::vector< uint8_t > & buffer );

    ~udp_socket
        ( void );

private:
    int fd_;
    ::socklen_t address_size_;
    ::sockaddr mdns_multicast_address_;
};

/**
 *
 */
struct udp_socket::configuration
{
    ::socklen_t address_size;
    ::sockaddr bind_address;
    ::sockaddr target_address;
};

typedef std::tr1::shared_ptr< udp_socket > udp_socket_ptr;

#endif

