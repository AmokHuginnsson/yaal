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

#include "tools/hhuginn.hxx"
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HFunction : public HStatement {
public:
	typedef HFunction this_type;
	typedef HHuginn::expressions_t expressions_t;
	typedef void ( huginn::HThread::* function_frame_creator_t )( HStatement const*, HHuginn::value_t*, int );
	typedef void ( huginn::HThread::* function_frame_popper_t )( void );
private:
	HHuginn::identifier_id_t _name;
	int _parameterCount;
	int _defaultParametersStart;
	expressions_t _defaultValues;
	HHuginn::scope_t _scope;
	bool _isVariadic;
public:
	HFunction( HHuginn::identifier_id_t, int, HHuginn::scope_t const&, expressions_t const&, bool );
	HHuginn::value_t execute( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int ) const;
	HHuginn::value_t execute_incremental_main( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int ) const;
	HHuginn::identifier_id_t name( void ) const {
		return ( _name );
	}
protected:
	HHuginn::value_t execute_impl( huginn::HThread*, HHuginn::values_t const&, int ) const;
	int upcast( HHuginn::value_t* ) const;
private:
	HFunction( HFunction const& ) = delete;
	HFunction( HFunction&& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FUNCTION_HXX_INCLUDED */

