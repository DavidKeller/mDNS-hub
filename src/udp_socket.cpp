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

#include "udp_socket.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <iostream>

#include "error.hpp"
#include "address.hpp"

namespace {

enum { MAX_DATAGRAM_SIZE = 32 * 1024 };
enum { RECEIVE_FLAGS };

void
setup_igmp_subscription
    ( int fd
    , ::in_addr const& igmp_multicast_address
    , ::in_addr const& interface_address )
{
    // subscribe to mdns multicast address on the current interface.
    ::ip_mreq igmp_subscription;
    igmp_subscription.imr_interface = interface_address;
    igmp_subscription.imr_multiaddr = igmp_multicast_address;

    if ( ::setsockopt( fd, IPPROTO_IP, IP_ADD_MEMBERSHIP
                     , &igmp_subscription
                     , sizeof( igmp_subscription ) ) < 0 )
        throw because() << "can't subscribe to " << igmp_multicast_address
                << " on '" << interface_address << "' '" 
                << ::strerror( errno ) << "'";
}

} // namespace

udp_socket::udp_socket
    ( ::sockaddr const& listening_address )
        : fd_( ::socket( PF_INET
                       , SOCK_DGRAM
                       , IPPROTO_UDP ) )
{
    if ( fd_ < 0 )
        throw because() << "can't create socket '" 
                << ::strerror( errno ) << "'";

    // Bind the socket to a specific interface.
    if ( ::bind( fd_
               , &listening_address
               , sizeof( listening_address ) ) < 0 )
        throw because() << "can't bind socket '" 
                << ::strerror( errno ) << "'"; 

    // Bind multicast traffic send using this socket
    // to the current interface instead of the system default
    // home interface.
    if ( ::setsockopt( fd_, IPPROTO_IP, IP_MULTICAST_IF
                     , &as_inet( listening_address ).sin_addr
                     , sizeof( ::in_addr ) ) < 0 )
        throw because() << "can't bind multicast output '" 
                << ::strerror( errno ) << "'";

    // Disable multicast loop.
    ::u_char loop_back_state = 0;
    if ( ::setsockopt( fd_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop_back_state, sizeof( loop_back_state ) ) < 0 )
        throw because() << "can't disable loopback '"
                << ::strerror( errno ) << "'";
}

udp_socket::udp_socket
    ( ::sockaddr const& multicast_address
    , igmp_subscription_interfaces const& subscriptions )
        : fd_( ::socket( PF_INET
                       , SOCK_DGRAM
                       , IPPROTO_UDP ) )
{
    if ( fd_ < 0 )
        throw because() << "can't create socket '" 
                << ::strerror( errno ) << "'";

     // Bind the socket to a specific interface.
    if ( ::bind( fd_
               , &multicast_address
               , sizeof( multicast_address ) ) < 0 )
        throw because() << "can't bind socket '" 
                << ::strerror( errno ) << "'"; 
   
    for ( igmp_subscription_interfaces::const_iterator i = subscriptions.begin()
            , e = subscriptions.end()
            ; i != e ; ++i )
        setup_igmp_subscription( fd_
                               , as_inet( multicast_address ).sin_addr
                               , *i );
}

void
udp_socket::receive_from
    ( ::sockaddr & sender_address
    , message & buffer )
{
    socklen_t address_length = sizeof( sender_address );

    buffer.resize( MAX_DATAGRAM_SIZE );
    ssize_t const count = ::recvfrom( fd_
                                    , &buffer.front()
                                    , buffer.size()
                                    , RECEIVE_FLAGS
                                    , &sender_address 
                                    , &address_length );
    if ( count < 0 )
        throw because() << "socket can't receive data '"
                << ::strerror( errno ) << "'";
        
    buffer.resize( count );
}

void
udp_socket::send_to
    ( ::sockaddr const& address
    , message const& buffer )
{
    assert( ! buffer.empty() && "buffer is not empty" );
    if ( ::sendto( fd_
                 , &buffer.front()
                 , buffer.size()
                 , 0
                 , &address
                 , sizeof( address ) ) < 0 )
        throw because() << "can't send data '" << ::strerror( errno ) << "'";
}

udp_socket::~udp_socket
    ( void )
{ ::close( fd_ ); }

