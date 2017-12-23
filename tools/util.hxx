/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  util.hxx - this file is integral part of `yaal' project.

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
#include "tools/color.hxx"

namespace yaal {

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
private:
	typedef yaal::hcore::HBoundCall<> call_t;
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
	STATIC_ASSERT( is_numeric<T>::value );
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

