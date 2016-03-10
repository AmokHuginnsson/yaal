/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  expression.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hboundcall.hxx"
#include "tools/huginn/statement.hxx"
#include "tools/huginn/operator.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;
class HFrame;

class HExpression : public HStatement {
public:
	typedef HExpression this_type;
	typedef HStatement base_type;
	enum class ACCESS {
		VALUE,
		REFERENCE
	};
private:
	typedef yaal::hcore::HBoundCall<void ( huginn::HFrame* )> execution_step_t;
	typedef yaal::hcore::HArray<execution_step_t> execution_steps_t;
	execution_steps_t _executionSteps;
public:
	HExpression( int = 0 );
	int add_execution_step( execution_step_t const& );
	void replace_execution_step( int, execution_step_t const& );
	void pop_execution_step( void );
	void merge( HExpression& );
	void oper( OPERATOR, huginn::HFrame*, int );
	void close_parenthesis( huginn::HFrame*, int );
	void plus( huginn::HFrame*, int );
	void minus( huginn::HFrame*, int );
	void mul( huginn::HFrame*, int );
	void div( huginn::HFrame*, int );
	void mod( huginn::HFrame*, int );
	void factorial( huginn::HFrame*, int );
	void negate( huginn::HFrame*, int );
	void function_call( huginn::HFrame*, int );
	void make_dict( huginn::HFrame*, int );
	void get_reference( HHuginn::identifier_id_t, huginn::HFrame*, int );
	void make_variable( HHuginn::identifier_id_t, huginn::HFrame*, int );
	void set_variable( huginn::HFrame*, int );
	void subscript( ACCESS, huginn::HFrame*, int );
	void range( huginn::HFrame*, int );
	void get_field( ACCESS, HHuginn::identifier_id_t, huginn::HFrame*, int );
	void power( huginn::HFrame*, int );
	void equals( huginn::HFrame*, int );
	void not_equals( huginn::HFrame*, int );
	void less( huginn::HFrame*, int );
	void greater( huginn::HFrame*, int );
	void less_or_equal( huginn::HFrame*, int );
	void greater_or_equal( huginn::HFrame*, int );
	void boolean_and( huginn::HFrame*, int );
	void boolean_or( huginn::HFrame*, int );
	void boolean_xor( huginn::HFrame*, int );
	void boolean_not( huginn::HFrame*, int );
	void ternary( huginn::HFrame*, int );
	void store_direct( HHuginn::value_t const&, huginn::HFrame*, int );
	void store_real( double long, huginn::HFrame*, int );
	void store_integer( int long long, huginn::HFrame*, int );
	void store_string( yaal::hcore::HString const&, huginn::HFrame*, int );
	void store_number( yaal::hcore::HString const&, huginn::HFrame*, int );
	void store_character( char, huginn::HFrame*, int );
	bool is_empty( void ) const;
protected:
	virtual void do_execute( huginn::HThread* ) const override;
private:
	HExpression( HExpression const& ) = delete;
	HExpression& operator = ( HExpression const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED */

