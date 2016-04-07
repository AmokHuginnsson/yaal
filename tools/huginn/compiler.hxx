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
#include "tools/huginn/scope.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HExpression;
class HFrame;

/*! \brief Huginn language compiler.
 */
struct OCompiler {
	typedef void ( HExpression::* expression_action_t ) ( HFrame*, int );
	typedef yaal::hcore::HStack<HFunction::expressions_t> expressions_stack_t;
	typedef yaal::hcore::HHashMap<HHuginn::identifier_id_t, HHuginn::type_id_t> variable_types_t;
	/*! \brief Saves _compiled_ `if'/`case' expression-scope pairs.
	 *
	 * Once created by compiler OActiveScope instance is never modified
	 * and only passed around.
	 *
	 * Term "active" comes from a main (entry) expression that is intrinsic for this kind of scope.
	 */
	struct OActiveScope {
		HHuginn::scope_t _scope;
		HHuginn::expression_t _expression;
		OActiveScope( HHuginn::scope_t&, HHuginn::expression_t& );
	};
	/*! \brief Huginn program scope as seen by the compiler.
	 */
	struct OScopeContext {
		typedef yaal::hcore::HArray<OActiveScope> active_scopes_t;
		OScopeContext* _parent;  /*!< parent scope */
		HHuginn::scope_t _scope; /*!< currently compiled Huginn run-time scope */

		/*! \brief Currently constructed expression.
		 *
		 * Expressions need to have form of a stack to support ternary operator
		 * and short-circuiting boolean operators.
		 *
		 * Expressions need to be held per scope to support `while' and `for'
		 * which commit their main (entry) expressions after their scope
		 * has been closed.
		 */
		expressions_stack_t _expressionsStack;

		/*! \brief Local symbol type book-keeper.
		 *
		 * Used for compile-time detection of type mismatches.
		 */
		variable_types_t _variableTypes;

		/*! \brief type of currently compiled exception catch clause
		 */
		HHuginn::identifier_id_t _exceptionType;

		/*! \brief identifier used either as `for' variable or caught exception variable
		 */
		HHuginn::identifier_id_t _identifier;
		int _position;

		/*! \brief Set of already fully compiled scopes of either if/else-if of switch->case..case chain.
		 */
		active_scopes_t _scopeChain;

		/*! \brief Already fully compiled scope for `else' in if/else-if/else chain.
		 * Also used as fully compiled `default' clause in `switch' statement.
		 */
		HHuginn::scope_t _else;

		/*! \brief Fully compiled chain of catch statements.
		 */
		HTryCatch::catches_t _catches;

		/*! \brief Tells what was statement count when `return/break/continue/throw' statement occurred.
		 */
		int _terminatedAt;

		/*! \brief Identificator of currently compiled statement.
		 */
		HStatement::statement_id_t _statementId;

		OScopeContext( OScopeContext*, HStatement::statement_id_t, int );
		HHuginn::expression_t& expression( void );
		HHuginn::type_id_t guess_type( HHuginn::identifier_id_t ) const;
		void note_type( HHuginn::identifier_id_t, HHuginn::type_id_t );
		void reset( HStatement::statement_id_t, int );
	private:
		OScopeContext( OScopeContext const& ) = delete;
		OScopeContext& operator = ( OScopeContext const& ) = delete;
	};
	/* \brief Compilation context for currently compiled function/method/lambda.
	 */
	struct OFunctionContext {
		typedef yaal::hcore::HPointer<OScopeContext> scope_context_t;
		typedef yaal::hcore::HStack<scope_context_t> scope_stack_t;
		typedef yaal::hcore::HStack<HHuginn::type_id_t> type_stack_t;
		typedef yaal::hcore::HStack<HHuginn::identifier_id_t> variable_stack_t;

		/*! \brief Identifier of currently compiled function.
		 */
		HHuginn::identifier_id_t _functionIdentifier;

		/*! \param Names of parameters for currently compiled function.
		 */
		HFunction::parameter_names_t _parameters;

		/*! \brief Default values for parameters of currently compiled function.
		 */
		HFunction::expressions_t _defaultValues;

		/*! \brief Index of default value for function argument that was defined last.
		 */
		int _lastDefaultValuePosition;

		/*! \brief Currently compiled scope stack.
		 */
		scope_stack_t _scopeStack;

		/*! \brief Expression execution emulator helper (operators).
		 *
		 * Keeps track of operators to be executed in currently compiled expression.
		 * This must be kept in \e OFunctionContext (as opposite to \e OScopeContext) to support
		 * expressions with directly called lambdas, e.g:
		 * \code
		 * x = 1 + @(){return ( 2 );}();
		 * \endcode
		 */
		operations_t _operations;

		/*! \brief Expression execution emulator helper (value types).
		 *
		 * Complementary of \e _operations.
		 * Helps with detection of mismatched types in expressions using built-in operators.
		 * Must be held in \e OFunctionContext for the same reason as \e _operations.
		 */
		type_stack_t _valueTypes;

		/*! \brief Remember what variables are used in (re)assignment sub-expression.
		 *
		 * Those remembered variable identifiers are used in dispatch_assign()
		 * to link types to variables in current compilation context.
		 */
		variable_stack_t _variables;

		/*! \brief Keep track how deep we are in nested for/while statements.
		 *
		 * Helps in detection on invalid use of `break' and `continue' statement.
		 */
		int _loopCount;

		/*! \brief Keep track how deep we are in nested for/while/switch statements.
		 *
		 * Helps in detection on invalid use of `break' statement.
		 */
		int _loopSwitchCount;

		/*! \brief Count how many nested calls we have at current stage of compilation of an expression.
		 *
		 * This count is used to support `assert' statement.
		 *
		 * By nested calls we mean:
		 * \code
		 * foo( foo( foo( foo( 0 ) ) ) )
		 * \codeend
		 */
		int _nestedCalls;

		/*! \brief Keeps information about type of last dereference operators used in currently compiled expression.
		 *
		 * Used to prevent assignment to function call result.
		 */
		OPERATOR _lastDereferenceOperator;

		/*! \brief Tell if currently compiled expression is in fact in assert statement.
		 *
		 * Used to support `assert' statement, also
		 * helps with detection of invalid uses of `assert' statement.
		 */
		bool _isAssert;

		/*! \brief Keeps information of last used member access name in currently compiled expression.
		 *
		 * Allows support for assignment to referenced object members:
		 * \code
		 * x._field = 0;
		 * \endcode
		 */
		HHuginn::identifier_id_t _lastMemberName;

		/*! \brief Tell if this function context refers to lambda.
		 */
		bool _isLambda;

		OFunctionContext( HStatement::statement_id_t, bool );
		expressions_stack_t& expressions_stack( void );
	};
	typedef yaal::hcore::HResource<OFunctionContext> function_context_t;
	typedef yaal::hcore::HStack<function_context_t> function_contexts_t;
	struct OClassContext {
		typedef yaal::hcore::HArray<yaal::hcore::HString> field_names_t;
		typedef yaal::hcore::HHashMap<int, HHuginn::expression_t> expressions_t;
		typedef yaal::hcore::HHashMap<int, HHuginn::function_t> methods_t;
		HHuginn::identifier_id_t _classIdentifier;
		HHuginn::identifier_id_t _baseName;
		field_names_t _fieldNames;
		expressions_t _fieldDefinitions;
		methods_t _methods;
		executing_parser::position_t _position;
		executing_parser::position_t _basePosition;
		OClassContext( void );
	};
	struct OExecutionStep {
		enum class OPERATION {
			DEFINE,
			USE
		};
		OPERATION _operation;
		HHuginn::expression_t _expression;
		OFunctionContext::scope_context_t _scope;

		/*! \brief Class identifier if this scope if part of class method.
		 */
		HHuginn::identifier_id_t _classId;
		int _index;
		HHuginn::identifier_id_t _identifier;
		int _position;
		OExecutionStep(
			OPERATION, HHuginn::expression_t const&, OFunctionContext::scope_context_t const&, HHuginn::identifier_id_t, int, HHuginn::identifier_id_t, int
		);
	};
	struct OIdentifierUse {
		enum class TYPE {
			UNKNOWN,
			CLASS,
			FIELD,
			METHOD,
			VARIABLE,
			FUNCTION,
			PACKAGE
		};
		TYPE _type;
		int _readCount;
		int _readPosition;
		int _writeCount;
		int _writePosition;
		OIdentifierUse( void );
		void read( int, TYPE = TYPE::UNKNOWN );
		void write( int, TYPE );
	};
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, OIdentifierUse> used_identifiers_t;
	typedef yaal::hcore::HArray<OExecutionStep> execution_steps_backlog_t;
	typedef yaal::hcore::HResource<OClassContext> class_context_t;
	typedef yaal::hcore::HHashMap<HHuginn::identifier_id_t, class_context_t> submitted_classes_t;
	typedef yaal::hcore::HHashMap<HHuginn::identifier_id_t, HHuginn::identifier_id_t> submitted_imports_t;
	function_contexts_t _functionContexts;
	class_context_t _classContext;
	submitted_classes_t _submittedClasses;
	submitted_imports_t _submittedImports;
	HHuginn::identifier_id_t _importIdentifier;
	HHuginn::identifier_id_t _importAlias;
	execution_steps_backlog_t _executionStepsBacklog;
	used_identifiers_t _usedIdentifiers;
	HHuginn::compiler_setup_t _setup;
	HStatement::statement_id_t _statementIdGenerator;
	HHuginn* _huginn;
	OCompiler( HHuginn* );
	OFunctionContext& f( void );
	void set_setup( HHuginn::compiler_setup_t );
	void detect_misuse( void ) const;
	void optimize( void );
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

	/*! \brief Verify that there are no holes in default argument definitions in function parameters.
	 *
	 * This function is called both explicitly by OCompiler
	 * and implicitly by executing_parser (from grammar).
	 */
	void verify_default_argument( executing_parser::position_t );
	void track_name_cycle( HHuginn::identifier_id_t );
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
	HHuginn::type_id_t guess_type( HHuginn::identifier_id_t );
	void note_type( HHuginn::identifier_id_t, HHuginn::type_id_t );
	void reset_expression( void );
	void pop_function_context( void );
	HHuginn::scope_t pop_scope_context( void );
	void terminate_scope( HScope::statement_t&& );
	void start_if_statement( executing_parser::position_t );
	void inc_loop_count( executing_parser::position_t );
	void start_switch_statement( executing_parser::position_t );
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
	void dispatch_factorial( executing_parser::position_t );
	void dispatch_negate( executing_parser::position_t );
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

extern HHuginn::identifier_id_t const INVALID_IDENTIFIER;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED */

