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

namespace real {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::REAL ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::REAL ) ),
			nullptr,
			HHuginn::field_definitions_t{},
			"The `real` is a scalar type that is used to represent and operate on floating point numbers. "
			"It supports basic operations of addition, subtraction, multiplication, division, modulo, power and comparisons, "
			"it can also be used as an argument in functions and algorithms from Mathematics package. "
			"The range of possible values it can hold is the same as `double long` from C++ programming language."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HReal::HReal( HHuginn::HClass const* class_, HHuginn::HReal::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HHuginn::HReal::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->runtime().object_factory()->create_real( _value ) );
}

}

}

