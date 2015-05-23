/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  frame.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/operator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;

class HFrame {
public:
	typedef HFrame this_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::value_t> variables_t;
	typedef yaal::hcore::HStack<HHuginn::value_t> values_t;
	enum class STATE {
		NORMAL,
		RETURN,
		BREAK,
		CONTINUE,
		EXCEPTION,
		RUNTIME_EXCEPTION
	};
private:
	HThread* _thread;
	HFrame* const _parent;
	HHuginn::HObject* _object;
	variables_t _variables;
	operations_t _operations;
	values_t _values;
	HHuginn::value_t _result;
	int const _number;
	bool _loop;
	STATE _state;
public:
	HFrame( HThread*, HFrame*, HHuginn::HObject*, bool, bool );
	HHuginn::value_t make_variable( yaal::hcore::HString const&, int );
	void set_variable( yaal::hcore::HString const&, HHuginn::value_t const&, int );
	HHuginn::value_t get_reference( yaal::hcore::HString const&, int );
	HHuginn::value_t try_reference( yaal::hcore::HString const&, int );
	bool can_continue( void ) const;
	void break_execution( STATE );
	int number( void ) const;
	HFrame* parent( void );
	HThread* thread( void ) const;
	bool is_loop( void ) const;
	HHuginn::value_t result( void ) const;
	void set_result( HHuginn::value_t const& );
	operations_t& operations( void );
	values_t& values( void );
	void reset( void );
	HHuginn::HObject* object( void ) const;
private:
	HFrame( HFrame const& ) = delete;
	HFrame& operator = ( HFrame const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED */

