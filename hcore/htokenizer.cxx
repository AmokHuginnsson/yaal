/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/htokenizer.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "htokenizer.hxx"

namespace yaal
{

namespace hcore
{

HTokenizer::HTokenizer( HString const& str_, HString const& delim_, behavior_t const& behavior_ )
	: _behavior( behavior_ ), _string( str_ ), _delimiter( delim_ ), _buffer()
	{
	}

HTokenizer::HTokenizer( HString const& delim_, behavior_t const& behavior_ )
	: _behavior( behavior_ ), _string(), _delimiter( delim_ ), _buffer()
	{
	}

void HTokenizer::assign( HString const& str_ )
	{
	M_PROLOG
	_string = str_;
	return;
	M_EPILOG
	}

HString HTokenizer::tail( HIterator it_ ) const
	{
	M_PROLOG
	return ( _string.mid( it_._start ) );
	M_EPILOG
	}

HString const& HTokenizer::operator[] ( int long nth_ ) const
	{
	M_PROLOG
	M_ENSURE( nth_ >= 0 );
	_buffer.clear();
	int long i( nth_ );
	for ( HIterator it( begin() ), endIt( end() ); it != endIt; ++ it, -- i )
		{
		if ( ! i )
			{
			_buffer = *it;
			break;
			}
		}
	M_ENSURE( ( _behavior == HTokenizer::INCLUDE_EMPTY ) || ! _buffer.is_empty() );
	return ( _buffer );
	M_EPILOG
	}

HTokenizer::HIterator HTokenizer::begin( void ) const
	{
	M_PROLOG
	int long idx( _string.find_other_than( _delimiter.raw(), 0 ) );
	return ( HIterator( this, _behavior == INCLUDE_EMPTY ? 0 : ( idx >= 0 ? idx : -1 ) ) );
	M_EPILOG
	}

HTokenizer::HIterator HTokenizer::end( void ) const
	{
	M_PROLOG
	return ( HIterator( this, -1 ) );
	M_EPILOG
	}

HTokenizer::HIterator::HIterator( HTokenizer const* owner_, int long start_ )
	: _owner( owner_ ), _start( start_ ), _buffer()
	{
	}

HTokenizer::HIterator::HIterator( HTokenizer::HIterator const& it_ )
	: _owner( it_._owner ), _start( it_._start ), _buffer( it_._buffer )
	{
	}

HTokenizer::HIterator& HTokenizer::HIterator::operator ++ ( void )
	{
	M_PROLOG
	M_ENSURE( _start >= 0 );
	_start = _owner->_string.find_one_of( _owner->_delimiter.raw(), _start );
	if ( _start >= 0 )
		{
		if ( ( _owner->_behavior == HTokenizer::INCLUDE_EMPTY ) )
			++ _start;
		else
			_start = _owner->_string.find_other_than( _owner->_delimiter.raw(), _start + 1 );
		}
	return ( *this );
	M_EPILOG
	}

HString const& HTokenizer::HIterator::operator* ( void ) const
	{
	M_PROLOG
	M_ENSURE( _start >= 0 );
	_buffer.clear();
	int long end = _owner->_string.find_one_of( _owner->_delimiter.raw(), _start );
	_buffer = _owner->_string.mid( _start, end >= 0 ? ( end - _start ) : meta::max_signed<int long>::value );
	M_ASSERT( ( _owner->_behavior == HTokenizer::INCLUDE_EMPTY ) || ! _buffer.is_empty()  );
	return ( _buffer );
	M_EPILOG
	}

HString const* HTokenizer::HIterator::operator-> ( void ) const
	{
	M_PROLOG
	return ( &operator*() );
	M_EPILOG
	}

bool HTokenizer::HIterator::operator != ( HIterator const& it ) const
	{
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._start != _start );
	M_EPILOG
	}

bool HTokenizer::HIterator::operator == ( HIterator const& it ) const
	{
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._start == _start );
	M_EPILOG
	}

}

}

