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

