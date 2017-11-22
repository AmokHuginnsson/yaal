/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  function.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_FUNCTION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FUNCTION_HXX_INCLUDED 1

#include "runtime.hxx"
#include "helper.hxx"
#include "scope.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HFunction : public HStatement {
public:
	typedef HFunction this_type;
	typedef yaal::hcore::HArray<HHuginn::identifier_id_t> parameter_names_t;
	typedef HHuginn::expressions_t expressions_t;
	typedef void ( huginn::HThread::* function_frame_creator_t )( HStatement const*, HHuginn::value_t*, int );
	typedef void ( huginn::HThread::* function_frame_popper_t )( void );
private:
	HHuginn::identifier_id_t _name;
	parameter_names_t _parameterNames;
	int _defaultParametersStart;
	expressions_t _defaultValues;
	HHuginn::scope_t _scope;
	int _parameterCount;
	bool _isVariadic;
	bool _capturesNamedParameters;
public:
	HFunction( HHuginn::identifier_id_t, parameter_names_t const&, HHuginn::scope_t const&, expressions_t const&, bool, bool );
	HHuginn::value_t execute( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const;
	HHuginn::value_t execute_incremental_main( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const;
	HHuginn::identifier_id_t name( void ) const {
		return ( _name );
	}
protected:
	HHuginn::value_t execute_incremental_main_impl( huginn::HThread* thread_ ) const {
		HFrame* f( thread_->current_frame() );
		_scope->execute( thread_ );
		return ( f->result() );
	}
	HHuginn::value_t execute_impl( huginn::HThread*, HHuginn::values_t&, int ) const;
	void note_parameters( huginn::HThread* ) const;
	int upcast( HHuginn::value_t* ) const;
private:
	HFunction( HFunction const& ) = delete;
	HFunction( HFunction&& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FUNCTION_HXX_INCLUDED */

