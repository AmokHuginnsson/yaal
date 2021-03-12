/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "htokenizer.hxx"
#include "algorithm.hxx"

namespace yaal {

namespace hcore {

HTokenizer::behavior_t const HTokenizer::DEFAULT = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::SKIP_EMPTY = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::INCLUDE_EMPTY = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::HANDLE_EMPTY_MASK = HTokenizer::SKIP_EMPTY | HTokenizer::INCLUDE_EMPTY;
HTokenizer::behavior_t const HTokenizer::DELIMITED_BY_ANY_OF = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::DELIMITED_BY_WHOLE_STRING = HTokenizer::behavior_t::new_flag();
HTokenizer::behavior_t const HTokenizer::DELIMITED_BY_MASK = HTokenizer::DELIMITED_BY_ANY_OF | HTokenizer::DELIMITED_BY_WHOLE_STRING;

HTokenizer::HTokenizer( HString const& str_, HString const& delim_, behavior_t const& behavior_, code_point_t escape_ )
	: _behavior( behavior_ )
	, _string( str_ )
	, _delimiter( delim_ )
	, _escape( escape_ )
	, _buffer() {
	M_PROLOG
	if ( ( _behavior & INCLUDE_EMPTY ) && ( _behavior & SKIP_EMPTY ) ) {
		throw HTokenizerException( "bad empty item handling specified" );
	}
	if ( ! ( _behavior & HANDLE_EMPTY_MASK ) ) {
		_behavior = static_cast<behavior_t>( _behavior | INCLUDE_EMPTY );
	}
	if ( ( _behavior & DELIMITED_BY_ANY_OF ) && ( _behavior & DELIMITED_BY_WHOLE_STRING ) ) {
		throw HTokenizerException( "bad delimited by specified" );
	}
	if ( ! ( _behavior & DELIMITED_BY_MASK ) ) {
		_behavior = static_cast<behavior_t>( _behavior | DELIMITED_BY_WHOLE_STRING );
	}
	return;
	M_EPILOG
}

HTokenizer::HTokenizer( HString const& delim_, behavior_t const& behavior_, code_point_t escape_ )
	: _behavior( behavior_ )
	, _string()
	, _delimiter( delim_ )
	, _escape( escape_ )
	, _buffer() {
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
	if ( start_ == HString::npos ) {
		return start_;
	}
	if ( _behavior & DELIMITED_BY_WHOLE_STRING ) {
		int long delimLen( _delimiter.get_length() );
		HString::const_iterator from( _string.begin() + start_ );
		HString::const_iterator to( from + delimLen );
		HString::const_iterator endIt( _string.end() );
		while ( ( to <= endIt ) && equal( from, to, _delimiter.begin() ) ) {
			start_ += delimLen;
			from += delimLen;
			to += delimLen;
		}
	} else if ( start_ != HString::npos ) {
		start_ = _string.find_other_than( _delimiter, start_ );
	}
	if ( start_ >= _string.get_length() ) {
		start_ = HString::npos;
	}
	return start_;
	M_EPILOG
}

HTokenizer::HIterator::HIterator( HTokenizer const* owner_, int long start_ )
	: _owner( owner_ )
	, _start( start_ )
	, _end( HString::npos )
	, _buffer() {
	M_PROLOG
	find_end();
	return;
	M_EPILOG
}

HTokenizer::HIterator::HIterator( HTokenizer::HIterator const& it_ )
	: _owner( it_._owner )
	, _start( it_._start )
	, _end( it_._end )
	, _buffer( it_._buffer ) {
	return;
}

void HTokenizer::HIterator::find_end( void ) {
	M_PROLOG
	_end = HString::npos;
	if ( _start == HString::npos ) {
		return;
	}
	HString const& s( _owner->_string );
	HString::size_type size( s.get_length() );
	if ( _start >= s.get_length() ) {
		return;
	}
	if ( _owner->_behavior & HTokenizer::DELIMITED_BY_WHOLE_STRING ) {
		_end = s.find( _owner->_delimiter, _start );
	} else if ( !! _owner->_escape ) {
		bool escape( false );
		for ( HString::size_type i( _start ); i < size; ++ i ) {
			if ( escape ) {
				escape = false;
				continue;
			}
			code_point_t c( s[i] );
			if ( c == _owner->_escape ) {
				escape = true;
				continue;
			}
			if ( _owner->_delimiter.find( c ) != HString::npos ) {
				_end = i;
				break;
			}
		}
	} else {
		_end = s.find_one_of( _owner->_delimiter, _start );
	}
	return;
	M_EPILOG
}

HTokenizer::HIterator& HTokenizer::HIterator::operator ++ ( void ) {
	M_PROLOG
	M_ENSURE( _start != HString::npos );
	M_ASSERT( _start >= 0 );
	_start = _end;
	if ( _start == HString::npos ) {
		return ( *this );
	}
	if ( _owner->_behavior & HTokenizer::DELIMITED_BY_WHOLE_STRING ) {
		_start += _owner->_delimiter.get_length();
	} else {
		++ _start;
	}
	if ( _owner->_behavior & HTokenizer::SKIP_EMPTY ) {
		_start = _owner->skip_empty( _start );
	}
	find_end();
	return ( *this );
	M_EPILOG
}

HString const& HTokenizer::HIterator::operator* ( void ) const {
	M_PROLOG
	M_ENSURE( _start != HString::npos );
	M_ASSERT( _start >= 0 );
	_buffer.assign( _owner->_string, _start, _end != HString::npos ? ( _end - _start ) : meta::max_signed<int long>::value );
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

