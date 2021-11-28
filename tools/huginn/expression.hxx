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
	static int const UNBOUND_INDEX_BASE = 0;
	struct OExecutionStep {
		typedef void ( HExpression::*action_t ) ( OExecutionStep const&, huginn::HFrame* );
		HExpression* _expression;
		action_t _action;
		int _position;
		HFrame::ACCESS _access;
		int _index;
		HHuginn::identifier_id_t _identifierId;
		HHuginn::value_t _value;
		HHuginn::value_t const* _valueRef;
		double long _real;
		int long long _integer;
		yaal::hcore::HString _string;
		yaal::hcore::HNumber _number;
		code_point_t _character;
		HHuginn::TYPE _literalType;
		HPartial::unbound_indexes_t _unboundIndexes;
		OExecutionStep( void );
		OExecutionStep( HExpression*, action_t, int );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, int );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, HHuginn::identifier_id_t );
		OExecutionStep( HExpression*, action_t, int, HFrame::ACCESS, int, HHuginn::identifier_id_t );
		OExecutionStep( HExpression*, action_t, int, HHuginn::value_t const* );
		OExecutionStep( HExpression*, action_t, int, HHuginn::value_t const&, HHuginn::TYPE = HHuginn::TYPE::UNKNOWN );
		OExecutionStep( HExpression*, action_t, int, double long );
		OExecutionStep( HExpression*, action_t, int, int long long );
		OExecutionStep( HExpression*, action_t, int, yaal::hcore::HString const& );
		OExecutionStep( HExpression*, action_t, int, yaal::hcore::HNumber const& );
		OExecutionStep( HExpression*, action_t, int, code_point_t );
		OExecutionStep( HExpression*, action_t, int, HPartial::unbound_indexes_t const&, int );
		OExecutionStep( OExecutionStep const& ) = default;
		OExecutionStep( OExecutionStep&& ) = default;
		OExecutionStep& operator = ( OExecutionStep const& ) = default;
		OExecutionStep& operator = ( OExecutionStep&& ) = default;
		OExecutionStep& operator += ( OExecutionStep const& );
		OExecutionStep& operator -= ( OExecutionStep const& );
		OExecutionStep& operator *= ( OExecutionStep const& );
		OExecutionStep& divide( OExecutionStep const&, int, int );
		OExecutionStep& modulo( OExecutionStep const&, int, int );
		OExecutionStep& power( OExecutionStep const&, int, int );
		OExecutionStep& negate( int, int );
		OExecutionStep& modulus( int, int );
		OExecutionStep& factorial( int, int );
	};
private:
	typedef yaal::hcore::HArray<OExecutionStep> execution_steps_t;
	typedef yaal::hcore::HArray<OPositionedOperator> instructions_t;
	typedef HClass const* ( HObjectFactory::* class_getter_t )( void ) const;
	execution_steps_t _executionSteps;
	instructions_t _instructions;
	operations_t _operations;
	bool _final;
public:
	HExpression( int = MAIN_FILE_ID, executing_parser::range_t = { 0, 0 } );
	int execution_step_count( void ) const;
	int add_execution_step( OExecutionStep const& );
	OExecutionStep& execution_step( int );
	void replace_execution_step( int, OExecutionStep const& );
	void pop_execution_step( void );
	void merge( HExpression& );
	void oper( OPERATOR, int );
	void commit_oper( OPERATOR, int, int );
	void try_collapse( int, int );
	void try_collapse_assign( int, int );
	void try_collapse_assign_action( HHuginn::TYPE, OExecutionStep::action_t, OExecutionStep::action_t );
	void try_collapse_assign_integer( int, int );
	void try_collapse_assign_real( int, int );
	void try_collapse_assign_number( int, int );
	void try_collapse_assign_string( int, int );
	void close_parenthesis( OExecutionStep const&, huginn::HFrame* );
	void plus( OExecutionStep const&, huginn::HFrame* );
	template<typename huginn_type, typename operator_type>
	void oper_assign_ref( HFrame*, OPERATOR, HHuginn::TYPE, class_getter_t, operator_type, typename huginn_type::value_type const& );
	template<typename huginn_type, typename operator_type>
	void oper_assign_val( HFrame*, OPERATOR, HHuginn::TYPE, class_getter_t, operator_type, typename huginn_type::value_type const& );
	template<OPERATOR>
	void oper_assign_integer_ref( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_integer_val( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_real_ref( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_real_val( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_number_ref( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_number_val( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_string_ref( OExecutionStep const&, huginn::HFrame* );
	template<OPERATOR>
	void oper_assign_string_val( OExecutionStep const&, huginn::HFrame* );
	void plus_assign_number( OExecutionStep const&, huginn::HFrame* );
	void plus_assign_string( OExecutionStep const&, huginn::HFrame* );
	void plus_integer( OExecutionStep const&, huginn::HFrame* );
	void plus_real( OExecutionStep const&, huginn::HFrame* );
	void plus_number( OExecutionStep const&, huginn::HFrame* );
	void plus_string( OExecutionStep const&, huginn::HFrame* );
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
	void make_partial( OExecutionStep const&, huginn::HFrame* );
	void get_field_direct( OExecutionStep const&, huginn::HFrame* );
	void get_field_ref_direct( OExecutionStep const&, huginn::HFrame* );
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
	void is_element_of( OExecutionStep const&, huginn::HFrame* );
	void is_not_element_of( OExecutionStep const&, huginn::HFrame* );
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
	HHuginn::value_t evaluate( huginn::HThread* ) const;
	void commit( huginn::HThread*, HHuginn::value_t&& ) const;
	void mark_final( void );
protected:
	void grab_args( HFrame*, HArguments& );
	HHuginn::values_t grab_args( HFrame*, HPartial::unbound_indexes_t& );
	virtual void do_execute( huginn::HThread* ) const override;
private:
	HExpression( HExpression const& ) = delete;
	HExpression& operator = ( HExpression const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_EXPRESSION_HXX_INCLUDED */

