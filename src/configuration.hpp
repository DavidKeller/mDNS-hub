#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <stdint.h>
#include <vector>
#include <string>

struct configuration
{
    std::vector< std::string > interfaces_name;
    bool daemonize;
    bool verbose;
    bool print_help;
};

#endif

