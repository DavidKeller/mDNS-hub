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

#include "interface.hpp"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <tr1/memory>
#include <cerrno>
#include <cassert>

#include "error.hpp"
#include "address.hpp"

interface_descriptions
get_interfaces_description
    ( void )
{
    ::ifaddrs * current_address;
    if ( ::getifaddrs( &current_address ) < 0 )
        throw because() << ::strerror( errno );
    std::tr1::shared_ptr< ::ifaddrs > cleaner( current_address, ::freeifaddrs ); 

    // Iterate over each interface's addresses
    interface_descriptions descriptions;
    for ( 
        ; current_address
        ; current_address = current_address->ifa_next )
    {
        if ( ! current_address->ifa_addr )
            continue;
      
        short const family = current_address->ifa_addr->sa_family;

        if ( family != AF_INET )
            continue;

        interface_description const new_description = 
            { as_inet( *current_address->ifa_addr ).sin_addr
            , as_inet( *current_address->ifa_netmask ).sin_addr };

        descriptions.insert( std::make_pair( current_address->ifa_name
                                           , new_description ) );
    }

    return descriptions;
}

