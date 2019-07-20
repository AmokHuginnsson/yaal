/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>
#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hformat.hxx"
#include "hcore/memory.hxx"
#include "harray.hxx"
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
		static conversion_t const CODE_POINT;
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
	typedef HArray<OToken> tokens_t;
	typedef HMap<int, int> positions_t;
	typedef HPointer<positions_t> positions_ptr_t;
	typedef HVariant<bool, char, code_point_t, int short, int, int long, int long long, void const*, float, double, double long, HString> format_arg_t;
	typedef HMap<int, format_arg_t> args_t;
	typedef HPointer<args_t> args_ptr_t;
	int _positionIndex;
	HString _format;
	mutable HString _buffer;
	mutable HString _string;
	tokens_t _tokens;
	positions_ptr_t _positions;
	args_ptr_t _args;
	mutable HUTF8String _errorMessage;
	HFormatImpl( HString const& );
	HFormatImpl( HFormatImpl const& );
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
	void format_string( yaal::hcore::HString const&, flag_t, int, int ) const;
	HFormatImpl& operator = ( HFormatImpl const& ) = delete;
	friend class HFormat;
	template<typename tType, typename... arg_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( arg_t&&... );
};

HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::EMPTY = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::INT = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::DOUBLE = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::STRING = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::CHAR = HFormat::HFormatImpl::conversion_t::new_flag();
HFormat::HFormatImpl::conversion_t const HFormat::HFormatImpl::CONVERSION::CODE_POINT = HFormat::HFormatImpl::conversion_t::new_flag();
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
	, _positions( make_pointer<positions_t>( *fi._positions ) )
	, _args( make_pointer<args_t>( *fi._args ) )
	, _errorMessage( fi._errorMessage ) {
	return;
}

HFormat::HFormat( HString const& aFmt_ )
	: _impl( make_pointer<HFormatImpl>( aFmt_ ) ) {
	M_PROLOG
	HFormatImpl::OToken t;
	int idx = 0;
	HFormatImpl& impl( *_impl );
	while ( ! _isKilled_ ) {
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
	for ( HFormatImpl::tokens_t::iterator beg( _impl->_tokens.begin() ), it( _impl->_tokens.begin() ), end( _impl->_tokens.end() ); it != end; ++ it, ++ pos ) {
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
			_impl->_positions->insert( make_pair( - ( it->_width + 2 ), -1 ) );
			++ pos;
		}
		if ( it->_precision == -1 ) {
			it->_precision = ( - pos ) - 2;
		}
		if ( it->_precision < 0 ) {
			precIdxs.insert( - ( it->_precision + 2 ) );
			_impl->_positions->insert( make_pair( - ( it->_precision + 2 ), -1 ) );
			++ pos;
		}
		if ( it->_position > 0 ) {
			idxs.insert( -- it->_position );
		} else {
			it->_position = pos;
		}
		_impl->_positions->insert( make_pair( it->_position, static_cast<int>( it - beg ) ) );
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

HFormat::HFormat( format_impl_ptr_t impl_ )
	: _impl( impl_ ) {
}

HFormat::HFormat( HFormat const& other_ )
	: _impl( make_pointer<HFormat::HFormatImpl>( *(other_._impl) ) ) {
}

HFormat::HFormat( HFormat&& other_ )
	: _impl( yaal::move( other_._impl ) ) {
	return;
}

HFormat& HFormat::operator = ( HFormat const& other_ ) {
	M_PROLOG
	if ( &other_ != this ) {
		HFormat tmp( other_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HFormat& HFormat::operator = ( HFormat&& other_ ) {
	if ( &other_ != this ) {
		swap( other_ );
		other_._impl.reset();
	}
	return ( *this );
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
	static int const MAX_FMT_SIZE( 64 );
	char fmt[MAX_FMT_SIZE];
	M_ENSURE( _impl->_positions->size() == _impl->_args->size() );
	_impl->_string.clear();
	for ( HFormatImpl::tokens_t::const_iterator it = _impl->_tokens.begin(), end = _impl->_tokens.end(); it != end; ++ it ) {
		char* fp( fmt );
		HFormatImpl::conversion_t conv = it->_conversion;
		if ( conv == HFormatImpl::CONVERSION::CONSTANT ) {
			_impl->_string += it->_const;
		} else {
			*fp++ = '%';
			if ( !!( it->_flag & HFormatImpl::FLAG::ALTERNATE ) ) {
				*fp++ = '#';
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::ZERO_PADDED ) ) {
				*fp++ = '0';
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::SPACE_PADDED ) ) {
				*fp++ = ' ';
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::SIGN_PREFIX ) ) {
				*fp++ = '+';
			}
			if ( !!( it->_flag & HFormatImpl::FLAG::LEFT_ALIGNED ) ) {
				*fp++ = '-';
			}
			int w( 0 );
			if ( it->_width > 0 ) {
				w = it->_width;
			} else if ( it->_width < 0 ) {
				w = HFormatImpl::variant_shell<int>::get( *_impl->_args, - ( it->_width + 2 ) );
			}
			if ( w > 0 ) {
				fp += snprintf( fp, static_cast<size_t>( ( MAX_FMT_SIZE - 1 ) - ( fp - fmt ) ), "%d", w );
			}
			if ( it->_precision ) {
				*fp++ = '.';
			}
			int p( 0 );
			if ( it->_precision > 0 ) {
				p = it->_precision;
			} else if ( it->_precision < 0 ) {
				p = HFormatImpl::variant_shell<int>::get( *_impl->_args, - ( it->_precision + 2 ) );
			}
			if ( p > 0 ) {
				fp += snprintf( fp, static_cast<size_t>( ( MAX_FMT_SIZE - 1 ) - ( fp - fmt ) ), "%d", p );
			}
			if ( !!( conv & HFormatImpl::CONVERSION::BYTE ) ) {
				*fp++ = 'h';
				*fp++ = 'h';
			} else if ( !!( conv & HFormatImpl::CONVERSION::SHORT ) ) {
				*fp++ = 'h';
			} else if ( !!( conv & HFormatImpl::CONVERSION::LONG ) ) {
				if ( !!( conv & HFormatImpl::CONVERSION::INT ) ) {
					*fp++ = 'l';
				} else {
					M_ASSERT( !!( conv & HFormatImpl::CONVERSION::DOUBLE ) );
					*fp++ = 'L';
				}
			} else if ( !!( conv & HFormatImpl::CONVERSION::LONG_LONG ) ) {
				*fp++ = 'l';
				*fp++ = 'l';
			}
			if ( !!( conv & HFormatImpl::CONVERSION::OCTAL ) ) {
				*fp++ = 'o';
			} else if ( !!( conv & HFormatImpl::CONVERSION::HEXADECIMAL ) ) {
				*fp++ = 'x';
			} else if ( !!( conv & HFormatImpl::CONVERSION::UNSIGNED ) ) {
				*fp++ = 'u';
			} else if ( !!( conv & HFormatImpl::CONVERSION::INT ) ) {
				*fp++ = 'd';
			} else if ( !!( conv & HFormatImpl::CONVERSION::DOUBLE ) ) {
				if ( !!( conv & HFormatImpl::CONVERSION::SHORT ) ) {
					-- fp;
				}
				*fp++ = 'f';
			} else if ( !!( conv & HFormatImpl::CONVERSION::STRING ) ) {
				*fp++ = 's';
			} else if ( !!( conv & HFormatImpl::CONVERSION::CHAR ) ) {
				*fp++ = 'c';
			} else if ( !!( conv & HFormatImpl::CONVERSION::CODE_POINT ) ) {
				*fp++ = 'C';
			} else if ( !!( conv & HFormatImpl::CONVERSION::POINTER ) ) {
				*fp++ = 'p';
			}
			*fp = 0;
			static int const MAX_FLOAT_DIGIT_COUNT( 8192 );
			char buffer[MAX_FLOAT_DIGIT_COUNT] = "\0";
			if ( !!( conv & HFormatImpl::CONVERSION::INT ) ) {
				if ( !!( conv & HFormatImpl::CONVERSION::BYTE ) ) {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<char>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & HFormatImpl::CONVERSION::SHORT ) ) {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<int short>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & HFormatImpl::CONVERSION::LONG ) ) {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<int long>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & HFormatImpl::CONVERSION::LONG_LONG ) ) {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<int long long>::get( *_impl->_args, it->_position ) );
				} else {
					M_ASSERT( ( conv & ~( HFormatImpl::CONVERSION::UNSIGNED | HFormatImpl::CONVERSION::OCTAL | HFormatImpl::CONVERSION::HEXADECIMAL ) ) == HFormatImpl::CONVERSION::INT );
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<int>::get( *_impl->_args, it->_position ) );
				}
			} else if ( !!( conv & HFormatImpl::CONVERSION::STRING ) ) {
				HString const& s( HFormatImpl::variant_shell<HString>::get( *_impl->_args, it->_position ) );
				_impl->format_string( s, it->_flag, p, w );
			} else if ( !!( conv & HFormatImpl::CONVERSION::POINTER ) ) {
				snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<void const*>::get( *_impl->_args, it->_position ) );
			} else if ( !!( conv & HFormatImpl::CONVERSION::CHAR ) ) {
				snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<char>::get( *_impl->_args, it->_position ) );
			} else if ( !!( conv & HFormatImpl::CONVERSION::CODE_POINT ) ) {
				_impl->format_string( HFormatImpl::variant_shell<code_point_t>::get( *_impl->_args, it->_position ), it->_flag, p, w );
			} else if ( !!( conv & HFormatImpl::CONVERSION::DOUBLE ) ) {
				if ( !!( conv & ( HFormatImpl::CONVERSION::LONG ) ) ) {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<double long>::get( *_impl->_args, it->_position ) );
				} else if ( !!( conv & ( HFormatImpl::CONVERSION::SHORT ) ) ) {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<float>::get( *_impl->_args, it->_position ) );
				} else {
					snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, HFormatImpl::variant_shell<double>::get( *_impl->_args, it->_position ) );
				}
			}
			_impl->_string.append( buffer[0] ? buffer : _impl->_buffer );
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

HFormat HFormat::operator % ( code_point_t c ) {
	M_PROLOG
	M_ENSURE( ! _impl->_format.is_empty() );
	int idx = _impl->next_token( HFormatImpl::CONVERSION::CODE_POINT );
	_impl->_args->insert( make_pair( idx, HFormatImpl::format_arg_t( c ) ) );
	return ( _impl );
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
	int idx = _impl->next_token( HFormatImpl::CONVERSION::DOUBLE | HFormatImpl::CONVERSION::SHORT );
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
	int pos( it->second );
	OToken* t( pos >= 0 ? &_tokens[pos] : nullptr );
	M_ENSURE(
		( t && (
			( conv == ( t->_conversion & ~( HFormatImpl::CONVERSION::OCTAL | HFormatImpl::CONVERSION::HEXADECIMAL ) ) )
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
	if ( t._conversion == CONVERSION::CONSTANT ) {
		t._const = "%";
	}
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
	int token = static_cast<int>( s.find( '%'_ycp, i ) );
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
		enum LENGTH { hh, h, ll, l, q, L };
		char const* _label;
		conversion_t _conversion;
		LENGTH _length;
	} const lengthConversions[] = {
		{ "hh", CONVERSION::BYTE,      OLength::hh },
		{ "h",  CONVERSION::SHORT,     OLength::h },
		{ "ll", CONVERSION::LONG_LONG, OLength::ll },
		{ "l",  CONVERSION::LONG,      OLength::l },
		{ "q",  CONVERSION::LONG_LONG, OLength::q },
		{ "L",  CONVERSION::LONG_LONG, OLength::L }
	};
	OLength const* cl( nullptr );
	for ( OLength const& len : lengthConversions ) {
		if ( s.find( len._label, i ) == i ) {
			i += static_cast<int>( ::std::strlen( len._label ) );
			cl = &len;
			break;
		}
	}
	M_ENSURE( i < s.get_length() );
	switch ( s[ i ].get() ) {
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
		case ( 'C' ): {
			conversion = CONVERSION::CODE_POINT;
		} break;
		case ( 's' ): {
			conversion = CONVERSION::STRING;
		} break;
		case ( 'p' ): {
			conversion = CONVERSION::POINTER;
		} break;
		case ( '%' ): {
			if ( s[ i - 1 ] == '%' ) {
				conversion = CONVERSION::CONSTANT;
				break;
			}
		} /* fallthrough */
		/* % conversion can only occur immediately after %, eg: %% is ok but %*% is not */
		/* no break */
		default: {
			M_THROW( "bad conversion", s[ i ].get()  );
			break;
		}
	}
	char const* const BAD_LEN_MOD = _( "bad lengthConversions mod." );
	if ( cl ) {
		switch ( cl->_length ) {
			case ( OLength::hh ):
			case ( OLength::h ): {
				if ( !!( conversion & CONVERSION::DOUBLE ) ) {
					conversion |= cl->_conversion;
					break;
				}
			} /* fall-through */
			case ( OLength::ll ):
			case ( OLength::l ):
			case ( OLength::q ): {
				if ( !!( conversion & CONVERSION::INT ) ) {
					conversion |= cl->_conversion;
				} else {
					M_THROW( BAD_LEN_MOD, cl->_length );
				}
			} break;
			case ( OLength::L ): {
				if ( !!( conversion & CONVERSION::DOUBLE ) ) {
					conversion |= CONVERSION::LONG;
				} else if ( !!( conversion & CONVERSION::INT ) ) {
					conversion |= CONVERSION::LONG_LONG;
				} else {
					M_THROW( BAD_LEN_MOD, cl->_length );
				}
			} break;
		}
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
		switch ( s[ i ].get() ) {
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

void HFormat::HFormatImpl::format_string( yaal::hcore::HString const& string_, flag_t flag_, int precision_, int width_ ) const {
	M_PROLOG
	int len( static_cast<int>( string_.get_length() ) );
	if ( precision_ ) {
		len = min( precision_, len );
	}
	_buffer.clear();
	bool leftAligned( !!( flag_ & HFormatImpl::FLAG::LEFT_ALIGNED ) );
	int spaces( width_ > len ? width_ - len : 0 );
	if ( ( spaces > 0 ) && ! leftAligned ) {
		_buffer.append( spaces, ' '_ycp );
	}
	_buffer.append( string_, 0, len );
	if ( ( spaces > 0 ) && leftAligned ) {
		_buffer.append( spaces, ' '_ycp );
	}
	return;
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
	_buffer.assign( "format: " ).append( _format ).append( ", at: " ).append( _positionIndex );
	_errorMessage = _buffer;
	return ( _errorMessage.c_str() );
	M_EPILOG
}

yaal::hcore::HStreamFormatProxy operator << ( yaal::hcore::HStreamInterface& stream, yaal::hcore::HFormat const& format ) {
	M_PROLOG
	return ( yaal::hcore::HStreamFormatProxy( stream, format ) );
	M_EPILOG
}

}

}

