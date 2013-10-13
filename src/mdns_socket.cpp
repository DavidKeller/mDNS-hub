#include "mdns_socket.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <iostream>

#include "error.hpp"

namespace {

enum { MAX_DATAGRAM_SIZE = 32 * 1024 };
enum { MDNS_PORT = 5353 };
const char MDNS_INET[] = { "224.0.0.251" };

::sockaddr_in
mdns_inet_address
    ( void )
{
    ::sockaddr_in result;
    result.sin_family = AF_INET;
    result.sin_port = htons( MDNS_PORT );
    bool const ok = ::inet_pton( AF_INET, MDNS_INET , &result.sin_addr ) == 1;
    assert( ok && "multicast string has been converted" );

    return result;
}


const ::sockaddr_in MDNS_INET_ADDRESS( mdns_inet_address() );

void
setup_igmp_subscription
    ( int fd
    , mdns_socket::configuration const& conf )
{
    // Subscribe to mDNS multicast address on the current interface.
    ::ip_mreq igmp_subscription;
    igmp_subscription.imr_interface = conf.bind_address;
    igmp_subscription.imr_multiaddr = MDNS_INET_ADDRESS.sin_addr;
    if ( ::setsockopt( fd, IPPROTO_IP, IP_ADD_MEMBERSHIP
                      , &igmp_subscription, sizeof( igmp_subscription ) ) < 0 )
        throw because() << "can't subscribe to " << conf.bind_address 
            << " on " << conf.interface_name << " '" 
            << ::strerror( errno ) << "'";

    // Bind multicast traffic send using this socket
    // to the current interface instead of the system default
    // home interface.
    if ( ::setsockopt( fd, IPPROTO_IP, IP_MULTICAST_IF
                     , &conf.bind_address
                     , sizeof( conf.bind_address ) ) < 0 )
        throw because() << "can't bind multicast output on " 
                << conf.interface_name << " '" << ::strerror( errno ) << "'";

    // Prevent multicast traffic to pass through routers.
    int ttl = 1;
    if ( ::setsockopt( fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof( ttl ) ) < 0 )
        throw because() << "can't set multicast ttl to 1 '"
                << ::strerror( errno ) << "'";
}

} // namespace

mdns_socket::mdns_socket
    ( configuration const& conf )
        : fd_( ::socket( PF_INET
                       , SOCK_DGRAM
                       , IPPROTO_UDP ) )
        , interface_name_( conf.interface_name )
{
    if ( fd_ < 0 )
        throw because() << "can't create socket '" 
                << ::strerror( errno ) << "'";

    ::sockaddr_in bind_address = {};
    bind_address.sin_family = AF_INET;
    bind_address.sin_addr = conf.bind_address;
    bind_address.sin_port = htons( MDNS_PORT );

    // Bind the socket to a specific interface.
    if ( ::bind( fd_
               , reinterpret_cast< ::sockaddr const* >( &bind_address )
               , sizeof( bind_address ) ) < 0 )
        throw because() << "can't bind socket '" 
                << ::strerror( errno ) << "'"; 

    std::cout << "Bound to " << bind_address << " on "
            << conf.interface_name << "." << std::endl;

    setup_igmp_subscription( fd_, conf );
}

void
mdns_socket::send
    ( std::vector< uint8_t > const& buffer )
{
    assert( ! buffer.empty() && "buffer is not empty" );
    if ( ::sendto( fd_
                 , &buffer.front()
                 , buffer.size()
                 , 0
                 , reinterpret_cast< ::sockaddr const* >( &MDNS_INET_ADDRESS )
                 , sizeof( MDNS_INET_ADDRESS ) ) < 0 )
        throw because() << "can't send data '" << ::strerror( errno ) << "'";

    std::cout << "Sent " << buffer.size() << " bytes to " 
            << MDNS_INET_ADDRESS << "." << std::endl;
}

void
mdns_socket::receive
    ( std::vector< uint8_t > & buffer )
{
    union {
        ::sockaddr_in inet_sender;
        ::sockaddr sender;
    };
    socklen_t address_length = sizeof( sender );

    buffer.resize( MAX_DATAGRAM_SIZE );
    ssize_t const count = ::recvfrom( fd_
                                    , &buffer.front()
                                    , buffer.size()
                                    , 0
                                    , &sender 
                                    , &address_length );
    if ( count < 0 )
        throw because() << "socket can't receive data '"
                << ::strerror( errno ) << "'";
        
    buffer.resize( count );

    std::cout << "Received " << buffer.size() << " bytes from " 
            << inet_sender << "." << std::endl;
}

mdns_socket::~mdns_socket
    ( void )
{ ::close( fd_ ); }

std::ostream &
operator<<
    ( std::ostream & out
    , ::in_addr const& address )
{
    char buffer[INET_ADDRSTRLEN];
    return out << ::inet_ntop( AF_INET
                             , &address
                             , buffer, sizeof( buffer) );
}

std::ostream &
operator<<
    ( std::ostream & out
    , ::sockaddr_in const& address )
{
    return out << address.sin_addr << ":" << ntohs( address.sin_port );
}

