/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hformat.cxx - this file is integral part of `yaal' project.

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
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hformat.hxx"
#include "hcore/memory.hxx"
#include "hlist.hxx"
#include "hmap.hxx"
#include "hset.hxx"
#include "hvariant.hxx"
#include "iterator.hxx"
#include "hbitflag.hxx"

namespace yaal {

namespace hcore {

class HFormat::HFormatImpl {
private:
	struct CONVERSION;
	typedef HBitFlag<CONVERSION> conversion_t;
	struct CONVERSION {
		static conversion_t const EMPTY;
		static conversion_t const INT;
		static conversion_t const DOUBLE;
		static conversion_t const STRING;
		static conversion_t const CHAR;
		static conversion_t const POINTER;
		static conversion_t const CONSTANT;
		static conversion_t const BYTE;
		static conversion_t const SHORT;
		static conversion_t const LONG;
		static conversion_t const LONG_LONG;
		static conversion_t const UNSIGNED;
		static conversion_t const OCTAL;
		static conversion_t const HEXADECIMAL;
	};
	struct FLAG;
	typedef HBitFlag<FLAG> flag_t;
	struct FLAG {
		static flag_t const NONE;
		static flag_t const ALTERNATE;
		static flag_t const ZERO_PADDED;
		static flag_t const LEFT_ALIGNED;
		static flag_t const SPACE_PADDED;
		static flag_t const SIGN_PREFIX;
	};
	struct OToken {
		conversion_t _conversion;
		flag_t _flag;
		int _position;
		int _width;
		int _precision;
		HString _const;
		OToken( void )
			: _conversion( CONVERSION::EMPTY )
			, _flag( FLAG::NONE )
			, _position( 0 )
			, _width( 0 )
			, _precision( 0 )
			, _const() {
		}
	};
	typedef HList<OToken> tokens_t;
	typedef HMap<int, OToken*> positions_t;
	typedef HPointer<positions_t> positions_ptr_t;
	typedef HVariant<bool, char, int short, int, int long, int long long, void const*, float, double, double long, HString> format_arg_t;
	typedef HMap<int, format_arg_t> args_t;
	typedef HPointer<args_t> args_ptr_t;
	int _positionIndex;
	HString _format;
	mutable HString _buffer;
	mutable HString _string;
	tokens_t _tokens;
	positions_ptr_t _positions;
	args_ptr_t _args;
	mutable HString _errorMessage;
	HFormatImpl( HString const& );
	HFormatImpl( HFormatImpl const& );
	HFormatImpl& operator = ( HFormatImpl const& );
	void swap( HFormatImpl& );
	int next_token( conversion_t const& );
	static bool has_conversion( HString const&, int );
	OToken next_conversion( HString const&, int& );
	static bool has_constant( HString const&, int );
	OToken next_constant( HString const&, int& );
	static bool has_position( HString const&, int );
	int get_position( HString const&, int& );
	static bool has_flag( HString const&, int );
	static flag_t get_flag( HString const&, int& );
	static bool has_width( HString const&, int );
	int get_width( HString const&, int& );
	static bool has_precision( HString const&, int );
	int get_precision( HString const&, int& );
	conversion_t get_conversion( HString const&, int& );
	template<typename T>
	struct variant_shell {
		static T get( HFormat::HFormatImpl::args_t const& args, int idx ) {
			HFormat::HFormatImpl::args_t::const_iterator it = args.find( idx );
			M_ASSERT( it != args.end() );
			return ( it->second.get<T>() );
		}
	};
public:
	char const* error_message( int = 0 ) const;
private:
	friend class HFormat;
	template<typename tType, typename... arg_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( arg_t&&... );
};

HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::EMPTY = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::INT = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::DOUBLE = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::STRING = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::CHAR = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::POINTER = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::CONSTANT = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::BYTE = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::SHORT = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::LONG = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::LONG_LONG = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::UNSIGNED = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::OCTAL = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::HEXADECIMAL = HFormat::HFormatImpl::conversion_t::new_flag();

HFormat::HFormatImpl::flag_t const HFormat::HFormatImpl::FLAG::NONE = HFormat::HFormatImpl::flag_t::new_flag();
HFormat::HFormatImpl::flag_t const HFormat::HFormatImpl::FLAG::ALTERNATE = HFormat::HFormatImpl::flag_t::new_flag();
HFormat::HFormatImpl::flag_t const HFormat::HFormatImpl::FLAG::ZERO_PADDED = HFormat::HFormatImpl::flag_t::new_flag();
HFormat::HFormatImpl::flag_t const HFormat::HFormatImpl::FLAG::LEFT_ALIGNED = HFormat::HFormatImpl::flag_t::new_flag();
HFormat::HFormatImpl::flag_t const HFormat::HFormatImpl::FLAG::SPACE_PADDED = HFormat::HFormatImpl::flag_t::new_flag();
HFormat::HFormatImpl::flag_t const HFormat::HFormatImpl::FLAG::SIGN_PREFIX = HFormat::HFormatImpl::flag_t::new_flag();

HFormat::HFormatImpl::HFormatImpl( HString const& fmt )
	: _positionIndex( 0 )
	, _format( fmt )
	, _buffer()
	, _string()
	, _tokens()
	, _positions( make_pointer<positions_t>() )
	, _args( make_pointer<args_t>() )
	, _errorMessage() {
	return;
}

HFormat::HFormatImpl::HFormatImpl( HFormatImpl const& fi )
	: _positionIndex( fi._positionIndex )
	, _format( fi._format )
	, _buffer( fi._buffer )
	, _string( fi._string )
	, _tokens( fi._tokens )
	, _positions( make_pointer<positions_t>() )
	, _args( make_pointer<args_t>() )
	, _errorMessage( fi._errorMessage ) {
	M_PROLOG
	*_positions = *fi._positions;
	*_args = *fi._args;
	M_EPILOG
}

HFormat::HFormatImpl& HFormat::HFormatImpl::operator = ( HFormat::HFormatImpl const& fi ) {
	M_PROLOG
	if ( &fi != this ) {
		HFormatImpl tmp( fi );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

void HFormat::HFormatImpl::swap( HFormat::HFormatImpl& fi ) {
	M_PROLOG
	if ( &fi != this ) {
		using yaal::swap;
		swap( _positionIndex, fi._positionIndex );
		swap( _format, fi._format );
		swap( _buffer, fi._buffer );
		swap( _string, fi._string );
		swap( _tokens, fi._tokens );
		swap( _positions, fi._positions );
		swap( _args, fi._args );
		swap( _errorMessage, fi._errorMessage );
	}
	return;
	M_EPILOG
}

namespace {

class common {
	int long _count;
public:
	common( int long count = 0 ) : _count( count ) {}
	common& operator = ( int ) { return ( *this ); }
	common& operator* ( void ) { return ( *this ); }
	common& operator ++ ( void ) { ++ _count; return ( *this ); }
	int long operator() ( void ) const { return ( _count ); }
};

template<typename iter1_t, typename iter2_t>
bool does_intersect( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 ) {
	return ( set_intersection( it1, end1, it2, end2, common() )() > 0 );
}

}

HFormat::HFormat( HString const& aFmt_ )
	: _impl(  make_pointer<HFormatImpl>( aFmt_ ) ) {
	M_PROLOG
	HFormatImpl::OToken t;
	int idx = 0;
	HFormatImpl& impl( *_impl );
	while ( true ) {
		t = HFormatImpl::OToken();
		if ( impl.has_conversion( aFmt_, idx ) ) {
			t = impl.next_conversion( aFmt_, idx );
		} else if ( impl.has_constant( aFmt_, idx ) ) {
			t = impl.next_constant( aFmt_, idx );
		}
		if ( t._conversion != HFormatImpl::CONVERSION::EMPTY ) {
			_impl->_tokens.push_back( t );
		} else {
			break;
		}
	}
	bool anyTokenHaveExplicitIndex = false;
	bool firstToken = true;
	typedef HSet<int> idx_t;
	idx_t idxs;
	idx_t widthIdxs;
	idx_t precIdxs;
	int pos = 0;
	/* positions are 1 based, but we want to index argument 0 based */
	for ( HFormatImpl::tokens_t::iterator it( _impl->_tokens.begin() ), end( _impl->_tokens.end() ); it != end; ++ it, ++ pos ) {
		if ( it->_conversion == HFormatImpl::CONVERSION::CONSTANT ) {
			-- pos;
			continue;
		}
		bool thisTokenHasExplicitIndex( ( it->_position > 0 ) || ( it->_width < -1 ) || ( it->_precision < -1 ) );
		M_ENSURE( ! thisTokenHasExplicitIndex
				|| ( thisTokenHasExplicitIndex && ( it->_position > 0 ) && ( it->_width != -1 ) && ( it->_precision != -1 ) ) );
		M_ENSURE( firstToken || ( anyTokenHaveExplicitIndex && thisTokenHasExplicitIndex ) || ( ! ( anyTokenHaveExplicitIndex || thisTokenHasExplicitIndex ) ) );
		if ( it->_width == -1 ) {
			it->_width = ( - pos ) - 2;
		}
		if ( it->_width < 0 ) {
			widthIdxs.insert( - ( it->_width + 2 ) );
			_impl->_positions->insert( make_pair( - ( it->_width + 2 ), static_cast<HFormatImpl::OToken*>( nullptr ) ) );
			++ pos;
		}
		if ( it->_precision == -1 ) {
			it->_precision = ( - pos ) - 2;
		}
		if ( it->_precision < 0 ) {
			precIdxs.insert( - ( it->_precision + 2 ) );
			_impl->_positions->insert( make_pair( - ( it->_precision + 2 ), static_cast<HFormatImpl::OToken*>( nullptr ) ) );
			++ pos;
		}
		if ( it->_position > 0 ) {
			idxs.insert( -- it->_position );
		} else {
			it->_position = pos;
		}
		_impl->_positions->insert( make_pair( it->_position, &*it ) );
		if ( firstToken ) {
			anyTokenHaveExplicitIndex = thisTokenHasExplicitIndex;
		}
		firstToken = false;
	}
	M_ENSURE( ! does_intersect( widthIdxs.begin(), widthIdxs.end(), precIdxs.begin(), precIdxs.end() ) );
	M_ENSURE( ! does_intersect( widthIdxs.begin(), widthIdxs.end(), idxs.begin(), idxs.end() ) );
	M_ENSURE( ! does_intersect( idxs.begin(), idxs.end(), precIdxs.begin(), precIdxs.end() ) );
	copy( widthIdxs.begin(), widthIdxs.end(), insert_iterator( idxs ) );
	copy( precIdxs.begin(), precIdxs.end(), insert_iterator( idxs ) );
	int last( -1 );
	for ( idx_t::iterator it( idxs.begin() ), end( idxs.end() ); it != end; ++ it, ++ last ) {
		M_ENSURE( *it == ( last + 1 ) );
	}
	return;
	M_EPILOG
}

HFormat::HFormat( format_impl_ptr_t fi )
	: _impl( fi ) {
}

HFormat::HFormat( HFormat const& fi )
	: _impl( new ( memory::yaal ) HFormat::HFormatImpl( *(fi._impl) ) ) {
}

HFormat& HFormat::operator = ( HFormat const& fi ) {
	M_PROLOG
	if ( &fi != this ) {
		HFormat tmp( fi );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

void HFormat::swap( HFormat& fi ) {
	M_PROLOG
	if ( &fi != this ) {
		using yaal::swap;
		swap( _impl, fi._impl );
	}
	return;
	M_EPILOG
}

HString HFormat::string( void ) const {
	M_PROLOG
	HString fmt;
	M_ENSURE( _impl->_positions->size() == _impl->_args->size() );
	_impl->_string.clear();
	for ( HFormatImpl::tokens_t::const_iterator it = _impl->_tokens.begin(), end = _impl->_tokens.end(); it != end; ++ it ) {
		HFormatImpl::conversion_t conv = it->_conversion;
		if ( conv == HFormatImpl::CONVERSION::CONSTANT ) {
			_impl->_string += it->_const;
		} else {
			fmt = "%";
			if ( !!( it->_flag & HFormatImpl::FLAG::ALTERNATE ) ) {
				fmt += "#";
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::ZERO_PADDED ) ) {
				fmt += "0";
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::SPACE_PADDED ) ) {
				fmt += " ";
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::SIGN_PREFIX ) ) {
				fmt += "+";
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::LEFT_ALIGNED ) ) {
				fmt += "-";
			}
			if ( it->_width > 0 ) {
				fmt += it->_width;
			} else if ( it->_width < 0 ) {
				fmt += HFormatImpl::variant_shell<int>::get( *_impl->_args, - ( it->_width + 2 ) );
			}
			if ( it->_precision ) {
				fmt += ".";
			}
			if ( it->_precision > 0 ) {
				fmt += it->_precision;
			} else if ( it->_precision < 0 ) {
				fmt += HFormatImpl::variant_shell<int>::get( *_impl->_args, - ( it->_precision + 2 ) );
			}
			if ( !!( conv & HFormatImpl::CONVERSION::BYTE ) ) {
				fmt += "hh";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::SHORT ) ) {
				fmt += "h";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::LONG ) ) {
				if ( !!( conv & HFormatImpl::CONVERSION::INT ) ) {
					fmt += "l";
				} else {
					M_ASSERT( !!( conv & HFormatImpl::CONVERSION::DOUBLE ) );
					fmt += "L";
				}
			}
			if ( !!( conv & HFormatImpl::CONVERSION::LONG_LONG ) ) {
				fmt += "ll";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::OCTAL ) ) {
				fmt += "o";
			} else if ( !!( conv & HFormatImpl::CONVERSION::HEXADECIMAL ) ) {
				fmt += "x";
			} else if ( !!( conv & HFormatImpl::CONVERSION::UNSIGNED ) ) {
				fmt += "u";
			} else if ( !!( conv & HFormatImpl::CONVERSION::INT ) ) {
				fmt += "d";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::DOUBLE ) ) {
				fmt += "f";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::STRING ) ) {
				fmt += "s";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::CHAR ) ) {
				fmt += "c";
			}
			if ( !!( conv & HFormatImpl::CONVERSION::INT ) ) {
				if ( !!( conv & HFormatImpl::CONVERSION::BYTE ) ) {
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<char>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & HFormatImpl::CONVERSION::SHORT ) ) {
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<int short>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & HFormatImpl::CONVERSION::LONG ) ) {
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<int long>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & HFormatImpl::CONVERSION::LONG_LONG ) ) {
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<int long long>::get( *_impl->_args, it->_position ) );
				} else {
					M_ASSERT( conv == HFormatImpl::CONVERSION::INT );
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<int>::get( *_impl->_args, it->_position ) );
				}
			} else if ( !!( conv & HFormatImpl::CONVERSION::STRING ) ) {
				_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<HString>::get( *_impl->_args, it->_position ).raw() );
			} else if ( !!( conv & HFormatImpl::CONVERSION::POINTER ) ) {
				_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<void const*>::get( *_impl->_args, it->_position ) );
			} else if ( !!( conv & HFormatImpl::CONVERSION::CHAR ) ) {
				_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<char>::get( *_impl->_args, it->_position ) );
			} else if ( !!( conv & HFormatImpl::CONVERSION::DOUBLE ) ) {
				if ( !!( conv & ( HFormatImpl::CONVERSION::LONG ) ) ) {
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<double long>::get( *_impl->_args, it->_position ) );
				} else {
					_impl->_buffer.format( fmt.raw(), HFormatImpl::variant_shell<double>::get( *_impl->_args, it->_position ) );
				}
			}
			_impl->_string += _impl->_buffer;
		}
	}
	return ( _impl->_string );
	M_EPILOG
}

HString HFormat::format( void ) const {
	return ( _impl->_format );
}

HFormat HFormat::operator % ( char c ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::CHAR );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( c ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( char unsigned cu ) {
	M_PROLOG
	return ( operator % ( static_cast<char>( cu ) ) );
	M_EPILOG
}

HFormat HFormat::operator % ( int short is ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::SHORT );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( is ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int short unsigned isu ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::SHORT | HFormatImpl::CONVERSION::UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( static_cast<int short>( isu ) ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int i ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( i ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int unsigned iu ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( static_cast<int>( iu ) ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int long il ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::LONG );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( il ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int long unsigned ilu ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::LONG | HFormatImpl::CONVERSION::UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( static_cast<int long>( ilu ) ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int long long ill ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::LONG_LONG );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( ill ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( int long long unsigned illu ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::INT | HFormatImpl::CONVERSION::LONG_LONG | HFormatImpl::CONVERSION::UNSIGNED );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( static_cast<int long long>( illu ) ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( double d ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::DOUBLE );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( d ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( double long dl ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::DOUBLE | HFormatImpl::CONVERSION::LONG );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( dl ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( float f ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::DOUBLE );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( f ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( char const* s ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::STRING );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( HString( s ) ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( void const* p ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::POINTER );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( p ) ) );
	return ( _impl );
	M_EPILOG
}

HFormat HFormat::operator % ( HString const& s ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::STRING );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( s ) ) );
	return ( _impl );
	M_EPILOG
}

int HFormat::HFormatImpl::next_token( HFormatImpl::conversion_t const& conv ) {
	M_PROLOG
	if ( _positions->get_size() == _args->get_size() ) {
		_positionIndex = 0;
		_string.clear();
		_args->clear();
	}
	positions_t::const_iterator it = _positions->find( _positionIndex );
	M_ENSURE( it != _positions->end() );
	OToken* t = it->second;
	M_ENSURE(
		( t && (
			( conv == t->_conversion )
			|| ( ( conv == HFormatImpl::CONVERSION::CHAR ) && ( t->_conversion & HFormatImpl::CONVERSION::INT ) && ( t->_conversion & HFormatImpl::CONVERSION::BYTE ) )
		) )
		|| ( ! t && ( conv == HFormatImpl::CONVERSION::INT ) )
	);
	int idx = _positionIndex;
	++ _positionIndex;
	return ( idx );
	M_EPILOG
}

bool HFormat::HFormatImpl::has_conversion( HString const& s, int i ) {
	M_PROLOG
	return ( ( i < s.get_length() ) && ( s[ i ] == '%' ) );
	M_EPILOG
}

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_conversion( HString const& s, int& i ) {
	M_PROLOG
	OToken t;
	++ i;
	if ( has_position( s, i ) ) {
		t._position = get_position( s, i );
	}
	if ( has_flag( s, i ) ) {
		t._flag = get_flag( s, i );
	}
	if ( has_width( s, i ) ) {
		t._width = get_width( s, i );
	}
	if ( has_precision( s, i ) ) {
		t._precision = get_precision( s, i );
	}
	t._conversion = get_conversion( s, i );
	return ( t );
	M_EPILOG
}

bool HFormat::HFormatImpl::has_constant( HString const& s, int i ) {
	M_PROLOG
	return ( ( i < s.get_length() ) && ( s[ i ] != '%' ) );
	M_EPILOG
}

HFormat::HFormatImpl::OToken HFormat::HFormatImpl::next_constant( HString const& s, int& i ) {
	M_PROLOG
	OToken t;
	int token = static_cast<int>( s.find( '%', i ) );
	t._const = s.mid( i, token > 0 ? token - i : meta::max_signed<int>::value );
	i += static_cast<int>( t._const.get_length() );
	t._conversion = CONVERSION::CONSTANT;
	return ( t );
	M_EPILOG
}

bool HFormat::HFormatImpl::has_flag( HString const& s, int i ) {
	M_PROLOG
	HString flags = "#0- +";
	return ( ( i < s.get_length() ) && ( flags.find( s[ i ] ) >= 0 ) );
	M_EPILOG
}

bool HFormat::HFormatImpl::has_precision( HString const& s, int i ) {
	M_PROLOG
	return ( ( i < s.get_length() ) && ( s[ i ] == '.' ) );
	M_EPILOG
}

bool HFormat::HFormatImpl::has_position( HString const& s, int i ) {
	M_PROLOG
	bool hasPosition = false;
	int long len = s.get_length();
	if ( ( i + 1 ) < len ) /* + 1 is for '$' character */ {
		int k = i;
		while ( ( s[ k ] >= '0' ) && ( s[ k ] <= '9' ) && ( k < len ) ) {
			++ k;
		}
		if ( ( k < len ) && ( s[ k ] == '$' ) ) {
			hasPosition = true;
		}
	}
	return ( hasPosition );
	M_EPILOG
}

bool HFormat::HFormatImpl::has_width( HString const& s, int i ) {
	M_PROLOG
	return ( ( i < s.get_length() ) && ( ( s[ i ] == '*' ) || ( ( s[ i ] >= '0' ) && ( s[ i ] <= '9' ) ) ) );
	M_EPILOG
}

int HFormat::HFormatImpl::get_position( HString const& s, int& i ) {
	M_PROLOG
	M_ENSURE( ( i + 1 ) < s.get_length() );
	int position = lexical_cast<int>( s.mid( i ) );
	while ( ( s[ i ] >= '0' ) && ( s[ i ] <= '9' ) ) {
		++ i;
	}
	++ i; /* for '$' character. */
	return ( position );
	M_EPILOG
}

int HFormat::HFormatImpl::get_width( HString const& s, int& i ) {
	M_PROLOG
	int width = 0;
	if ( s[ i ] == '*' ) {
		++ i; /* skip '*' character. */
		if ( has_position( s, i ) ) {
			width = - ( get_position( s, i ) + 1 );
		} else {
			width = -1;
		}
	} else {
		width = get_position( s, i );
		-- i; /* width is in place, no '$' character at the end of width. */
	}
	return ( width );
	M_EPILOG
}

int HFormat::HFormatImpl::get_precision( HString const& s, int& i ) {
	M_PROLOG
	++ i; /* skip '.' character */
	return ( get_width( s, i ) );
	M_EPILOG
}

HFormat::HFormatImpl::conversion_t HFormat::HFormatImpl::get_conversion( HString const& s, int& i ) {
	M_PROLOG
	conversion_t conversion = CONVERSION::EMPTY;
	M_ENSURE( i < s.get_length() );
	struct OLength {
		char const* _label;
		conversion_t _conversion;
	} length[] = {
		{ "hh", CONVERSION::BYTE },
		{ "h", CONVERSION::SHORT },
		{ "ll", CONVERSION::LONG_LONG },
		{ "l", CONVERSION::LONG },
		{ "q", CONVERSION::LONG_LONG },
		{ "L", CONVERSION::LONG_LONG }
	};
	int lenMod = -1;
	for ( size_t k = 0; k < ( sizeof ( length ) / sizeof ( OLength ) ); ++ k ) {
		if ( s.find( length[ k ]._label, i ) == i ) {
			i += static_cast<int>( ::std::strlen( length[ k ]._label ) );
			lenMod = static_cast<int>( k );
			break;
		}
	}
	M_ENSURE( i < s.get_length() );
	switch ( s[ i ] ) {
		case ( 'd' ):
		case ( 'i' ): {
			conversion = CONVERSION::INT;
		} break;
		case ( 'u' ): {
			conversion = CONVERSION::INT | CONVERSION::UNSIGNED;
		} break;
		case ( 'o' ): {
			conversion = CONVERSION::INT | CONVERSION::OCTAL;
		} break;
		case ( 'x' ):
		case ( 'X' ): {
			conversion = CONVERSION::INT | CONVERSION::HEXADECIMAL;
		} break;
		case ( 'f' ):
		case ( 'F' ):
		case ( 'e' ):
		case ( 'E' ):
		case ( 'g' ):
		case ( 'G' ): {
			conversion = CONVERSION::DOUBLE;
		} break;
		case ( 'c' ): {
			conversion = CONVERSION::CHAR;
		} break;
		case ( 's' ): {
			conversion = CONVERSION::STRING;
		} break;
		case ( 'p' ): {
			conversion = CONVERSION::POINTER;
		} break;
		case ( '%' ): {
			if ( s[ i - 1 ] == '%' ) {
				break;
			}
		}
		/* % conversion can only occur immediately after %, eg: %% is ok but %*% is not */
		/* no break */
		default: {
			M_THROW( "bad conversion", s[ i ]  );
			break;
		}
	}
	char const* const BAD_LEN_MOD = _( "bad length mod." );
	switch ( lenMod ) {
		case ( 0 ):
		case ( 1 ):
		case ( 2 ):
		case ( 3 ):
		case ( 4 ): {
			if ( !!( conversion & CONVERSION::INT ) ) {
				conversion |= length[ lenMod ]._conversion;
			} else {
				M_THROW( BAD_LEN_MOD, lenMod );
			}
		} break;
		case ( 5 ): {
			if ( !!( conversion & CONVERSION::DOUBLE ) ) {
				conversion |= CONVERSION::LONG;
			} else if ( !!( conversion & CONVERSION::INT ) ) {
				conversion |= CONVERSION::LONG_LONG;
			} else {
				M_THROW( BAD_LEN_MOD, lenMod );
			}
		} break;
	}
	++ i;
	return ( conversion );
	M_EPILOG
}

HFormat::HFormatImpl::flag_t HFormat::HFormatImpl::get_flag( HString const& s, int& i ) {
	M_PROLOG
	int long len = s.get_length();
	flag_t flag = FLAG::NONE;
	bool done = false;
	for ( ; ! done && ( i < len ); ++ i ) {
		switch ( s[ i ] ) {
			case ( '#' ): flag |= FLAG::ALTERNATE; break;
			case ( '0' ): flag |= FLAG::ZERO_PADDED; break;
			case ( '-' ): flag |= FLAG::LEFT_ALIGNED; break;
			case ( ' ' ): flag |= FLAG::SPACE_PADDED; break;
			case ( '+' ): flag |= FLAG::SIGN_PREFIX; break;
			default:
				done = true;
			break;
		}
	}
	-- i;
	return ( flag );
	M_EPILOG
}

HFormat operator "" _yf ( char const* str_, size_t len_ ) {
	return ( HString( str_, static_cast<int>( len_ ) ) );
}

HString str( HFormat const& format_ ) {
	M_PROLOG
	return ( format_.string() );
	M_EPILOG
}

char const* HFormat::HFormatImpl::error_message( int ) const {
	M_PROLOG
	_errorMessage.format( _( "format: %s, at: %d" ), _format.raw(), _positionIndex );
	return ( _errorMessage.raw() );
	M_EPILOG
}

yaal::hcore::HStreamFormatProxy operator << ( yaal::hcore::HStreamInterface& stream, yaal::hcore::HFormat const& format ) {
	M_PROLOG
	return ( yaal::hcore::HStreamFormatProxy( stream, format ) );
	M_EPILOG
}

}

}

