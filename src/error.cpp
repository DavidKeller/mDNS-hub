#include "error.hpp"

#include <sstream>
#include <stdexcept>

namespace {

struct error_streambuf : std::stringbuf 
{
    int
    sync
        ( void );
};

int 
error_streambuf::sync
    ( void )
{
    throw std::runtime_error(str());
}

} // namespace

std::ostream & 
throw_because
    ( void )
{
    // XXX Would be better to use per thread local storage.
    static error_streambuf error_streambuf_;
    static std::ostream stream_(&error_streambuf_);

    // Clean stream.
    error_streambuf_.str(std::string());

    return stream_;
}


