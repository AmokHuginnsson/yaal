/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hformat.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hformat.hxx"
#include "hlist.hxx"

namespace yaal
{

namespace hcore
{

class HFormat::HFormatImpl
	{
	struct CONVERSION
		{
		typedef enum
			{
			D_EMPTY,
			D_INT,
			D_FLOAT,
			D_DOUBLE,
			D_STRING,
			D_POINTER,
			D_CONSTANT
			} converion_t;
		};
	struct OToken
		{
		CONVERSION::converion_t _conversion;
		OToken( void ) : _conversion( CONVERSION::D_EMPTY ) {}
		};
	typedef HList<OToken> tokens_t;
	typedef HPointer<tokens_t> tokens_ptr_t;
	int _tokenIndex;
	HString _format;
	HString _string;
	HFormatImpl( char const* const = "" );
	HFormatImpl( HFormatImpl const& );
	HFormatImpl& operator = ( HFormatImpl const& );
	void swap( HFormatImpl& );
	OToken next_token( void );
	friend class HFormat;
	};

HFormat::HFormatImpl::HFormatImpl( char const* const fmt )
	: _tokenIndex( 0 ), _format( fmt ), _string()
	{
	}

HFormat::HFormatImpl::HFormatImpl( HFormatImpl const& fi )
	: _tokenIndex( fi._tokenIndex ), _format( fi._format ), _string( fi._string )
	{
	}

HFormat::HFormatImpl& HFormat::HFormatImpl::operator = ( HFormat::HFormatImpl const& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		HFormatImpl tmp( fi );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HFormat::HFormatImpl::swap( HFormat::HFormatImpl& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		using yaal::swap;
		swap( _tokenIndex, fi._tokenIndex );
		swap( _format, fi._format );
		swap( _string, fi._string );
		}
	return;
	M_EPILOG
	}

HFormat::HFormat( char const* const aFmt )
	: _impl()
	{
	HString fmt( aFmt );
	HFormatImpl::tokens_ptr_t tokens( new HFormatImpl::tokens_t );
	HFormatImpl::OToken t;
	do
		{
		t = HFormatImpl::OToken();
		do
			{
			fmt.find( "%" );
			}
		while ( 0 );
		tokens->push_back( t );
		}
	while ( t._conversion != HFormatImpl::CONVERSION::D_EMPTY );
	}

HFormat::HFormat( format_impl_ptr_t fi )
	: _impl( fi )
	{
	}

HFormat::HFormat( HFormat const& fi )
	: _impl( new HFormat::HFormatImpl( *(fi._impl) ) )
	{
	}

HFormat& HFormat::operator = ( HFormat const& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		HFormat tmp( fi );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HFormat::swap( HFormat& fi )
	{
	M_PROLOG
	if ( &fi != this )
		{
		using yaal::swap;
		swap( _impl, fi._impl );
		}
	return;
	M_EPILOG
	}

HString HFormat::string( void ) const
	{
	return ( _impl->_string );
	}

HString HFormat::format( void ) const
	{
	return ( _impl->_format );
	}

HFormat HFormat::operator % ( int const& i )
	{
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	HFormatImpl::OToken t = _impl->next_token();
	M_ENSURE( t._conversion == HFormatImpl::CONVERSION::D_INT );
	_impl->_string += i;
	return ( _impl );
	M_EPILOG
	}

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_token( void )
	{
	M_PROLOG
	return ( OToken() );
	M_EPILOG
	}

HStreamInterface& operator << ( HStreamInterface& stream, HFormat const& format )
	{
	M_PROLOG
	return ( stream << format.string() );
	M_EPILOG
	}


}

}

