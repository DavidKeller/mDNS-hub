#include "main_loop.hpp"

#include <poll.h>
#include <stdint.h>
#include <iostream>
#include <cerrno>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "error.hpp"
#include "interface.hpp"
#include "mdns_socket.hpp"

namespace main_loop  {

namespace {

/**
 *
 */
std::map< int, mdns_socket_ptr >
init_sockets_map
    ( configuration const& parsed_configuration )
{
    std::map< int, mdns_socket_ptr > result;

    std::vector< mdns_socket_ptr > mdns_sockets = create_mdns_sockets_bound_to_interfaces
            ( parsed_configuration.interfaces_name );

    for ( std::vector< mdns_socket_ptr >::iterator i = mdns_sockets.begin(), e = mdns_sockets.end()
        ; i != e
        ; ++ i )
    {
        mdns_socket_ptr current_socket = *i;
        result.insert( std::make_pair( static_cast< int >( *current_socket ), current_socket ) );
    }

    return result;
}

typedef std::map< int, mdns_socket_ptr > sockets_map;

::pollfd
socket_to_monitor
    ( sockets_map::value_type const& current_interface_data )
{
    ::pollfd const result = 
            { current_interface_data.first
            , POLLIN };

    return result;
}

typedef std::vector< ::pollfd > monitors;

} // namespace

void
run
    ( configuration const& parsed_configuration )
{
    // Init socket.
    sockets_map sockets = init_sockets_map( parsed_configuration );

    monitors monitors;
    std::transform( sockets.begin() , sockets.end()
                  , std::back_inserter( monitors )
                  , socket_to_monitor );

    std::vector< uint8_t > buffer;
    while ( ::poll( &monitors.front(), monitors.size(), -1 ) > 0 )
    {
        // Iterate over each registered socket.
        for ( monitors::iterator i = monitors.begin(), ei = monitors.end()
            ; i != ei
            ; ++ i )
        {
            // Skip socket that didn't received data.
            if ( ! i->revents & POLLIN )
                continue;

            // Reset POLLIN flag.
            i->revents &= ~POLLIN;

            // The socket is ready, so read from it.
            sockets[i->fd]->receive( buffer );

            // Then forward received message to other sockets.
            for ( monitors::iterator j = monitors.begin(), ej = monitors.end()
                ; j != ej
                ; ++ j )
                // Obviously skip the interface that received the data.
                if ( sockets[j->fd]->associated_interface() 
                        != sockets[i->fd]->associated_interface() )
                    sockets[j->fd]->send( buffer );
        }
    }

    throw because() << "polling failed with " << ::strerror( errno );
}

} // namespace main_loop

