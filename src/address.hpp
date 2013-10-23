#ifndef ADDRESS_HPP
#define ADDRESS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <iosfwd>

std::ostream &
operator<<
    ( std::ostream & out
    , ::in_addr const& address );

std::ostream &
operator<<
    ( std::ostream & out
    , ::sockaddr_in const& address );

::in_addr const&
as_inet_address
    ( ::sockaddr const& address );

typedef union
{
    ::sockaddr_in socket_inet_address_;
    ::sockaddr socket_address_;
} socket_address;

inline ::in_addr
operator &
    ( ::in_addr const& a
    , ::in_addr const& b )
{ 
    ::in_addr const result = { a.s_addr & b.s_addr };

    return result;
}

inline bool
operator ==
    ( ::in_addr const& a
    , ::in_addr const& b )
{ return a.s_addr == b.s_addr; }


#endif // ADDRESS_HPP

