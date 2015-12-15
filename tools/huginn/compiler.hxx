/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  compiler.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hstack.hxx"
#include "tools/huginn/operator.hxx"
#include "tools/huginn/function.hxx"
#include "tools/huginn/trycatch.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HExpression;
class HFrame;

struct OCompiler {
	typedef void ( HExpression::* expression_action_t ) ( HFrame*, int );
	typedef yaal::hcore::HStack<HFunction::expressions_t> expressions_stack_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, HHuginn::type_id_t> variable_types_t;
	struct OActiveScope {
		HHuginn::scope_t _scope;
		HHuginn::expression_t _expression;
		OActiveScope( HHuginn::scope_t&, HHuginn::expression_t& );
	};
	struct OScopeContext {
		typedef yaal::hcore::HArray<OActiveScope> active_scopes_t;
		OScopeContext* _parent;
		HHuginn::scope_t _scope;
		expressions_stack_t _expressionsStack;
		variable_types_t _variableTypes;
		yaal::hcore::HString _type;
		yaal::hcore::HString _identifier;
		int _position;
		active_scopes_t _scopeChain;
		HHuginn::scope_t _else;
		HTryCatch::catches_t _catches;
		OScopeContext( OScopeContext* );
		HHuginn::expression_t& expression( void );
		HHuginn::type_id_t guess_type( yaal::hcore::HString const& ) const;
		void note_type( yaal::hcore::HString const&, HHuginn::type_id_t );
		void clear( void );
	private:
		OScopeContext( OScopeContext const& ) = delete;
		OScopeContext& operator = ( OScopeContext const& ) = delete;
	};
	struct OFunctionContext {
		typedef yaal::hcore::HResource<OScopeContext> scope_context_t;
		typedef yaal::hcore::HStack<scope_context_t> scope_stack_t;
		typedef yaal::hcore::HStack<HHuginn::type_id_t> type_stack_t;
		typedef yaal::hcore::HStack<yaal::hcore::HString> variable_stack_t;
		yaal::hcore::HString _functionName;
		HFunction::parameter_names_t _parameters;
		HFunction::expressions_t _defaultValues;
		int _lastDefaultValuePosition;
		scope_stack_t _scopeStack;
		operations_t _operations;
		type_stack_t _valueTypes;
		variable_stack_t _variables;
		int _loopCount;
		int _loopSwitchCount;
		int _nestedCalls;
		OPERATOR _lastDereferenceOperator;
		bool _isAssert;
		yaal::hcore::HString _lastMemberName;
		OFunctionContext( void );
		expressions_stack_t& expressions_stack( void );
	};
	typedef yaal::hcore::HResource<OFunctionContext> function_context_t;
	typedef yaal::hcore::HStack<function_context_t> function_contexts_t;
	struct OClassContext {
		typedef yaal::hcore::HArray<yaal::hcore::HString> field_names_t;
		typedef yaal::hcore::HHashMap<int, HHuginn::expression_t> expressions_t;
		typedef yaal::hcore::HHashMap<int, HHuginn::function_t> methods_t;
		yaal::hcore::HString _className;
		yaal::hcore::HString _baseName;
		field_names_t _fieldNames;
		expressions_t _fieldDefinitions;
		methods_t _methods;
		executing_parser::position_t _position;
		executing_parser::position_t _basePosition;
		OClassContext( void );
	};
	typedef yaal::hcore::HResource<OClassContext> class_context_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, class_context_t> submitted_classes_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, yaal::hcore::HString> submitted_imports_t;
	function_contexts_t _functionContexts;
	class_context_t _classContext;
	submitted_classes_t _submittedClasses;
	submitted_imports_t _submittedImports;
	yaal::hcore::HString _importName;
	yaal::hcore::HString _importAlias;
	HHuginn* _huginn;
	OCompiler( HHuginn* );
	OFunctionContext& f( void );
	void set_function_name( yaal::hcore::HString const&, executing_parser::position_t );
	void set_import_name( yaal::hcore::HString const&, executing_parser::position_t );
	void set_import_alias( yaal::hcore::HString const&, executing_parser::position_t );
	void set_class_name( yaal::hcore::HString const&, executing_parser::position_t );
	void set_base_name( yaal::hcore::HString const&, executing_parser::position_t );
	void set_field_name( yaal::hcore::HString const&, executing_parser::position_t );
	void add_field_name( yaal::hcore::HString const&, executing_parser::position_t );
	void set_lambda_name( executing_parser::position_t );
	HHuginn::function_t create_function( executing_parser::position_t );
	void commit_import( executing_parser::position_t );
	void submit_class( executing_parser::position_t );
	void create_lambda( executing_parser::position_t );
	void set_identifier( yaal::hcore::HString const&, executing_parser::position_t );
	void start_function_call( executing_parser::position_t );
	void close_function_call( executing_parser::position_t );
	void set_type_name( yaal::hcore::HString const&, executing_parser::position_t );
	void add_paramater( yaal::hcore::HString const&, executing_parser::position_t );
	void verify_default_argument( executing_parser::position_t );
	void track_name_cycle( yaal::hcore::HString const& );
	static bool is_numeric( HHuginn::type_id_t );
	static bool is_numeric_congruent( HHuginn::type_id_t );
	static bool is_summable( HHuginn::type_id_t );
	static bool is_comparable( HHuginn::type_id_t );
	static bool is_comparable_congruent( HHuginn::type_id_t );
	static bool is_boolean_congruent( HHuginn::type_id_t );
	static bool is_unknown( HHuginn::type_id_t );
	static bool is_reference_congruent( HHuginn::type_id_t );
	static bool is_integer_congruent( HHuginn::type_id_t );
	static bool are_congruous( HHuginn::type_id_t, HHuginn::type_id_t );
	static HHuginn::type_id_t congruent( HHuginn::type_id_t, HHuginn::type_id_t );
	HHuginn::scope_t& current_scope( void );
	HHuginn::expression_t& current_expression( void );
	OScopeContext& current_scope_context( void );
	HHuginn::type_id_t guess_type( yaal::hcore::HString const& );
	void note_type( yaal::hcore::HString const&, HHuginn::type_id_t );
	void reset_expression( void );
	void pop_function_context( void );
	void inc_loop_count( executing_parser::position_t );
	void inc_loop_switch_count( executing_parser::position_t );
	void start_subexpression( executing_parser::position_t );
	void add_subexpression( OPERATOR, executing_parser::position_t );
	void add_field_definition( executing_parser::position_t );
	void add_method( executing_parser::position_t );
	void commit_boolean( OPERATOR, executing_parser::position_t );
	void commit_ternary( executing_parser::position_t );
	void create_scope( executing_parser::position_t );
	void commit_scope( executing_parser::position_t );
	void commit_catch( executing_parser::position_t );
	void commit_if_clause( executing_parser::position_t );
	void commit_else_clause( executing_parser::position_t );
	void add_return_statement( executing_parser::position_t );
	void add_throw_statement( executing_parser::position_t );
	void add_break_statement( executing_parser::position_t );
	void add_continue_statement( executing_parser::position_t );
	void add_while_statement( executing_parser::position_t );
	void add_try_catch_statement( executing_parser::position_t );
	void add_for_statement( executing_parser::position_t );
	void add_if_statement( executing_parser::position_t );
	void add_switch_statement( executing_parser::position_t );
	void add_default_value( executing_parser::position_t );
	void commit_expression( executing_parser::position_t );
	void mark_expression_position( executing_parser::position_t );
	void make_reference( executing_parser::position_t );
	void defer_get_reference( yaal::hcore::HString const&, executing_parser::position_t );
	void defer_get_field_reference( yaal::hcore::HString const&, executing_parser::position_t );
	void defer_make_variable( yaal::hcore::HString const&, executing_parser::position_t );
	void defer_oper( char, executing_parser::position_t );
	void defer_str_oper( yaal::hcore::HString const&, executing_parser::position_t );
	void defer_oper_direct( OPERATOR, executing_parser::position_t );
	void dispatch_action( OPERATOR, executing_parser::position_t );
	void dispatch_plus( executing_parser::position_t );
	void dispatch_mul( executing_parser::position_t );
	void dispatch_power( executing_parser::position_t );
	void dispatch_compare( executing_parser::position_t );
	void dispatch_equals( executing_parser::position_t );
	void dispatch_boolean( expression_action_t const&, executing_parser::position_t );
	void dispatch_ternary( void );
	void dispatch_assign( executing_parser::position_t );
	void dispatch_subscript( executing_parser::position_t );
	void dispatch_function_call( expression_action_t const&, executing_parser::position_t );
	void dispatch_member_access( executing_parser::position_t );
	void defer_action( expression_action_t const&, executing_parser::position_t );
	void defer_store_direct( HHuginn::value_t const&, executing_parser::position_t );
	void defer_store_real( double long, executing_parser::position_t );
	void defer_store_integer( int long long, executing_parser::position_t );
	void defer_store_string( yaal::hcore::HString const&, executing_parser::position_t );
	void defer_store_number( yaal::hcore::HString const&, executing_parser::position_t );
	void defer_store_character( char, executing_parser::position_t );
	void defer_call( yaal::hcore::HString const&, executing_parser::position_t );
private:
	OCompiler( OCompiler const& ) = delete;
	OCompiler& operator = ( OCompiler const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED */

