/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/htokenizer.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htokenizer.hxx"

namespace yaal {

namespace hcore {

HTokenizer::behavior_t const HTokenizer::DEFAULT = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::SKIP_EMPTY = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::INCLUDE_EMPTY = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::HANDLE_EMPTY_MASK = HTokenizer::SKIP_EMPTY | HTokenizer::INCLUDE_EMPTY;
HTokenizer::behavior_t const HTokenizer::DELIMITED_BY_ANY_OF = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::DELIMITED_BY_WHOLE_STRING = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::DELIMITED_BY_MASK = HTokenizer::DELIMITED_BY_ANY_OF | HTokenizer::DELIMITED_BY_WHOLE_STRING;

HTokenizer::HTokenizer( HString const& str_, HString const& delim_, behavior_t const& behavior_ )
	: _behavior( behavior_ ), _string( str_ ), _delimiter( delim_ ), _buffer() {
	M_PROLOG
	if ( ( _behavior & INCLUDE_EMPTY ) && ( _behavior & SKIP_EMPTY ) )
		throw HTokenizerException( "bad empty item handling specified" );
	if ( ! ( _behavior & HANDLE_EMPTY_MASK ) )
		_behavior = static_cast<behavior_t>( _behavior | INCLUDE_EMPTY );
	if ( ( _behavior & DELIMITED_BY_ANY_OF ) && ( _behavior & DELIMITED_BY_WHOLE_STRING ) )
		throw HTokenizerException( "bad delimited by specified" );
	if ( ! ( _behavior & DELIMITED_BY_MASK ) )
		_behavior = static_cast<behavior_t>( _behavior | DELIMITED_BY_WHOLE_STRING );
	return;
	M_EPILOG
}

HTokenizer::HTokenizer( HString const& delim_, behavior_t const& behavior_ )
	: _behavior( behavior_ ), _string(), _delimiter( delim_ ), _buffer() {
}

void HTokenizer::assign( HString const& str_ ) {
	M_PROLOG
	_string = str_;
	return;
	M_EPILOG
}

HString HTokenizer::tail( HIterator it_ ) const {
	M_PROLOG
	return ( _string.mid( it_._start ) );
	M_EPILOG
}

HString const& HTokenizer::operator[] ( int long nth_ ) const {
	M_PROLOG
	M_ENSURE( nth_ >= 0 );
	_buffer.clear();
	int long i( nth_ );
	for ( HIterator it( begin() ), endIt( end() ); it != endIt; ++ it, -- i ) {
		if ( ! i ) {
			_buffer = *it;
			break;
		}
	}
	M_ENSURE( ( _behavior & INCLUDE_EMPTY ) || ! _buffer.is_empty() );
	return ( _buffer );
	M_EPILOG
}

HTokenizer::HIterator HTokenizer::begin( void ) const {
	M_PROLOG
	return ( HIterator( this, _behavior & SKIP_EMPTY ? skip_empty( 0 ) : 0 ) );
	M_EPILOG
}

HTokenizer::HIterator HTokenizer::end( void ) const {
	M_PROLOG
	return ( HIterator( this, HString::npos ) );
	M_EPILOG
}

int long HTokenizer::skip_empty( int long start_ ) const {
	M_PROLOG
	M_ASSERT( _behavior & SKIP_EMPTY );
	if ( _behavior & DELIMITED_BY_WHOLE_STRING ) {
		if ( start_ != HString::npos ) {
			int long delimLen( _delimiter.get_length() );
			char const* delimRaw( _delimiter.raw() );
			char const* ptr( _string.raw() );
			while ( ! ::strncmp( delimRaw, ptr + start_, static_cast<size_t>( delimLen ) ) )
				start_ += delimLen;
		}
	} else {
		if ( start_ != HString::npos )
			start_ = _string.find_other_than( _delimiter.raw(), start_ );
	}
	if ( start_ >= _string.get_length() )
		start_ = HString::npos;
	return ( start_ );
	M_EPILOG
}

HTokenizer::HIterator::HIterator( HTokenizer const* owner_, int long start_ )
	: _owner( owner_ ), _start( start_ ), _buffer() {
}

HTokenizer::HIterator::HIterator( HTokenizer::HIterator const& it_ )
	: _owner( it_._owner ), _start( it_._start ), _buffer( it_._buffer ) {
}

HTokenizer::HIterator& HTokenizer::HIterator::operator ++ ( void ) {
	M_PROLOG
	M_ENSURE( _start != HString::npos );
	M_ASSERT( _start >= 0 );
	if ( _owner->_behavior & HTokenizer::DELIMITED_BY_WHOLE_STRING ) {
		_start = _owner->_string.find( _owner->_delimiter, _start );
		if ( _start != HString::npos )
			_start += _owner->_delimiter.get_length();
	} else {
		_start = _owner->_string.find_one_of( _owner->_delimiter.raw(), _start );
		if ( _start != HString::npos )
			++ _start;
	}
	if ( _owner->_behavior & HTokenizer::SKIP_EMPTY )
		_start = _owner->skip_empty( _start );
	return ( *this );
	M_EPILOG
}

HString const& HTokenizer::HIterator::operator* ( void ) const {
	M_PROLOG
	M_ENSURE( _start != HString::npos );
	M_ASSERT( _start >= 0 );
	_buffer.clear();
	int long end( ( _owner->_behavior & HTokenizer::DELIMITED_BY_WHOLE_STRING ) ? _owner->_string.find( _owner->_delimiter, _start ) : _owner->_string.find_one_of( _owner->_delimiter.raw(), _start ) );
	_buffer = _owner->_string.mid( _start, end != HString::npos ? ( end - _start ) : meta::max_signed<int long>::value );
	M_ASSERT( ( _owner->_behavior & HTokenizer::INCLUDE_EMPTY ) || ! _buffer.is_empty()  );
	return ( _buffer );
	M_EPILOG
}

HString const* HTokenizer::HIterator::operator-> ( void ) const {
	M_PROLOG
	return ( &operator*() );
	M_EPILOG
}

bool HTokenizer::HIterator::operator != ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._start != _start );
	M_EPILOG
}

bool HTokenizer::HIterator::operator == ( HIterator const& it ) const {
	M_PROLOG
	M_ASSERT( it._owner == _owner );
	return ( it._start == _start );
	M_EPILOG
}

}

}

