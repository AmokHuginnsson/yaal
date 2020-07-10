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
#include "enumeration.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HEnumeral::HEnumeral( HClass const* class_, HHuginn::identifier_id_t identifier_, value_type value_ )
	: HValue( class_ )
	, _identifier( identifier_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HEnumeral::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on enumerals.", thread_->file_id(), position_ );
}

bool HEnumeral::do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const& other_, int ) const {
	return ( _value == static_cast<HEnumeral const*>( other_.raw() )->_value );
}

yaal::hcore::HString HEnumeral::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker&, int ) const {
	enumeration::HEnumerationClass const* ec( static_cast<enumeration::HEnumerationClass::HEnumeralClass const*>( HValue::get_class() )->enumeration_class() );
	HRuntime& rt( thread_->runtime() );
	hcore::HString s( full_class_name( rt, ec ) );
	return ( s.append( "." ).append( rt.identifier_name( _identifier ) ) );
}

yaal::hcore::HString HEnumeral::do_to_string( huginn::HThread* thread_, HHuginn::value_t const& self_, HCycleTracker& cycleTracker_, int position_ ) const {
	return ( do_code( thread_, self_, cycleTracker_, position_ ) );
}

}

}

}

