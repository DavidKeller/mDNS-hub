#include "address.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ostream>
#include <cassert>

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

::in_addr const&
as_inet_address
    ( ::sockaddr const& address )
{ return reinterpret_cast< ::sockaddr_in const& >( address ).sin_addr; }

