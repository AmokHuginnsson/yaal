/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hboundcall.hxx"
#include "tools/huginn/statement.hxx"
#include "tools/huginn/operator.hxx"
#include "tools/huginn/frame.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;
class HArguments;

class HExpression : public HStatement {
public:
	typedef HExpression this_type;
	typedef HStatement base_type;
	struct OExecutionStep {
		typedef void ( HExpression::*action_t ) ( OExecutionStep const&, huginn::HFrame* );
		HExpression* _expression;
		action_t _action;
		int _position;
		HFrame::ACCESS _access;
		HStatement::statement_id_t _statementId;
		int _index;
		HHuginn::identifier_id_t _identifierId;
		HHuginn::value_t _value;
		HHuginn::value_t const* _valueRef;
		double long _real;
		int long long _integer;
		yaal::hcore::HString _string;
		code_point_t _character;
		OExecutionStep( void );
		OExecutionStep( HExpression*, action_t, int );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, int );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, HHuginn::identifier_id_t );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, HStatement::statement_id_t, int );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, HStatement::statement_id_t, int, HHuginn::identifier_id_t );
		OExecutionStep( HExpression*, action_t, int, HHuginn::value_t const* );
		OExecutionStep( HExpression*, action_t, int, HHuginn::value_t const& );
		OExecutionStep( HExpression*, action_t, int, double long );
		OExecutionStep( HExpression*, action_t, int, int long long );
		OExecutionStep( HExpression*, action_t, int, yaal::hcore::HString const& );
		OExecutionStep( HExpression*, action_t, int, code_point_t );
		OExecutionStep( OExecutionStep const& ) = default;
		OExecutionStep( OExecutionStep&& ) = default;
		OExecutionStep& operator = ( OExecutionStep const& ) = default;
		OExecutionStep& operator = ( OExecutionStep&& ) = default;
	};
private:
	typedef yaal::hcore::HArray<OExecutionStep> execution_steps_t;
	typedef yaal::hcore::HArray<OPositionedOperator> instructions_t;
	execution_steps_t _executionSteps;
	instructions_t _instructions;
	operations_t _operations;
public:
	HExpression( int = MAIN_FILE_ID, int = 0 );
	int execution_step_count( void ) const;
	int add_execution_step( OExecutionStep const& );
	OExecutionStep& execution_step( int );
	void replace_execution_step( int, OExecutionStep const& );
	void pop_execution_step( void );
	void merge( HExpression& );
	void oper( OPERATOR, int );
	void commit_oper( OPERATOR );
	void close_parenthesis( OExecutionStep const&, huginn::HFrame* );
	void plus( OExecutionStep const&, huginn::HFrame* );
	void minus( OExecutionStep const&, huginn::HFrame* );
	void mul( OExecutionStep const&, huginn::HFrame* );
	void div( OExecutionStep const&, huginn::HFrame* );
	void mod( OExecutionStep const&, huginn::HFrame* );
	void factorial( OExecutionStep const&, huginn::HFrame* );
	void negate( OExecutionStep const&, huginn::HFrame* );
	void function_call( OExecutionStep const&, huginn::HFrame* );
	void repack_named_parameters( OExecutionStep const&, huginn::HFrame* );
	void unpack_variadic_parameters( OExecutionStep const&, huginn::HFrame* );
	void make_assoc( OPERATOR, huginn::HFrame*, int );
	void make_dict( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
		make_assoc( OPERATOR::MAKE_DICT, frame_, executionStep_._position );
	}
	void make_lookup( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
		make_assoc( OPERATOR::MAKE_LOOKUP, frame_, executionStep_._position );
	}
	void get_field_direct( OExecutionStep const&, huginn::HFrame* );
	void get_variable_value( OExecutionStep const&, huginn::HFrame* );
	void get_variable_reference( OExecutionStep const&, huginn::HFrame* );
	void make_variable( OExecutionStep const&, huginn::HFrame* );
	void create_closure( OExecutionStep const&, huginn::HFrame* );
	void get_this( OExecutionStep const&, huginn::HFrame* );
	void get_super( OExecutionStep const&, huginn::HFrame* );
	void set_variable( OExecutionStep const&, huginn::HFrame* );
	void subscript( OExecutionStep const&, huginn::HFrame* );
	void range( OExecutionStep const&, huginn::HFrame* );
	void get_field( OExecutionStep const&, huginn::HFrame* );
	void power( OExecutionStep const&, huginn::HFrame* );
	void equals( OExecutionStep const&, huginn::HFrame* );
	void not_equals( OExecutionStep const&, huginn::HFrame* );
	void less( OExecutionStep const&, huginn::HFrame* );
	void greater( OExecutionStep const&, huginn::HFrame* );
	void less_or_equal( OExecutionStep const&, huginn::HFrame* );
	void greater_or_equal( OExecutionStep const&, huginn::HFrame* );
	void boolean_and( OExecutionStep const&, huginn::HFrame* );
	void boolean_or( OExecutionStep const&, huginn::HFrame* );
	void boolean_xor( OExecutionStep const&, huginn::HFrame* );
	void boolean_not( OExecutionStep const&, huginn::HFrame* );
	void ternary( OExecutionStep const&, huginn::HFrame* );
	void store_direct( OExecutionStep const&, huginn::HFrame* );
	void store_external_reference( OExecutionStep const&, huginn::HFrame* );
	void store_real( OExecutionStep const&, huginn::HFrame* );
	void store_integer( OExecutionStep const&, huginn::HFrame* );
	void store_string( OExecutionStep const&, huginn::HFrame* );
	void store_number( OExecutionStep const&, huginn::HFrame* );
	void store_character( OExecutionStep const&, huginn::HFrame* );
	bool is_empty( void ) const;
protected:
	void grab_args( HFrame*, HArguments& );
	virtual void do_execute( huginn::HThread* ) const override;
private:
	HExpression( HExpression const& ) = delete;
	HExpression& operator = ( HExpression const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED */

