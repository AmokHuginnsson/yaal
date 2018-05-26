/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/util.hxx
 * \brief Simple single function utilities.
 */

#ifndef YAAL_TOOLS_UTIL_HXX_INCLUDED
#define YAAL_TOOLS_UTIL_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/hstring.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/programoptionshelper.hxx"
#include "tools/hoptional.hxx"
#include "tools/color.hxx"

namespace yaal {

namespace program_options_helper {

template<typename tType>
void set_option_value_from_string( yaal::tools::HOptional<tType>& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	tType tmp;
	set_option_value_from_string( tmp, value );
	object = tmp;
	return;
	M_EPILOG
}

template<typename tType>
yaal::hcore::HString get_string_from_option_value( yaal::tools::HOptional<tType> const& value_ ) {
	M_PROLOG
	return ( value_ ? get_string_from_option_value( *value_ ) : "" );
	M_EPILOG
}

}

namespace hcore {

class HProgramOptionsHandler;

}

namespace tools {

/*! \brief Small but useful utilities.
 */
namespace util {

typedef yaal::hcore::HHashMap<yaal::code_point_t, yaal::code_point_t> code_point_translation_t;

class HScopeExitCall {
public:
	typedef HScopeExitCall this_type;
	typedef yaal::hcore::HBoundCall<> call_t;
private:
	call_t _call;
public:
	HScopeExitCall( call_t&& call_ )
		: _call( yaal::move( call_ ) ) {
		return;
	}
	~HScopeExitCall( void ) {
		M_SAFE( _call() );
	}
};

template<typename T>
inline T hton( T v_ ) {
	static_assert( is_numeric<T>::value, "switching endianness on non integer type" );
	reverse( static_cast<char*>( static_cast<void*>( &v_ ) ), static_cast<char*>( static_cast<void*>( &v_ ) ) + sizeof ( v_ ) );
	return ( v_ );
}

template<typename iter_t, typename pred_t>
iter_t find_local( iter_t it, iter_t end, pred_t pred ) {
	for ( ; it != end; ++ it )
		pred( it );
	return ( pred() );
}
yaal::hcore::HString& usun_ogonki( yaal::hcore::HString& );
yaal::hcore::HString article( yaal::hcore::HString const& );
double long atof_ex( yaal::hcore::HString const&, bool = false );
yaal::hcore::HString get_token( yaal::hcore::HString const&, yaal::hcore::HString const&, int long );
yaal::hcore::HString cardinal( int );
yaal::hcore::HString ordinal( int );
char const* near_keys( char );

class HTheme {
	COLOR::color_t _strong;
	COLOR::color_t _emphasis;
	COLOR::color_t _code;
	COLOR::color_t _special;
public:
	HTheme(
		COLOR::color_t strong_ = COLOR::ATTR_BOLD,
		COLOR::color_t emphasis_ = COLOR::ATTR_UNDERLINE,
		COLOR::color_t code_ = COLOR::FG_GREEN,
		COLOR::color_t special_ = COLOR::FG_YELLOW
	) : _strong( strong_ )
		, _emphasis( emphasis_ )
		, _code( code_ )
		, _special( special_ ) {
	}
	COLOR::color_t strong( void ) const {
		return ( _strong );
	}
	COLOR::color_t emphasis( void ) const {
		return ( _emphasis );
	}
	COLOR::color_t code( void ) const {
		return ( _code );
	}
	COLOR::color_t special( void ) const {
		return ( _special );
	}
};
yaal::hcore::HString highlight( yaal::hcore::HString const&, HTheme const& = HTheme() );

/*! \brief Helper structure for displaying program help and current configuration.
 */
class HOptionInfo {
	yaal::hcore::HProgramOptionsHandler const& _opt;
	char const* _name;
	char const* _intro;
	char const* _syntax;
	char const* _description;
	char const* _note;
	HTheme _theme;
	bool _color;
	bool _markdown;
public:
	HOptionInfo(
		yaal::hcore::HProgramOptionsHandler const& opt_
	) : _opt( opt_ )
		, _name( nullptr )
		, _intro( nullptr )
		, _syntax( nullptr )
		, _description( nullptr )
		, _note( nullptr )
		, _theme()
		, _color( true )
		, _markdown( false ) {
	}
	HOptionInfo& name( char const* );
	char const* name( void ) const;
	HOptionInfo& intro( char const* );
	char const* intro( void ) const;
	HOptionInfo& syntax( char const* );
	char const* syntax( void ) const;
	HOptionInfo& description( char const* );
	char const* description( void ) const;
	HOptionInfo& note( char const* );
	char const* note( void ) const;
	HOptionInfo& theme( HTheme const& );
	HTheme const& theme( void ) const;
	HOptionInfo& color( bool );
	bool color( void ) const;
	HOptionInfo& markdown( bool );
	bool markdown( void ) const;
	yaal::hcore::HProgramOptionsHandler const& opt( void ) const;
private:
	HOptionInfo( HOptionInfo const& );
	HOptionInfo& operator = ( HOptionInfo const& );
};

void show_help( HOptionInfo const&, yaal::hcore::HStreamInterface& = yaal::hcore::cout );
void dump_configuration( HOptionInfo const&, yaal::hcore::HStreamInterface& = yaal::hcore::cout );

void failure( int, char const*, ... ) __attribute__(( __noreturn__, format( printf, 2, 3 ) ));

}

}

}

#endif /* #ifndef YAAL_TOOLS_UTIL_HXX_INCLUDED */

