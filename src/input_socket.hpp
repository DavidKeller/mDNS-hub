#ifndef INPUT_SOCKET_HPP
#define INPUT_SOCKET_HPP

#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <tr1/memory>

#include "message.hpp"

class input_socket
{
public:
    typedef std::vector< ::in_addr > igmp_subscription_addresses;

public:
    explicit
    input_socket
        ( ::sockaddr const& multicast_address
        , igmp_subscription_addresses const& igmp_subscriptions );

    ~input_socket
        ( void );

    void 
    receive 
        ( message & message
        , ::sockaddr & sender_address );

private:
    input_socket
        ( input_socket const& );

    input_socket &
    operator=
        ( input_socket const& );

private:
    int fd_;
};

typedef std::tr1::shared_ptr< input_socket > input_socket_ptr;

#endif

