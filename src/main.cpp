#include <cstdlib>
#include <iostream>

int 
main
    (int argc
    , char * argv)
{
    try 
    {
        return EXIT_SUCCESS;
    }
    catch (std::exception const& failure)
    {
        std::cerr << failure.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...) 
    {
        std::cerr << "unknown fatal failure." << std::endl;
        return EXIT_FAILURE;
    }
}

