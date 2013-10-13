#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <stdint.h>
#include <tr1/memory>
#include <string>
#include <vector>
#include <netinet/in.h>

class mdns_socket
{
public:
    struct configuration;

public:
    explicit
    mdns_socket( configuration const& conf );

    void
    send
        ( std::vector< uint8_t > const& buffer );

    void 
    receive 
        ( std::vector< uint8_t > & buffer );

    ~mdns_socket
        ( void );

    operator int
        ( void )
        const;

    std::string const&
    associated_interface
        ( void )
        const;

private:
    mdns_socket
        ( mdns_socket const& );

    mdns_socket &
    operator=
        ( mdns_socket const& );

private:
    int fd_;
    std::string interface_name_;
};

inline
mdns_socket::operator int
    ( void )
    const
{ return fd_; }

inline std::string const&
mdns_socket::associated_interface
    ( void )
    const
{
    return interface_name_;
}

/**
 *
 */
struct mdns_socket::configuration
{
    std::string interface_name;
    ::in_addr bind_address;
};

typedef std::tr1::shared_ptr< mdns_socket > mdns_socket_ptr;

std::ostream &
operator<<
    ( std::ostream & out
    , ::in_addr const& address );

std::ostream &
operator<<
    ( std::ostream & out
    , ::sockaddr_in const& address );

#endif

