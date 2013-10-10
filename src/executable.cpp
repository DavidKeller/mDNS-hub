#include "executable.hpp"

#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <sstream>

#include "configuration.hpp"
#include "main_loop.hpp"
#include "error.hpp"

namespace executable {

namespace {

/**
 *
 */
configuration
parse_configuration
    ( int argc
    , char * argv[] )
{
    configuration parsed_configuration = {};

    // Prevent print to stderr.
    opterr = 0;

    // Parse optional arguments.
    int current_option;
    while ( ( current_option = ::getopt( argc, argv, "+dvh" ) ) != -1 )
        switch ( current_option )
        {
            case 'd':
                parsed_configuration.daemonize = true;
                break;
            case 'v':
                parsed_configuration.verbose = true;
                break;
                break;
            case 'h':
                parsed_configuration.print_help = true;
                break;
            case '?':
                throw because() << "'-" << char( optopt ) << "'"
                        " command line argument is unknown";
            default:
                break;
        }

    // Parse remaining arguments.
    for ( ; optind < argc; ++optind )
        parsed_configuration.interfaces_name.push_back( argv[ optind ] );

    return parsed_configuration;
}

/**
 *
 */
void 
daemonize
    ( configuration const& parsed_configuration )
{
    throw because() << "daemonizing is unimplemented";

    main_loop::run( parsed_configuration );
}

} // namespace

void
run
    ( int argc
    , char * argv[] )
{
    const configuration parsed_configuration( parse_configuration( argc, argv ) );

    if ( parsed_configuration.print_help )
        throw because() << "arguments are incorrect: " << argv[0] 
                << " [-dvh] interfaces...";

    else if ( parsed_configuration.interfaces_name.size() < 2 )
        throw because() << "at least two interfaces are expected";

    else if ( parsed_configuration.daemonize )
        daemonize( parsed_configuration );

    else
        main_loop::run( parsed_configuration );
}

} // namespace executable

