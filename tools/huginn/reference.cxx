/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

HHuginn::value_t HHuginn::HReference::do_get( huginn::HThread*, int ) const {
	return ( *_ref );
}

void HHuginn::HReference::do_set( huginn::HThread*, HHuginn::value_t&& value_, int ) {
	swap( *_ref, value_ );
}

HHuginn::value_t HHuginn::HReference::do_clone( huginn::HThread*, HHuginn::value_t*, int ) const {
	M_ASSERT( 0 && "cloning reference"[0] );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( HHuginn::value_t() );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

}

}

