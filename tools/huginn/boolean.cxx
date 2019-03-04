/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "builtin.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace boolean {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<huginn::HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::BOOLEAN ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::BOOLEAN ) ),
			"The `boolean` is a scalar type that is used to represent and operate on boolean values. "
			"It supports basic operations of negation, logical \"and\", \"or\", and \"xor\" and comparisons.",
			&builtin::boolean
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HBoolean::HBoolean( huginn::HClass const* class_, HBoolean::value_type value_ )
	: HValue( class_ )
	, _value( value_ ) {
	return;
}

HHuginn::value_t HBoolean::do_clone( huginn::HThread*, HHuginn::value_t* object_, int ) const {
	return ( *object_ );
}

}

}

