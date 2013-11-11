// Copyright (c) 2013, David Keller
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the University of California, Berkeley nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY DAVID KELLER AND CONTRIBUTORS ``AS IS AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef SUBNET_HPP
#define SUBNET_HPP

#include <netinet/in.h>

#include "address.hpp"

/**
 *  @brief Represents a subnet.
 */
class subnet
{
public:
    /**
     *  @brief Construct a subnet from an address and a mask.
     *  @param address The subnet IPv4 address.
     *  @param mask The subnet corresponding mask.
     */
    explicit
    subnet
        ( ::in_addr const& address 
        , ::in_addr const& mask );
   
    /**
     *  @brief Check if an IPv4 lives within the subnet.
     *  @param address The IPv4 candidate address.
     *  @return true of the address belongs to this subnet.
     */
    bool
    contains
        ( ::in_addr const& address );

private:
    /// The IPv4 subnet address.
    ::in_addr subnet_;
    /// The subnet mask.
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

