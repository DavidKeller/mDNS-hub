#include "interface.hpp"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <tr1/memory>
#include <cerrno>
#include <cassert>

#include "error.hpp"
#include "address.hpp"

interface_descriptions
get_interfaces_description
    ( void )
{
    ::ifaddrs * current_address;
    if ( ::getifaddrs( &current_address ) < 0 )
        throw because() << ::strerror( errno );
    std::tr1::shared_ptr< ::ifaddrs > cleaner( current_address, ::freeifaddrs ); 

    // Iterate over each interface's addresses
    interface_descriptions descriptions;
    for ( 
        ; current_address
        ; current_address = current_address->ifa_next )
    {
        if ( ! current_address->ifa_addr )
            continue;
      
        short const family = current_address->ifa_addr->sa_family;

        if ( family != AF_INET )
            continue;

        interface_description const description = 
            { as_inet_address( *current_address->ifa_addr )
            , as_inet_address( *current_address->ifa_netmask ) };

        descriptions.insert( std::make_pair( current_address->ifa_name, description ) );
    }

    return descriptions;
}

