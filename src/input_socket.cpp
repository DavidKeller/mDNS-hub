#include "input_socket.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <iostream>

#include "error.hpp"
#include "address.hpp"

namespace {

enum { MAX_DATAGRAM_SIZE = 32 * 1024 };

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
                      , &igmp_subscription, sizeof( igmp_subscription ) ) < 0 )
        throw because() << "can't subscribe to " << igmp_multicast_address
                << " on '" << interface_address << "' '" << ::strerror( errno ) << "'";
}

} // namespace

input_socket::input_socket
    ( ::sockaddr const& multicast_address
    , igmp_subscription_addresses const& igmp_subscriptions )
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
   
    for ( igmp_subscription_addresses::const_iterator i = igmp_subscriptions.begin()
            , e = igmp_subscriptions.end()
            ; i != e ; ++i )
        setup_igmp_subscription( fd_
                               , reinterpret_cast< ::sockaddr_in const& >( multicast_address ).sin_addr
                               , *i );
}

void
input_socket::receive
    ( message & buffer
    , ::sockaddr & sender_address )
{
    socklen_t address_length = sizeof( sender_address );

    buffer.resize( MAX_DATAGRAM_SIZE );
    ssize_t const count = ::recvfrom( fd_
                                    , &buffer.front()
                                    , buffer.size()
                                    , 0
                                    , &sender_address 
                                    , &address_length );
    if ( count < 0 )
        throw because() << "socket can't receive data '"
                << ::strerror( errno ) << "'";
        
    buffer.resize( count );
}

input_socket::~input_socket
    ( void )
{ ::close( fd_ ); }

