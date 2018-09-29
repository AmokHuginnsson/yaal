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

namespace integer {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::INTEGER ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::INTEGER ) ),
			"The `integer` is a scalar type that is used to represent and operate on integers. "
			"It supports basic operations of addition, subtraction, multiplication, division, modulo and comparisons. "
			"The range of possible values it can hold is [-2^32, 2^32).",
			&builtin::integer
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HInteger::HInteger( HHuginn::HClass const* class_, HHuginn::HInteger::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HInteger::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_integer( _value ) );
}

}

}

