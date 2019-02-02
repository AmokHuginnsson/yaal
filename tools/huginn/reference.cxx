/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "reference.hxx"
#include "instruction.hxx"

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

namespace huginn {

HSubscriptReference::HSubscriptReference( HHuginn::HClass const* class_, HHuginn::value_t const& base_, HHuginn::value_t const& key_ )
	: HReference( class_, nullptr )
	, _base( base_ )
	, _key( key_ ) {
	return;
}

HHuginn::value_t HSubscriptReference::do_get( huginn::HThread* thread_, int position_ ) const {
	return ( instruction::subscript_value( thread_, _base, _key, position_ ) );
}

void HSubscriptReference::do_set( huginn::HThread* thread_, HHuginn::value_t&& value_, int position_ ) {
	instruction::subscript_assign( thread_, _base, _key, yaal::move( value_ ), position_ );
}

}

}

}

