#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include <netinet/in.h>
#include <string>
#include <tr1/memory>

#include "message.hpp"

class udp_socket
{
public:
    typedef std::vector< ::in_addr > igmp_subscription_interfaces;

public:
    explicit
    udp_socket
        ( ::sockaddr const& multicast_address
        , igmp_subscription_interfaces const& igmp_subscriptions );

    explicit
    udp_socket
        ( ::sockaddr const& interface_address );

    ~udp_socket
        ( void );

    void 
    receive_from
        ( ::sockaddr & sender_address
        , message & message );

    void
    send_to
        ( ::sockaddr const& address
        , message const& data );

private:
    udp_socket
        ( udp_socket const& );

    udp_socket &
    operator=
        ( udp_socket const& );

private:
    int fd_;
};

typedef std::tr1::shared_ptr< udp_socket > udp_socket_ptr;

#endif

