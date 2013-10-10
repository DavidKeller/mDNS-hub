#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <vector>
#include <string>

#include "udp_socket.hpp"

/**
 *
 */
std::vector< udp_socket_ptr >
create_udp_sockets_bound_to_interfaces
    ( std::vector< std::string > const& interfaces_name );

#endif

