/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  introfunction.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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
	bool isVariadic_
) : HFunction(
	name_,
	parameterNames_,
	scope_,
	defaults_,
	isVariadic_
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
	thread_->create_function_frame( this, object_, upcast( object_ ) );
	note_parameters( thread_ );
	HHuginn::value_t res( execute_impl( thread_, values_, position_ ) );
	thread_->pop_frame();
	return ( res );
	M_EPILOG
}

HHuginn::value_t HIntroFunction::execute_incremental_main(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) const {
	M_PROLOG
	thread_->create_incremental_function_frame( this, object_, upcast( object_ ) );
	note_parameters( thread_ );
	HHuginn::value_t res( execute_impl( thread_, values_, position_ ) );
	thread_->pop_incremental_frame();
	return ( res );
	M_EPILOG
}

}

}

}

