/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HFunctionReference::HFunctionReference(
	huginn::HClass const* class_,
	HHuginn::identifier_id_t identifierId_,
	HHuginn::function_t const& function_,
	yaal::hcore::HString const& doc_,
	huginn::HClass const* juncture_
) : HValue( class_ )
	, _identifierId( identifierId_ )
	, _function( function_ )
	, _juncture( juncture_ )
	, _doc( doc_ ) {
	return;
}

void HFunctionReference::reset( HHuginn::function_t const& function_ ) {
	M_PROLOG
	_function = function_;
	return;
	M_EPILOG
}

HHuginn::value_t HFunctionReference::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_function_reference( _identifierId, _function, _doc ) );
}

yaal::hcore::HString const& HFunctionReference::doc( void ) const {
	return ( _doc );
}

bool HFunctionReference::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _identifierId == static_cast<huginn::HFunctionReference const*>( other_.raw() )->identifier_id() );
}

bool HFunctionReference::do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _identifierId < static_cast<huginn::HFunctionReference const*>( other_.raw() )->identifier_id() );
}

}

}

}

