#include "output_socket.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <iostream>

#include "error.hpp"
#include "address.hpp"

output_socket::output_socket
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
                     , &as_inet_address( listening_address )
                     , sizeof( ::in_addr ) ) < 0 )
        throw because() << "can't bind multicast output '" 
                << ::strerror( errno ) << "'";

    // Disable multicast loop.
    ::u_char loop_back_state = 0;
    if ( ::setsockopt( fd_, IPPROTO_IP, IP_MULTICAST_LOOP, &loop_back_state, sizeof( loop_back_state ) ) < 0 )
        throw because() << "can't disable loopback '"
                << ::strerror( errno ) << "'";
}

void
output_socket::send_to
    ( message const& buffer
    , ::sockaddr const& address )
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

output_socket::~output_socket
    ( void )
{ ::close( fd_ ); }

