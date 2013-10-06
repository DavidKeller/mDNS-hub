#include <cstdlib>
#include <iostream>

#include "executable.hpp"

int 
main
    ( int argc
    , char * argv[] )
{
    try 
    {
        executable::run(argc, argv);
        return EXIT_SUCCESS;
    }
    catch (std::exception const& failure)
    {
        std::cerr << "fatal failure: " << failure.what() << "." << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) 
    {
        std::cerr << "unknown fatal failure." << std::endl;
        return EXIT_FAILURE;
    }
}

