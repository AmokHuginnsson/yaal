/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hinfointeger.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HInfoInteger::HInfoInteger( int long long data_ )
	: _integer( data_ ) {
	return;
}

HInfoInteger::~HInfoInteger( void ) {
}

int long long HInfoInteger::do_get_integer( void ) const {
	return ( _integer );
}

double long HInfoInteger::do_get_real( void ) const {
	M_ASSERT( 0 && "data lost in conversion" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( 0 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

HString const& HInfoInteger::do_get_string( void ) const {
	M_ASSERT( 0 && "impossible inplace conversion requested" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	static HString const dummy;
	return dummy;
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

HTime const& HInfoInteger::do_get_time( void ) const {
	M_ASSERT( 0 && "impossible inplace conversion requested" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	static HTime const dummy( HTime::TZ::LOCAL );
	return dummy;
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

void HInfoInteger::do_set_integer( int long long data_ ) {
	_integer = data_;
	return;
}

void HInfoInteger::do_set_real( double long ) {
	M_ASSERT( 0 && "invalid API called" );
}

void HInfoInteger::do_set_string( yaal::hcore::HString const& data_ ) {
	M_PROLOG
	_integer = lexical_cast<int long long>( data_ );
	return;
	M_EPILOG
}

void HInfoInteger::do_set_time( yaal::hcore::HTime const& data_ ) {
	M_PROLOG
	_integer = data_.raw();
	return;
	M_EPILOG
}

}

}

