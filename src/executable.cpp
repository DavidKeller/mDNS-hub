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

#include "executable.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <cerrno>
#include <sstream>
#include <iostream>

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
    while ( ( current_option = ::getopt( argc, argv, "+dfvh" ) ) != -1 )
        switch ( current_option )
        {
            case 'f':
                parsed_configuration.stay_in_foreground_ = true;
                break;
            case 'v':
                parsed_configuration.print_version_ = true;
                break;
            case 'd':
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
        std::cout << "Usage: " << argv[0] << " [-fdvh] interfaces...\n"
                "Where:\n"
                "\t-d\tEnable verbose mode.\n"
                "\t-v\tPrint version.\n"
                "\t-f\tStay in foreground.\n"
                "\t-h\tPrint this help." << std::endl;

    else if ( parsed_configuration.print_version_ )
        std::cout << argv[0] << " " << PACKAGE_VERSION
            << "\n\tBug report: " << PACKAGE_BUGREPORT << std::endl;

    else if ( parsed_configuration.interfaces_name_.size() < 2 )
        throw because() << "at least two interfaces arguments are expected";

    else if ( ! parsed_configuration.stay_in_foreground_ )
        daemonize( parsed_configuration );

    else
        main_loop::run( parsed_configuration );
}

} // namespace executable

