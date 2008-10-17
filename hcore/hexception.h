/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hexception.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/* This file holds (except HException class declaration) main #defines,
	 macros and global variables used acros whole yaal */

#ifndef __YAAL_HCORE_HEXCEPTION_H
#define __YAAL_HCORE_HEXCEPTION_H

#include <cerrno>
#include <cstring>
#include <typeinfo>

#ifdef __YAAL_BUILD__
#	include "config.h"
#endif /* __YAAL_BUILD__ */
#include "hcore/hstring.h"

typedef void self_t;
typedef void hier_t;

namespace yaal
{

namespace hcore
{

extern int n_iDebugLevel;

class HException
	{
	mutable bool f_bLocal;
protected:
	static void* ERROR_STREAM;
	char f_cChar;
	int	 f_iInt;
	long f_lLong;
	double f_dDouble;
	char* f_pcCharPtr;
	void* f_pvVoidPtr;
	int  f_iFrame;
	char* f_pcFileName;
	char* f_pcFunctionName;
	int  f_iCode;
	char* f_pcMessage;
public:
	HException( char const* const, char const* const, int const,
			char const* const, int const = 0 );
	HException( char const* const, char const* const, int const,
			HString const&, int const = 0 );
	HException( HException const& );
	virtual ~HException( void ) ;
	void set( char const = 0, int const = 0, long const = 0, double const = 0,
			char const* const = 0, void* const = 0 );
	void set( char const* );
	void set( HString const& );
	void print_error( bool const ) const;
	static void dump_call_stack( int const );
	void log( char const* const, char const* const, int const );
	char const* what( void ) const;
	int code( void ) const;
	static void failed_assert( char const* const, char const* const, int const,
			char const* const ) __attribute__(( __noreturn__ ));
	static void set_error_stream( void* );
	static char* get_type_name( char const* const );
	static void cleanup( char* );
private:
	HException& operator = ( HException const& );
	};

extern char const* const n_pcExceptionType;

template<typename tType, typename hier_t = HException>
class HExceptionT : public hier_t
	{
public:
	HExceptionT( char const* const a_pcReason, char* ptr = hier_t::get_type_name( typeid( tType ).name() ) )
		: hier_t( n_pcExceptionType, ptr, 0, a_pcReason, errno )
		{ hier_t::cleanup( ptr );	}
	HExceptionT( HString const& a_oReason, char* ptr = hier_t::get_type_name( typeid( tType ).name() ) )
		: hier_t( n_pcExceptionType, ptr, 0, a_oReason, errno )
		{ hier_t::cleanup( ptr );	}
	HExceptionT( char const* const a_pcFileName,
			char const* const a_pcFunctionName, int const a_iLine,
			char const* const a_pcReason, int const a_iCode )
		: hier_t( a_pcFileName, a_pcFunctionName, a_iLine, a_pcReason, a_iCode )
		{	}
	HExceptionT( char const* const a_pcFileName,
			char const* const a_pcFunctionName, int const a_iLine,
			HString const& a_oReason, int const a_iCode )
		: hier_t( a_pcFileName, a_pcFunctionName, a_iLine, a_oReason, a_iCode )
		{	}
	};

typedef HExceptionT<HString> HStringException;

class HFailedAssertion
	{
	char const* f_pcWhat;
public:
	HFailedAssertion( char const* const a_pcWhat ) : f_pcWhat( a_pcWhat ) {}
	HFailedAssertion( HFailedAssertion const& fa ) : f_pcWhat( fa.f_pcWhat ) {}
	HFailedAssertion& operator = ( HFailedAssertion const& fa )
		{
		if ( &fa != this )
			{
			HFailedAssertion n( fa );
			HFailedAssertion::swap( n, *this );
			}
		return ( *this );
		}
	char const* what( void ) { return ( f_pcWhat ); }
private:
	static void swap( HFailedAssertion& left, HFailedAssertion& right );
	};

/*
 * A,     AE,      AE = ET<A, E>
 * B : A, BE : AE, BE = ET<B, AE>
 * C : B, CE : BE, CE = ET<C, BE>
 */

namespace exception_auto_hierarchy
{

template<typename tType>
struct parent_exception;

template<typename tType>
struct void_to_exception
	{
	typedef typename parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef HExceptionT<tType, parent_exception_t> exception_t;
	};

template<>
struct void_to_exception<void>
	{
	typedef yaal::hcore::HException exception_t;
	};

template<typename tType, typename base_t>
struct void_to_base
	{
	typedef tType type;
	};

template<typename base_t>
struct void_to_base<void, base_t>
	{
	typedef base_t type;
	};

typedef char YES;
typedef struct { char x[2]; } NO;

template<int const, typename>
struct existing_hier;

template<typename tType>
struct context_hier
	{
	template<typename T>
	static YES has_hier( typename T::hier_t* );
	template<typename T>
	static NO has_hier( ... );
	typedef typename existing_hier<sizeof ( has_hier<tType>( 0 ) ), tType>::type type;
	};

template<typename T>
struct existing_hier<1, T>
	{
	template<typename Q>
	struct get_protected_typedef : public Q
		{
		typedef typename Q::hier_t type;
		};
	typedef typename get_protected_typedef<T>::type type;
	};

template<typename T>
struct existing_hier<2, T>
	{
	typedef hier_t type;
	};

template<typename tType>
struct parent_exception
	{
	typedef typename context_hier<tType>::type proposed_type_hier_t;
	typedef typename void_to_exception<proposed_type_hier_t>::exception_t parent_exception_t;
	};

template<>
struct parent_exception<void>
	{
	typedef yaal::hcore::HException parent_exception_t;
	};

}

template<typename tType, typename message_t, typename code_t>
void throw_exception( char const* const&, char const* const&, int const&, message_t const&, code_t const&, char const* const& = NULL ) __attribute__(( __noreturn__ ));
template<typename tType, typename message_t, typename code_t>
void throw_exception( char const* const& file, char const* const& function, int const& line, message_t const& message, code_t const& code, char const* const& reason )
	{
	typedef typename exception_auto_hierarchy::parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef typename yaal::hcore::HExceptionT<tType, parent_exception_t> exception_t;
	if ( reason )
		{
		exception_t e( file, function, line, message, static_cast<int>( code ) );
	 	e.set( reason );
		throw e;
		}
	else
		throw exception_t( file, function, line, message, static_cast<int>( code ) );
	}

}

}

#endif /* not __YAAL_HCORE_HEXCEPTION_H */

