/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "introfunction.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HIntroFunction::HIntroFunction(
	HHuginn::identifier_id_t name_,
	parameter_names_t const& parameterNames_,
	HHuginn::scope_t const& scope_,
	expressions_t const& defaults_,
	bool isVariadic_,
	bool capturesNamedParameters_
) : HFunction(
	name_,
	parameterNames_,
	scope_,
	defaults_,
	isVariadic_,
	capturesNamedParameters_
) {
	return;
}

HHuginn::value_t HIntroFunction::execute(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) const {
	M_PROLOG
	thread_->create_frame( this, object_, upcast( object_ ) );
	note_parameters( thread_ );
	HHuginn::value_t res( execute_impl( thread_, values_, position_ ) );
	thread_->pop_frame();
	return res;
	M_EPILOG
}

HHuginn::value_t HIntroFunction::execute_incremental_main(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t&,
	int
) const {
	M_PROLOG
	thread_->create_incremental_frame( this, object_, upcast( object_ ) );
	note_parameters( thread_ );
	HHuginn::value_t res( execute_incremental_main_impl( thread_ ) );
	thread_->pop_incremental_frame();
	return res;
	M_EPILOG
}

}

}

}

