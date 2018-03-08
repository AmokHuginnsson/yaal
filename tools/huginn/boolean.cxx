/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace boolean {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::BOOLEAN ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::BOOLEAN ) ),
			nullptr,
			"The `boolean` is a scalar type that is used to represent and operate on boolean values. "
			"It supports basic operations of negation, logical \"and\", \"or\", and \"xor\" and comparisons."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HBoolean::HBoolean( HHuginn::HClass const* class_, HHuginn::HBoolean::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HBoolean::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_boolean( _value ) );
}

}

}

