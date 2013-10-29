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

::sockaddr
to_socket_address
    ( char const * ip
    , uint16_t port );

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

inline ::sockaddr_in const&
as_inet
    ( ::sockaddr const& address )
{ return reinterpret_cast< ::sockaddr_in const& >( address ); }

inline ::sockaddr const&
as_generic
    ( ::sockaddr_in const& address )
{ return reinterpret_cast< ::sockaddr const& >( address ); }




#endif // ADDRESS_HPP

