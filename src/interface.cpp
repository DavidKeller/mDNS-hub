#include "interface.hpp"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>
#include <tr1/memory>
#include <cerrno>

#include "error.hpp"

namespace {

enum { MDNS_PORT = 5353 };
const char MDNS_INET[] = { "224.0.0.251" };
const char MDNS_INET6[] = { "FF02::FB" };

::sockaddr 
mdns_inet
    ( void )
{
    ::sockaddr_in result;
    result.sin_family = AF_INET;
    result.sin_port = htons( MDNS_PORT );
    ::inet_pton( AF_INET, MDNS_INET , &result.sin_addr );

    return reinterpret_cast< ::sockaddr const& >( result );
}

::sockaddr 
mdns_inet6
    ( void )
{
    ::sockaddr_in6 result;
    result.sin6_family = AF_INET6;
    result.sin6_port = htons( MDNS_PORT );
    ::inet_pton( AF_INET6, MDNS_INET6, &result.sin6_addr );

    return reinterpret_cast< ::sockaddr const& >( result );
}

void
create_inet_only_udp_socket
    ( std::vector< udp_socket_ptr > & udp_sockets
    , ::sockaddr const& current_address )
{
    // Skip non inet interfaces addresses.
    const ::u_char family = current_address.sa_family;
    if ( family == AF_INET )
    {
        const udp_socket::configuration new_configuration =
                { sizeof( ::sockaddr_in6 )
                , current_address
                , mdns_inet() };

        udp_socket_ptr new_udp_socket( new udp_socket( new_configuration ) ); 
        udp_sockets.push_back( new_udp_socket );
    }
    else if ( family == AF_INET6 )
    {
        const udp_socket::configuration new_configuration =
                { sizeof( ::sockaddr_in6 )
                , current_address
                , mdns_inet6() };

        udp_socket_ptr new_udp_socket( new udp_socket( new_configuration ) ); 
        udp_sockets.push_back( new_udp_socket );
    }
}

} // namespace

std::vector< udp_socket_ptr >
create_udp_sockets_bound_to_interfaces
    ( std::vector< std::string > const& interfaces_name )
{
    ::ifaddrs * current_address;
    if ( ::getifaddrs( &current_address ) < 0 )
        throw because() << ::strerror( errno );
    std::tr1::shared_ptr< ::ifaddrs > cleaner( current_address, ::freeifaddrs ); 

    // Iterate over each interface's addresses
    std::vector< udp_socket_ptr > udp_sockets;
    for ( 
        ; current_address
        ; current_address = current_address->ifa_next )
    {
        // Skip non required interface.
        if ( std::find( interfaces_name.begin()
                      , interfaces_name.end()
                      , current_address->ifa_name ) == interfaces_name.end() )
            continue;

        if ( ! current_address->ifa_addr )
            throw because() << current_address->ifa_name << " is not configured";

        create_inet_only_udp_socket( udp_sockets
                                   , *current_address->ifa_addr );
    }

    return udp_sockets;
}

