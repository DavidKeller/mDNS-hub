// Copyright (c) 2013, David Keller
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the University of California, Berkeley nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY DAVID KELLER AND CONTRIBUTORS ``AS IS AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main_loop.hpp"

#include <vector>

#include "error.hpp"
#include "interface.hpp"
#include "subnet.hpp"
#include "udp_socket.hpp"

namespace main_loop  {

namespace {

/// Represents the mDNS multicast endpoint.
const ::sockaddr MDNS_MULTICAST_ADDRESS( to_socket_address( "224.0.0.251", 5353) );

/// Represents a mDNS enabled subnet and its associated socket.
struct output_socket
{
    subnet subnet_;
    udp_socket_ptr socket_;
};

/// Represents all mDNS enabled subnets.
typedef std::vector< output_socket > output_sockets;

/// Represents all mDNS enabled subnets and an input socket to receive messages.
struct network
{
    udp_socket_ptr input_socket_;
    output_sockets output_sockets_;
};

/**
 *
 */
output_socket
create_output_socket
    ( interface_description const& interface )
{
    ::sockaddr_in socket_address = { };
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = as_inet( MDNS_MULTICAST_ADDRESS ).sin_port;
    socket_address.sin_addr = interface.ip_;

    // An output socket is an udp_socket associated
    // with a subnet.
    output_socket const new_output_socket =
        { subnet( interface.ip_, interface.mask_ )
        , udp_socket_ptr( new udp_socket( as_generic( socket_address ) ) ) };

    return new_output_socket; 
}

/**
 *
 */
network
create_network
    ( interfaces_name const& interfaces_name )
{
    interface_descriptions const descriptions = get_interfaces_description();

    network new_network;

    // Keep a track of each interface local address
    // to perform igmp subscriptions on each of them
    // and hence receive mDNS traffic.
    udp_socket::igmp_subscription_interfaces igmp_subscriptions;

    // For each interface required by arguments.
    for ( interfaces_name::const_iterator i = interfaces_name.begin()
            , e = interfaces_name.end() ; i != e; ++i )
    {
        // Ensure it is configured.
        interface_descriptions::const_iterator j = descriptions.find( *i );
        if ( j == descriptions.end() )
            throw because() << "interface '" << *i << "' is not configured";

        interface_description const& current_interface = j->second;

        // Create a socket on this network.
        new_network.output_sockets_.push_back( create_output_socket( current_interface ) );

        // And save socket listening address.
        igmp_subscriptions.push_back( current_interface.ip_ );
    }

    // Finish by created the input socket, subscribing
    // to mDNS multicast on all interfaces.
    new_network.input_socket_.reset( new udp_socket( MDNS_MULTICAST_ADDRESS
                                                   , igmp_subscriptions ) );

    return new_network;
}

/**
 *
 */
void
forward_to_other_subnets
    ( message & current_message
    , ::sockaddr const& sender
    , output_sockets & to )
{
    // For each subnet.
    for ( output_sockets::iterator i = to.begin(), e = to.end()
        ; i != e; ++i )
    {
        // Skip the subnet from where the current message is coming.
        if ( i->subnet_.contains( as_inet( sender ).sin_addr ) )
            continue;

        i->socket_->send_to( MDNS_MULTICAST_ADDRESS, current_message );
    }
}

} // namespace

/**
 *
 */
void
run
    ( configuration const& parsed_configuration )
{
    network whole_network = create_network( parsed_configuration.interfaces_name_ );

    for ( message current_message; ; )
    {
        ::sockaddr sender;

        // Blocking call until a mDNS socket is received by this host
        // an a subnet X.
        whole_network.input_socket_->receive_from( sender, current_message );

        // Forward it to all subnets except X.
        forward_to_other_subnets( current_message
                                , sender
                                , whole_network.output_sockets_ );
    }
}

} // namespace main_loop

