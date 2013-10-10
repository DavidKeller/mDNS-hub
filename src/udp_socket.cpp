#include "udp_socket.hpp"

#include <sys/socket.h>
#include <cassert>

#include "error.hpp"

enum { MAX_DATAGRAM_SIZE = 32 * 1024 };

udp_socket::udp_socket
    ( configuration const& conf )
        : fd_( ::socket( conf.bind_address.sa_family
                       , SOCK_DGRAM
                       , 0 ) )
        , address_size_( conf.address_size )
        , mdns_multicast_address_( conf.target_address )
{
    if ( fd_ < 0 )
        throw because() << "can't create udp_socket";

    if ( ::bind( fd_, &conf.bind_address, address_size_ ) < 0 )
        throw because() << "can't bind udp_socket"; 
}

void
udp_socket::send
    ( std::vector< uint8_t > const& buffer )
{
    assert( ! buffer.empty() && "buffer is not empty" );
    if ( ::sendto( fd_
                 , &buffer.front()
                 , buffer.size()
                 , 0
                 , &mdns_multicast_address_
                 , address_size_ ) < 0 )
        throw because() << "can't send data";
}

void
udp_socket::receive
    ( std::vector< uint8_t > & buffer )
{
    buffer.resize( MAX_DATAGRAM_SIZE );
    if ( ::recvfrom( fd_
                   , &buffer.front()
                   , buffer.size()
                   , 0
                   , NULL
                   , 0 ) < 0 )
        throw because() << "can't receive data";
}

udp_socket::~udp_socket
    ( void )
{ ::close( fd_ ); }
