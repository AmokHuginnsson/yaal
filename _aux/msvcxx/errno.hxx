#ifndef YAAL_MSVCXX_ERRNO_HXX_INCLUDED
#define YAAL_MSVCXX_ERRNO_HXX_INCLUDED 1

#include "hcore/macro.hxx"

namespace msvcxx
{

class Errno
	{
	mutable int _errno;
public:
	Errno( void ) : _errno( 0 ) {}
	operator int& ( void );
	operator int ( void ) const;
	Errno& operator = ( int errno_ );
	};

M_YAAL_HCORE_PUBLIC_API Errno& get_errno( void );

#undef errno
#define errno msvcxx::get_errno()

} /* namespace msvcxx */

#endif /* #ifndef YAAL_MSVCXX_ERRNO_HXX_INCLUDED */
