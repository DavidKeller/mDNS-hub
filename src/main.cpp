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
        std::cerr << "terminated because " << failure.what() << "." << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) 
    {
        std::cerr << "terminated because of an unknown failure." << std::endl;
        return EXIT_FAILURE;
    }
}

