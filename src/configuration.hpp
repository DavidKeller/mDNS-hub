#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <stdint.h>
#include <vector>
#include <string>

typedef std::vector< std::string > interfaces_name;

struct configuration
{
    interfaces_name interfaces_name_;
    bool daemonize_;
    bool verbose_;
    bool print_help_;
};

#endif

