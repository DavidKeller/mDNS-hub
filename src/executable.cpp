#include "executable.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <cerrno>
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
                parsed_configuration.daemonize_ = true;
                break;
            case 'v':
                parsed_configuration.verbose_ = true;
                break;
            case 'h':
                parsed_configuration.print_help_ = true;
                break;
            case '?':
                throw because() << "'-" << char( optopt ) << "'"
                        " command line argument is unknown";
            default:
                break;
        }

    // Parse remaining arguments.
    for ( ; optind < argc; ++optind )
        parsed_configuration.interfaces_name_.push_back( argv[ optind ] );

    return parsed_configuration;
}

/**
 *
 */
void 
daemonize
    ( configuration const& parsed_configuration )
{
    pid_t const pid = ::fork();
    if ( pid < 0)
        throw because() << "can't fork '" << ::strerror( errno ) << "'";

    // If this is parent process, exit.
    if ( pid )
        return;

    ::umask( 0 );

    // Detach from parent session.
    if ( ::setsid() < 0 )
        throw because() << "can't create a new process session '" 
                << ::strerror( errno ) << "'";

    if ( ::chdir( "/" ) < 0 )
        throw because() << "can't chdir to / '" << ::strerror( errno ) << "'";
    
    if ( ::close( STDIN_FILENO ) < 0 )
        throw because() << "can't close stdin '" << ::strerror( errno ) << "'";

    if ( ::open ("/dev/null", O_RDWR) != STDIN_FILENO )
        throw because() << "can't open /dev/null '" << ::strerror( errno ) << "'";
        
    if ( ::dup2( 0, STDOUT_FILENO ) < 0 || ::dup2( 0, STDERR_FILENO ) < 0 )
        throw because() << "can't reassign stdout & stderr'" 
                << ::strerror( errno ) << "'";

    main_loop::run( parsed_configuration );
}

} // namespace

void
run
    ( int argc
    , char * argv[] )
{
    configuration const parsed_configuration( parse_configuration( argc, argv ) );

    if ( parsed_configuration.print_help_ )
        throw because() << "arguments are incorrect: " << argv[0] 
                << " [-dvh] interfaces...";

    else if ( parsed_configuration.interfaces_name_.size() < 2 )
        throw because() << "at least two interfaces arguments are expected";

    else if ( parsed_configuration.daemonize_ )
        daemonize( parsed_configuration );

    else
        main_loop::run( parsed_configuration );
}

} // namespace executable

