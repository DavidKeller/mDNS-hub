#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <vector>
#include <string>

#include "mdns_socket.hpp"

/**
 *
 */
std::vector< mdns_socket_ptr >
create_mdns_sockets_bound_to_interfaces
    ( std::vector< std::string > const& interfaces_name );

#endif

