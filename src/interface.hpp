#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <map>
#include <string>
#include <netinet/in.h>

struct interface_description
{
    ::in_addr ip_;
    ::in_addr mask_;
};

typedef std::map< std::string, interface_description > interface_descriptions;

/**
 *
 */
interface_descriptions
get_interfaces_description
    ( void );

#endif

