/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hstack.hxx"
#include "tools/huginn/operator.hxx"
#include "tools/huginn/function.hxx"
#include "tools/huginn/functionreference.hxx"
#include "tools/huginn/frame.hxx"
#include "tools/huginn/trycatch.hxx"
#include "tools/huginn/scope.hxx"
#include "tools/huginn/expression.hxx"
#include "tools/huginn/keyword.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HExpression;

/*! \brief Huginn language compiler.
 */
struct OCompiler {
	static int const BOUND_INDEX = -1;
	static int const IMPLICIT_UNBOUND_INDEX = HExpression::UNBOUND_INDEX_BASE;
	typedef yaal::hcore::HStack<HFunction::expressions_t> expressions_stack_t;
	typedef yaal::hcore::HHashMap<HHuginn::identifier_id_t, huginn::HClass const*> variable_types_t;
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
	struct OFunctionContext;
	/*! \brief Huginn program scope as seen by the compiler.
	 */
	struct OScopeContext {
		typedef yaal::hcore::HArray<int> argument_indexes_t;
		typedef yaal::hcore::HStack<argument_indexes_t> nested_unbound_indexes_t;
		typedef yaal::hcore::HArray<OActiveScope> active_scopes_t;
		struct OLocalVariable {
			int _index;
			HExpression const* _definedBy;
			OLocalVariable( void )
				: _index( -1 )
				, _definedBy( nullptr ) {
				return;
			}
		};
		typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, OLocalVariable> local_variables_t;
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

		/*! \brief Type of currently compiled exception catch clause.
		 */
		HHuginn::identifier_id_t _exceptionType;

		/*! \brief Source code position where currently evaluated `assert' expression ends.
		 */
		int _assertExpressionEnd;

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

		/*! \brief Identifier of currently compiled statement.
		 */
		HStatement::statement_id_t _statementId;

		/*! \brief Context of a function this scope belongs to.
		 */
		HHuginn::identifier_id_t _functionId;

		/*! \brief Variable name to index map for variables defined in this scope.
		 *
		 * Used during local symbols dereferencing stage (in `resolve_symbols()`).
		 */
		local_variables_t _variables;

		/*! \brief Note indexes of unbound arguments in function calls.
		 *
		 * foo( a, ~, b )
		 * foo( a, ~2, b, ~1 )
		 */
		nested_unbound_indexes_t _argumentIndexes;

		/*! \brief Tells if given scope needs to have a fully materialized frame.
		 *
		 * Reasons for necessity of frame materialization:
		 *
		 * - existance of a local variable in direct children
		 * - break/continue statement in any of the children
		 */
		bool _needsFrame;

		/*! \brief Tells if given scope has any local variables defined.
		 *
		 * Used during statements/expressions compilation, i.e. before local
		 * symbols dereferencing stage (before `resolve_symbols()`)
		 */
		bool _hasLocalVariables;

		/*! \brief Tells if direct children of a given scope has any local variables defined.
		 *
		 * Used during `HIf` object construction.
		 */
		bool _hasLocalVariablesInDirectChildren;

		OScopeContext( OFunctionContext*, HStatement::statement_id_t, int, executing_parser::range_t );
		HHuginn::expression_t& expression( void );
		huginn::HClass const* guess_type( OCompiler const*, HHuginn::identifier_id_t ) const;
		void note_type( HHuginn::identifier_id_t, huginn::HClass const* );
	private:
		OScopeContext( OScopeContext const& ) = delete;
		OScopeContext& operator = ( OScopeContext const& ) = delete;
	};
	/* \brief Compilation context for currently compiled function/method/lambda.
	 */
	struct OFunctionContext {
		typedef yaal::hcore::HPointer<OScopeContext> scope_context_t;
		typedef yaal::hcore::HStack<scope_context_t> scope_stack_t;
		struct OValueDesc {
			huginn::HClass const* _class;
			HHuginn::identifier_id_t _identifier;
			OValueDesc( huginn::HClass const* class_ )
				: _class( class_ )
				, _identifier( IDENTIFIER::INVALID ) {
				return;
			}
			OValueDesc( huginn::HClass const* class_, HHuginn::identifier_id_t identifierId_ )
				: _class( class_ )
				, _identifier( identifierId_ ) {
				return;
			}
			OValueDesc( OValueDesc const& ) = default;
			OValueDesc& operator = ( OValueDesc const& ) = default;
		};
		typedef yaal::hcore::HStack<OValueDesc> type_stack_t;
		struct OVariableRef {
			HHuginn::identifier_id_t _identifier;
			int _executionStepIndex;
			OVariableRef( HHuginn::identifier_id_t identifier_, int index_ )
				: _identifier( identifier_ )
				, _executionStepIndex( index_ ) {
				return;
			}
		};
		typedef yaal::hcore::HStack<OVariableRef> variable_stack_t;
		typedef yaal::hcore::HStack<int> variable_count_t;

		/*! \brief Identifier of currently compiled function.
		 */
		HHuginn::identifier_id_t _functionIdentifier;

		/*! \param Names of parameters for currently compiled function.
		 */
		HFunction::parameter_names_t _parameters;

		/*! \param Names of captures for currently compiled lambda function.
		 */
		HFunction::parameter_names_t _captures;

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

		/*! \brief Number of variables on left side of the assignment (`=` sign).
		 *
		 * Used to support tuple unpacking.
		 */
		variable_count_t _variableCount;

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


		/*! \brief Count how many nested short-circuitable sub-expressions we have.
		 *
		 * Helps with preventing definition of variables in paths that could never be executed.
		 */
		int _shortCircuit;

		/*! \brief Count how many nested calls we have at current stage of compilation of an expression.
		 *
		 * This count is used to support `assert' statement.
		 *
		 * By nested calls we mean:
		 * \code
		 * foo( foo( foo( foo( 0 ) ) ) )
		 * \endcode
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

		/*! \brief Tell if this function context refers to lambda.
		 */
		bool _isLambda;

		/*! \brief Tell if this function is of a variadic kind.
		 */
		bool _isVariadic;

		/*! \brief Tell if this function captures non-positional named parameters.
		 */
		bool _capturesNamedParameters;

		/*! \brief Tell if next scope created with {...} should be inlined.
		 */
		bool _inline;

		/*! \brief Compiler reference.
		 */
		OCompiler* _compiler;

		OFunctionContext( OCompiler*, HHuginn::identifier_id_t, HStatement::statement_id_t, int, bool );
		expressions_stack_t& expressions_stack( void );
	private:
		OFunctionContext( OFunctionContext const& ) = delete;
		OFunctionContext& operator = ( OFunctionContext const& ) = delete;
	};
	typedef yaal::hcore::HResource<OFunctionContext> function_context_t;
	typedef yaal::hcore::HStack<function_context_t> function_contexts_t;
	struct OClassContext final {
		typedef yaal::hcore::HArray<yaal::hcore::HString> field_names_t;
		typedef yaal::hcore::HHashMap<int, HHuginn::expression_t> expressions_t;
		typedef yaal::hcore::HHashMap<int, HHuginn::function_t> methods_t;
		typedef yaal::hcore::HHashMap<int, yaal::hcore::HString> docs_t;
		HHuginn::identifier_id_t _classIdentifier;
		HHuginn::identifier_id_t _baseName;
		field_names_t _fieldNames;
		expressions_t _fieldDefinitions;
		methods_t _methods;
		docs_t _docs;
		int _position;
		int _basePosition;
		yaal::hcore::HString const* _doc;
		OClassContext( void );
	private:
		OClassContext( OClassContext const& ) = delete;
		OClassContext& operator = ( OClassContext const& ) = delete;
	};
	struct OExecutionStep {
		enum class OPERATION {
			DEFINE,
			USE,
			UPDATE
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
		bool _shortCircuit;
		OExecutionStep(
			OPERATION, HHuginn::expression_t const&, OFunctionContext::scope_context_t const&, HHuginn::identifier_id_t, int, HHuginn::identifier_id_t, int, bool = false
		);
	};
	struct OIdentifierUse {
		HHuginn::SYMBOL_KIND _type;
		int _readCount;
		int _readPosition;
		int _writeCount;
		int _writePosition;
		OIdentifierUse( void );
		void read( int, HHuginn::SYMBOL_KIND = HHuginn::SYMBOL_KIND::UNKNOWN );
		void write( int, HHuginn::SYMBOL_KIND );
	};
	/*! \brief Class used to gather information about all user defined classes prior to real compilation.
	 */
	struct OClassNoter {
		OCompiler* _compiler;
		/*! \brief Compilation stage.
		 *
		 * false - just note class names.
		 * true - actual compilation, initialize class definition creation.
		 */
		bool _passThrough;
		/*! \brief Either note user defined class name or forward call to OCompiler::set_class_name.
		 *
		 * Depending on compilation stage.
		 */
		void note( yaal::hcore::HString const&, executing_parser::range_t );
		OClassNoter( OCompiler* );
	private:
		OClassNoter( OClassNoter const& ) = delete;
		OClassNoter& operator = ( OClassNoter const& ) = delete;
	};
	struct OImportInfo {
		HHuginn::identifier_id_t _package;
		HHuginn::identifier_id_t _alias;
		HHuginn::identifiers_t _importedSymbols;
		int _position;
		OImportInfo( void );
		OImportInfo( OImportInfo&& );
		void swap( OImportInfo& );
		void reset( void );
	private:
		OImportInfo& operator = ( OImportInfo&& ) = delete;
	};
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, HFunction::parameter_names_t> captures_log_t;
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, OIdentifierUse> used_identifiers_t;
	typedef yaal::hcore::HArray<OExecutionStep> execution_steps_backlog_t;
	typedef yaal::hcore::HArray<HHuginn::identifier_id_t> class_identifiers_t;
	typedef yaal::hcore::HResource<OClassContext> class_context_t;
	typedef yaal::hcore::HHashMap<HHuginn::identifier_id_t, class_context_t> submitted_classes_t;
	typedef yaal::hcore::HHashMap<HHuginn::identifier_id_t, HHuginn::identifier_id_t> lambda_definition_sites_t;
	typedef yaal::hcore::HArray<OImportInfo> submitted_imports_t;
	typedef HHuginn::values_t submitted_enums_t;
	typedef yaal::hcore::HArray<OFunctionContext::scope_context_t> scope_context_cache_t;
	typedef yaal::hcore::HPair<HHuginn::identifier_id_t, HHuginn::function_t> function_info_t;
	function_contexts_t _functionContexts;
	class_context_t _classContext;
	class_identifiers_t _classIdentifiers;
	/*! \brief Class noter instance.
	 *
	 * Allow learning names of user defined classes prior to real compilation.
	 * Thanks to the fact that it can be bound directly to executing_parser callback
	 * it allows the diversification of compilation passes by callback id.
	 */
	OClassNoter _classNoter;
	submitted_classes_t _submittedClasses;
	submitted_enums_t _submittedEnums;
	submitted_imports_t _submittedImports;
	yaal::hcore::HString _moduleName;
	OImportInfo _importInfo;
	execution_steps_backlog_t _executionStepsBacklog;
	used_identifiers_t _usedIdentifiers;
	captures_log_t _capturesLog;
	lambda_definition_sites_t _lambdaDefinitionSites;
	HIntrospectorInterface* _introspector;
	HStatement::statement_id_t _statementIdGenerator;
	scope_context_cache_t _scopeContextCache;
	/*! \brief Tells if this compiler instance is used for sub-module.
	 */
	bool _isModule;
	/*! \brief Tells if currently loaded module was already loaded by different import path.
	 */
	bool _preloaded;
	/*! \brief Number of currently compiled source file, used for sub-modules.
	 */
	int _fileId;
	/*! \brief Tells if this compiler instance should work in `incremental' mode.
	 *
	 * In incremental mode redefinitions override original definitions.
	 */
	bool _isIncremental;
	/*! \brief Statement count in successfully executed previous incarnation of `main()' loop.
	 */
	int _mainExecutedStatementCount;
	/*! \brief Statement count in successfully compiled previous incarnation of `main()' loop.
	 */
	int _mainCompiledStatementCount;
	HRuntime* _runtime;
	OCompiler( HRuntime* );
	void reset( int );
	OFunctionContext& f( void );
	OFunctionContext const& f( void ) const;
	void set_setup( HIntrospectorInterface*, bool );
	void detect_misuse( void ) const;
	void resolve_symbols( void );
	void set_function_name( yaal::hcore::HString const&, executing_parser::range_t );
	void build_import_name( yaal::hcore::HString const&, executing_parser::range_t );
	void set_import_name( executing_parser::range_t );
	void set_import_alias( yaal::hcore::HString const&, executing_parser::range_t );
	void add_imported_symbol( yaal::hcore::HString const&, executing_parser::range_t );
	void set_class_name( HHuginn::identifier_id_t, executing_parser::range_t );
	void check_name_import( HHuginn::identifier_id_t, executing_parser::range_t );
	void check_name_class( HHuginn::identifier_id_t, bool, executing_parser::range_t );
	void check_name_function( HHuginn::identifier_id_t, executing_parser::range_t );
	void check_name_enum( HHuginn::identifier_id_t, bool, executing_parser::range_t );
	void set_enum_name( yaal::hcore::HString const&, executing_parser::range_t );
	void set_base_name( yaal::hcore::HString const&, executing_parser::range_t );
	void set_field_name( yaal::hcore::HString const&, executing_parser::range_t );
	void add_field_name( yaal::hcore::HString const&, executing_parser::range_t );
	void set_lambda_name( executing_parser::range_t );
	void mark_variadic( void );
	void mark_named_parameter_capture( void );
	void create_function( executing_parser::range_t );
	function_info_t create_function_low( executing_parser::range_t );
	void commit_import( executing_parser::range_t );
	void submit_class( executing_parser::range_t );
	void commit_enum( executing_parser::range_t );
	bool commit_unbound( HExpression::OExecutionStep::action_t const&, executing_parser::range_t );
	void create_lambda( executing_parser::range_t );
	void commit_assignable( executing_parser::range_t );
	void save_control_variable( executing_parser::range_t );
	void commit_catch_control_variable( executing_parser::range_t );
	void start_function_call( executing_parser::range_t );
	void close_function_call( executing_parser::range_t );
	void set_type_name( yaal::hcore::HString const&, executing_parser::range_t );
	void add_parameter( yaal::hcore::HString const&, executing_parser::range_t );
	void add_capture( yaal::hcore::HString const&, executing_parser::range_t );
	void commit_capture( executing_parser::range_t );

	/*! \brief Verify that there are no holes in default argument definitions in function parameters.
	 *
	 * This function is called implicitly by executing_parser (from grammar).
	 */
	void verify_default_argument( executing_parser::range_t );
	void track_name_cycle( HHuginn::identifier_id_t );
	huginn::HClass const* congruent( huginn::HClass const*, huginn::HClass const* ) const;
	HHuginn::scope_t& current_scope( void );
	HHuginn::expression_t& current_expression( void );
	OScopeContext& current_scope_context( void );
	huginn::HClass const* guess_type( HHuginn::identifier_id_t ) const;
	huginn::HClass const* type_id_to_class( HHuginn::type_id_t ) const;
	huginn::HClass const* type_to_class( HHuginn::TYPE ) const;
	void note_type( HHuginn::identifier_id_t, huginn::HClass const* );
	void reset_expression( void );
	void pop_function_context( void );
	HHuginn::scope_t pop_scope_context( void );
	void pop_scope_context_low( void );
	void terminate_scope( HScope::statement_t&& );
	void start_if_statement( executing_parser::range_t );
	void start_else_clause( executing_parser::range_t );
	void start_loop_statement( executing_parser::range_t );
	void start_switch_statement( executing_parser::range_t );
	void start_subexpression( executing_parser::range_t );
	void start_assignable( executing_parser::range_t );
	void finish_assignable( executing_parser::range_t );
	void add_subexpression( OPERATOR, executing_parser::range_t );
	void add_field_definition( executing_parser::range_t );
	void commit_boolean( OPERATOR, executing_parser::range_t );
	void commit_ternary( executing_parser::range_t );
	void create_scope( executing_parser::range_t );
	void commit_scope( executing_parser::range_t );
	void commit_catch( executing_parser::range_t );
	void commit_if_clause( executing_parser::range_t );
	void commit_else_clause( executing_parser::range_t );
	void add_return_statement( executing_parser::range_t );
	void add_throw_statement( executing_parser::range_t );
	void add_break_statement( executing_parser::range_t );
	void add_continue_statement( executing_parser::range_t );
	void add_while_statement( executing_parser::range_t );
	void add_try_catch_statement( executing_parser::range_t );
	void add_for_statement( executing_parser::range_t );
	void add_if_statement( executing_parser::range_t );
	void add_switch_statement( executing_parser::range_t );
	void add_default_value( executing_parser::range_t );
	void commit_expression( executing_parser::range_t );
	void mark_expression_position( executing_parser::range_t );
	void make_reference( executing_parser::range_t );
	void repack_named_parameters( executing_parser::range_t );
	void unpack_variadic_parameters( executing_parser::range_t );
	void note_function_argument( int, executing_parser::range_t );
	void index_unbound( int, executing_parser::range_t );
	void defer_get_reference( yaal::hcore::HString const&, executing_parser::range_t );
	void defer_get_field_reference( yaal::hcore::HString const&, executing_parser::range_t );
	void defer_make_variable( yaal::hcore::HString const&, executing_parser::range_t );
	void defer_oper( code_point_t, executing_parser::range_t );
	void defer_str_oper( yaal::hcore::HString const&, executing_parser::range_t );
	void defer_oper_direct( OPERATOR, executing_parser::range_t );
	void dispatch_action( OPERATOR, executing_parser::range_t );
	void dispatch_plus( executing_parser::range_t );
	void dispatch_mul( executing_parser::range_t );
	void dispatch_power( executing_parser::range_t );
	void dispatch_factorial( executing_parser::range_t );
	void dispatch_negate( executing_parser::range_t );
	void dispatch_compare( executing_parser::range_t );
	void dispatch_equals( executing_parser::range_t );
	void dispatch_boolean( HExpression::OExecutionStep::action_t const&, executing_parser::range_t );
	void dispatch_ternary( void );
	void dispatch_assign( executing_parser::range_t );
	void dispatch_subscript( executing_parser::range_t );
	void dispatch_function_call( HExpression::OExecutionStep::action_t const&, executing_parser::range_t );
	void dispatch_member_access( executing_parser::range_t );
	void defer_action( HExpression::OExecutionStep::action_t const&, executing_parser::range_t );
	void defer_function_call( HExpression::OExecutionStep::action_t, executing_parser::range_t );
	void defer_store_direct( HHuginn::value_t const&, executing_parser::range_t );
	void defer_store_real( double long, executing_parser::range_t );
	void defer_store_integer( int long long, executing_parser::range_t );
	void defer_store_string( yaal::hcore::HString const&, executing_parser::range_t );
	void defer_store_number( yaal::hcore::HNumber const&, executing_parser::range_t );
	void defer_store_character( code_point_t, executing_parser::range_t );
	void defer_call( yaal::hcore::HString const&, executing_parser::range_t );
	HHuginn::expression_t new_expression( int, executing_parser::range_t const& = { 0, 0 } );
	void merge( HHuginn::expression_t&, HHuginn::expression_t& );
private:
	HHuginn::identifier_id_t prep_import_result( yaal::hcore::HString const&, HHuginn::SYMBOL_KIND, executing_parser::range_t );
private:
	huginn::HClass const* function_ref_to_class( HHuginn::identifier_id_t );
	OCompiler( OCompiler const& ) = delete;
	OCompiler& operator = ( OCompiler const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_COMPILER_HXX_INCLUDED */

