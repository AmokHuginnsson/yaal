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

HHuginn::value_t HFunctionReference::do_operator_call( huginn::HThread* thread_, HHuginn::value_t& object_, HHuginn::values_t& arguments_, int position_ ) {
	return ( _function( thread_, &object_, arguments_, position_ ) );
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

bool HFunctionReference::do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _identifierId > static_cast<huginn::HFunctionReference const*>( other_.raw() )->identifier_id() );
}

bool HFunctionReference::do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _identifierId <= static_cast<huginn::HFunctionReference const*>( other_.raw() )->identifier_id() );
}

bool HFunctionReference::do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _identifierId >= static_cast<huginn::HFunctionReference const*>( other_.raw() )->identifier_id() );
}

int long HFunctionReference::do_operator_hash( HThread*, HHuginn::value_t const&, int ) const {
	return ( hcore::hash<int long>()( _identifierId.get() ) );
}

yaal::hcore::HString HFunctionReference::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker&, int ) const {
	if ( ! thread_ ) {
		return ( type_name( HHuginn::TYPE::FUNCTION_REFERENCE ) );
	}
	HRuntime& r( thread_->runtime() );
	HClass const* c( r.get_class( _function.id() ) );
	hcore::HString const* originName( !!c ? r.package_name( c->origin() ) : nullptr );
	hcore::HString str;
	if ( originName ) {
		str.assign( *originName ).append( "." );
	}
	str.append( r.identifier_name( _identifierId ) );
	return ( str );
}

yaal::hcore::HString HFunctionReference::do_to_string( huginn::HThread* thread_, HHuginn::value_t const& self_, HCycleTracker& cycleTracker_, int position_ ) const {
	return ( do_code( thread_, self_, cycleTracker_, position_ ) );
}

}

}

}

