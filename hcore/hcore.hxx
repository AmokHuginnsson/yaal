/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hcore.hxx
 * \brief Basic system level stuff.
 *
 * This file holds (except HException class declaration) main #defines,
 * macros and global variables used acros whole yaal.
 */

#ifndef YAAL_HCORE_HCORE_HXX_INCLUDED
#define YAAL_HCORE_HCORE_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace hcore {

class HProgramOptionsHandler;

HProgramOptionsHandler& yaal_options( void );
char const* yaal_version( bool );
char const* host_info_string( void );
bool eq( double long, double long );
void set_env( HString const&, HString const&, bool = true );
void unset_env( HString const& );
void decode_set_env( HString );
enum class ENV_SUBST_MODE {
	RECURSIVE,
	ONE_LAYER
};
HString bytes_to_string( char const*, HString::size_type = HString::npos );
void bytes_to_string( HString&, char const*, HString::size_type = HString::npos );
typedef yaal::hcore::HArray<char> bytes_t;
bytes_t string_to_bytes( HString const& );
void substitute_environment( HString&, ENV_SUBST_MODE, code_point_t = code_point_t( '\\' ) );
yaal::hcore::HString substitute_environment( yaal::hcore::HString const& );
void init_locale( char const* = nullptr );
void banner( char const* = nullptr, char const* = nullptr );
double long strtold( HString const&, int* = nullptr );

namespace extendable {

typedef double long ( *yaal_strtold_t )( HString const&, int* );
void set_strtold_impl( yaal_strtold_t );

}

}

}

#endif /* #ifndef YAAL_HCORE_HCORE_HXX_INCLUDED */

