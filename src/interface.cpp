#include "interface.hpp"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <string.h>
#include <tr1/memory>
#include <cerrno>
#include <cassert>
#include <set>

#include "error.hpp"

namespace {

template< typename UdpSocket >
void
create_inet_only_mdns_socket
    ( std::string const& interface_name
    , UdpSocket & mdns_sockets
    , ::in_addr const& bind_address )
{
    const mdns_socket::configuration new_configuration =
            { interface_name
            , bind_address };

    mdns_socket_ptr new_mdns_socket( new mdns_socket( new_configuration ) ); 
    mdns_sockets.push_back( new_mdns_socket );
}

template< typename Expected, typename Found >
void
throw_if_interface_not_found
    ( Expected const& expected
    , Found const& found )
{
    for ( typename Expected::const_iterator i = expected.begin(), e = expected.end()
        ; i != e
        ; ++ i )
        if ( found.find( *i ) == found.end() )
            throw because() << "interface " << *i << " isn't configured";
}

} // namespace

std::vector< mdns_socket_ptr >
create_mdns_sockets_bound_to_interfaces
    ( std::vector< std::string > const& interfaces_name )
{
    ::ifaddrs * current_address;
    if ( ::getifaddrs( &current_address ) < 0 )
        throw because() << ::strerror( errno );
    std::tr1::shared_ptr< ::ifaddrs > cleaner( current_address, ::freeifaddrs ); 


    // Iterate over each interface's addresses
    std::vector< mdns_socket_ptr > mdns_sockets;
    std::set< std::string > found_interfaces_name;
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
            throw because() << "interface " 
                    << current_address->ifa_name << " is not configured";

        short const family = current_address->ifa_addr->sa_family;

        if ( family != AF_INET )
            continue;
        

        found_interfaces_name.insert( current_address->ifa_name );

        ::sockaddr_in * bind_address 
                = reinterpret_cast< ::sockaddr_in * >( current_address->ifa_addr );
        create_inet_only_mdns_socket( current_address->ifa_name
                                   , mdns_sockets
                                   , bind_address->sin_addr );
    }

    throw_if_interface_not_found( interfaces_name, found_interfaces_name );

    return mdns_sockets;
}

