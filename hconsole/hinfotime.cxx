/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hinfotime.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HInfoTime::HInfoTime( HTime& string_ )
	: _data( string_ ) {
}

HInfoTime::~HInfoTime( void ) {
	return;
}

HString const& HInfoTime::do_get_string( void ) const {
	M_ASSERT( 0 && "impossible inplace conversion requested" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	static HString const dummy;
	return dummy;
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

int long long HInfoTime::do_get_integer( void ) const {
	return ( _data.raw() );
}

double long HInfoTime::do_get_real( void ) const {
	M_ASSERT( 0 && "data lost in conversion" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( 0 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

HTime const& HInfoTime::do_get_time( void ) const {
	return ( _data );
}

void HInfoTime::do_set_integer( int long long intLong_ ) {
	_data = HTime( intLong_, _iso8601DateTimeFormat_ );
}

void HInfoTime::do_set_real( double long ) {
	M_ASSERT( 0 && "invalid API called" );
}

void HInfoTime::do_set_string( HString const& str_ ) {
	_data.from_string( str_ );
}

void HInfoTime::do_set_time( HTime const& time_ ) {
	_data = time_;
}

}

}

