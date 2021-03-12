/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "omenuitem.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

OMenuItem::OMenuItem( void )
	: _call(), _label() {
	M_PROLOG
	return;
	M_EPILOG
}

OMenuItem::OMenuItem( OMenuItem const& menuItem_ )
	: _call( menuItem_._call ), _label( menuItem_._label ) {
	M_PROLOG
	return;
	M_EPILOG
}

void OMenuItem::call( void ) const {
	M_PROLOG
	_call();
	return;
	M_EPILOG
}

yaal::hcore::HString const& OMenuItem::get_string( void ) const {
	return ( _label );
}

int long OMenuItem::get_integer( void ) const {
	M_ASSERT( 0 && "invalid call" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( 0 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

double OMenuItem::get_real( void ) const {
	M_ASSERT( 0 && "invalid call" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( 0 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

yaal::hcore::HTime const& OMenuItem::get_time( void ) const {
	M_ASSERT( 0 && "invalid call" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	static HTime t( HTime::TZ::LOCAL );
	return t;
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

}

}

