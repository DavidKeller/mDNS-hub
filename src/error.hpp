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

#ifndef ERROR_HPP
#define ERROR_HPP

#include <sstream>
#include <stdexcept>

/**
 *
 */
class fatal_exception : public virtual std::exception
{
public:
    /**
     *
     */
    fatal_exception
        ( void )
        throw();

    /**
     *
     */
    fatal_exception
        ( fatal_exception const& other )
        throw();

    /**
     *
     */
    fatal_exception &
    operator=
        ( fatal_exception const& other )
        throw();

    /**
     *
     */
    ~fatal_exception
        ( void )
        throw();

    /**
     *
     */
    const char * 
    what
        ( void )
        const
        throw();

    /**
     *
     */
    template<typename Value>
    fatal_exception &
    operator<<
        ( Value const& value );

private:
    /**
     *
     */
    static char *
    strdup
        ( const char * source )
        throw();

private:
    ///
    char * msg_;
};

/**
 *
 */
inline fatal_exception
because
    ( void )
{ return fatal_exception(); }


inline 
fatal_exception::fatal_exception
    ( void )
    throw()
        : msg_( NULL )
{ }

inline char *
fatal_exception::strdup
    ( const char * source )
    throw()
{
    char * result;

    if ( source ) 
    {
        result = static_cast<char *>( std::malloc( std::strlen( source + 1 ) ) );
        if ( result )
            std::strcpy( result, source );
    } 
    else
        result = NULL;

    return result;
}

inline
fatal_exception::fatal_exception
    ( fatal_exception const& other )
    throw()
        : msg_( strdup( other.msg_ ) )
{ }

inline fatal_exception &
fatal_exception::operator=
    ( fatal_exception const& other )
    throw()
{
    if ( msg_ )
        std::free( msg_ );

    msg_ = strdup( other.msg_ );

    return *this;
}

inline 
fatal_exception::~fatal_exception
    ( void )
    throw()
{
    if ( msg_ )
        std::free( msg_ );
}

inline const char *
fatal_exception::what
    ( void )
    const
    throw()
{
    if ( msg_ )
        return msg_;

    return "can't store error message because memory is exhausted";
}

template<typename Value>
inline fatal_exception &
fatal_exception::operator<<
    ( Value const& value )
{
    std::ostringstream buffer( std::ostringstream::ate );

    if ( msg_ )
        buffer.str( msg_ );

    buffer << value;

    msg_ = strdup( buffer.str().c_str() );

    return *this;
}

#endif

