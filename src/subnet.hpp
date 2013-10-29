#ifndef SUBNET_HPP
#define SUBNET_HPP

#include <netinet/in.h>

#include "address.hpp"

class subnet
{
public:
    explicit
    subnet
        ( ::in_addr const& address 
        , ::in_addr const& mask );
   
    bool
    contains
        ( ::in_addr const& address );

private:
    ::in_addr subnet_;
    ::in_addr mask_;
};

inline
subnet::subnet
    ( ::in_addr const& address 
    , ::in_addr const& mask )
    : subnet_( address & mask )
    , mask_( mask )
{ }

inline bool
subnet::contains
    ( ::in_addr const& address )
{ return subnet_ == (address & mask_); }


#endif // SUBNET_HPP

