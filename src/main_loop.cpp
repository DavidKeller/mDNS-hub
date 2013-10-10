#include "main_loop.hpp"

#include <poll.h>
#include <stdint.h>
#include <vector>
#include <iterator>
#include <algorithm>

#include "error.hpp"
#include "interface.hpp"
#include "udp_socket.hpp"

namespace main_loop  {

namespace {

/**
 *
 */
struct interface_data
{
    bool ready_to_send;
    bool data_available;
    udp_socket_ptr udp_socket;
    std::vector< uint8_t > buffer;
};

/**
 *
 */
std::vector< interface_data >
init_interfaces_data
    ( configuration const& parsed_configuration )
{
    std::vector< interface_data > interfaces_data;

    std::vector< udp_socket_ptr > udp_sockets = create_udp_sockets_bound_to_interfaces
            ( parsed_configuration.interfaces_name );

    for ( std::vector< udp_socket_ptr >::iterator i = udp_sockets.begin(), e = udp_sockets.end()
        ; i != e
        ; ++ i ) 
    {
        const interface_data new_data = { false , false , *i };
        interfaces_data.push_back( new_data );
    }

    return interfaces_data;
}

} // namespace

void
run
    ( configuration const& parsed_configuration )
{
    // Init socket.
    std::vector< interface_data > interfaces_data 
            = init_interfaces_data( parsed_configuration );


    throw because() << "main loop is not yet implemented";
}

} // namespace main_loop

