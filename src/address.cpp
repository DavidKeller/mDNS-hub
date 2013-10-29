#include "address.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ostream>

#include "error.hpp"

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
{ return out << address.sin_addr << ":" << ntohs( address.sin_port ); }

::sockaddr
to_socket_address
    ( char const * ip
    , uint16_t port )
{
    ::sockaddr_in result;
    result.sin_family = AF_INET;
    result.sin_port = htons( port );
    if ( ::inet_pton( AF_INET, ip, &result.sin_addr ) != 1 )
        throw because() << "'" << ip << "' can't be interpretred as an IPv4";

    return as_generic( result );
}

