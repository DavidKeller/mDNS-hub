#include "main_loop.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <iostream>
#include <cerrno>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>

#include "error.hpp"
#include "interface.hpp"
#include "subnet.hpp"
#include "input_socket.hpp"
#include "output_socket.hpp"

namespace main_loop  {

namespace {

enum { MDNS_PORT = 5353 };
const char MDNS_INET[] = { "224.0.0.251" };

::sockaddr
mdns_inet_address
    ( void )
{
    ::sockaddr_in result;
    result.sin_family = AF_INET;
    result.sin_port = htons( MDNS_PORT );
    bool const ok = ::inet_pton( AF_INET, MDNS_INET , &result.sin_addr ) == 1;
    assert( ok && "multicast string has been converted" );

    return reinterpret_cast< ::sockaddr const& >( result );
}

const ::sockaddr MDNS_INET_ADDRESS( mdns_inet_address() );

struct destination
{
    subnet subnet_;
    output_socket_ptr socket_;
};

typedef std::vector< destination > destinations;

struct network
{
    input_socket_ptr input_socket_;
    destinations destinations_;
};

network
create_network
    ( interfaces_name const& interfaces_name )
{
    interface_descriptions const descriptions = get_interfaces_description();

    network network;
    input_socket::igmp_subscription_addresses igmp_subscriptions;
    for ( interfaces_name::const_iterator i = interfaces_name.begin()
                , e = interfaces_name.end()
            ; i != e; ++i )
    {
        interface_descriptions::const_iterator j = descriptions.find( *i );
        if ( j == descriptions.end() )
            throw because() << "interface '" << *i << "' is not configured";

        interface_description const& current_interface = j->second;

        ::sockaddr_in socket_address = { };
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons( MDNS_PORT );
        socket_address.sin_addr = current_interface.ip_;

        subnet const subnet( current_interface.ip_, current_interface.mask_ );
        destination const destination =
            { subnet
            , output_socket_ptr( new output_socket( reinterpret_cast< ::sockaddr const& >( socket_address ) ) ) };

        network.destinations_.push_back( destination );
        igmp_subscriptions.push_back( current_interface.ip_ );
    }

    network.input_socket_.reset( new input_socket( MDNS_INET_ADDRESS, igmp_subscriptions ) );

    return network;
}


} // namespace

void
run
    ( configuration const& parsed_configuration )
{
    network network = create_network( parsed_configuration.interfaces_name_ );

    message buffer;

    while ( true )
    {
        union {
            ::sockaddr_in inet_sender;
            ::sockaddr sender;
        };

        network.input_socket_->receive( buffer, sender );

        for ( destinations::iterator i = network.destinations_.begin(), e = network.destinations_.end()
            ; i != e; ++i )
        {
            if ( i->subnet_.is_from_this_subnet( inet_sender.sin_addr ) )
                continue;

            i->socket_->send_to( buffer, MDNS_INET_ADDRESS );
        }
    }
}

} // namespace main_loop

