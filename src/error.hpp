#ifndef ERROR_HPP
#define ERROR_HPP

#include <sstream>
#include <stdexcept>

/**
 *
 */
class because : public virtual std::exception
{
    char * msg_;

    /**
     *
     */
    static char *
    strdup
        ( const char * source )
        throw();

public:
    /**
     *
     */
    because
        ( void )
        throw();

    /**
     *
     */
    because
        ( because const& other )
        throw();

    /**
     *
     */
    because &
    operator=
        ( because const& other )
        throw();

    /**
     *
     */
    ~because
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
    because &
    operator<<
        ( Value const& value );
};

inline 
because::because
    ( void )
    throw()
        : msg_( NULL )
{ }

inline char *
because::strdup
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
because::because
    ( because const& other )
    throw()
        : msg_( strdup( other.msg_ ) )
{ }

inline because &
because::operator=
    ( because const& other )
    throw()
{
    if ( msg_ )
        std::free( msg_ );

    msg_ = strdup( other.msg_ );

    return *this;
}

inline 
because::~because
    ( void )
    throw()
{
    if ( msg_ )
        std::free( msg_ );
}

inline const char *
because::what
    ( void )
    const
    throw()
{
    if ( msg_ )
        return msg_;

    return "can't store error message because memory is exhausted";
}

template<typename Value>
inline because &
because::operator<<
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

