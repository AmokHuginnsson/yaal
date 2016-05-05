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
#include "tools/huginn/expression.hxx"
#include "tools/huginn/scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;
class HFunction;

class HFrame {
public:
	typedef HFrame this_type;
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, HHuginn::value_t> named_variables_t;
	typedef yaal::hcore::HStack<HHuginn::value_t> values_t;
	typedef yaal::hcore::HStack<int> instruction_pointers_t;
	enum class TYPE {
		SCOPE,
		LOOP,
		TRY_CATCH,
		FUNCTION
	};
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
	HHuginn::value_t* _object;
	int _upCast;
	HHuginn::values_t _variables;
	/*
	 * Instruction pointers must be kept per frame (and not in HThread)
	 * because new frame (new scope) can be created from within an expression
	 * by means of function call.
	 *
	 * Instruction pointers must be kept in form of stack to support subexpressions,
	 * e.g.: boolean short-circuits and ternary operators.
	 */
	instruction_pointers_t _instructionPointers;
	values_t _values;
	HHuginn::values_t _valueCache;
	HHuginn::value_t _result;
	int _number;
	TYPE _type;
	STATE _state;
	HStatement::statement_id_t _statementId;
public:
	HFrame( HThread*, HFrame* );
	void init( TYPE, HStatement::statement_id_t, HHuginn::value_t* = nullptr, int = 0 );
	HHuginn::value_t get_field( HExpression::ACCESS, int );
	HHuginn::value_t get_variable( HExpression::ACCESS, HStatement::statement_id_t, int );
	HHuginn::value_t get_this( void );
	HHuginn::value_t get_super( int );
	void add_variable( HHuginn::value_t const& );
	void commit_variable( HHuginn::value_t const&, int );
	bool can_continue( void ) const {
		return ( _state == STATE::NORMAL );
	}
	void break_execution( STATE );
	void continue_execution( void );
	int number( void ) const {
		return ( _number );
	}
	HFrame* parent( void ) {
		return ( _parent );
	}
	HThread* thread( void ) const {
		return ( _thread );
	}
	bool is_loop( void ) const {
		return ( _type == TYPE::LOOP );
	}
	bool has_catch( void ) const {
		return ( _type == TYPE::TRY_CATCH );
	}
	STATE state( void ) const {
		return ( _state );
	}
	HHuginn::value_t result( void ) const {
		return ( _result );
	}
	void set_result( HHuginn::value_t&& );
	values_t& values( void ) {
		return ( _values );
	}
	void reset( void );
	int& ip( void ) {
		return ( _instructionPointers.top() );
	}
	void start_expression( void ) {
		_instructionPointers.push( 0 );
	}
	void end_expression( void ) {
		_instructionPointers.pop();
	}
	HHuginn::values_t& value_cache( void ) {
		return ( _valueCache );
	}
private:
	HFrame( HFrame const& ) = delete;
	HFrame& operator = ( HFrame const& ) = delete;
	HHuginn::value_t* object( void ) const;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED */

