/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  compiler.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "compiler.hxx"
#include "function.hxx"
#include "scope.hxx"
#include "expression.hxx"
#include "if.hxx"
#include "switch.hxx"
#include "booleanevaluator.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

void operands_type_mismatch( char const*, HHuginn::type_t, HHuginn::type_t, int ) __attribute__(( noreturn ));

OCompiler::OScopeContext::OScopeContext( HHuginn* huginn_ )
	: _huginn( huginn_ ),
	_scope( make_pointer<HScope>() ),
	_expressionsStack() {
	_expressionsStack.emplace( 1, make_pointer<HExpression>() );
	return;
}

OCompiler::OScopeContext::OScopeContext( HHuginn* huginn_, HHuginn::scope_t const& scope_, HHuginn::expression_t const& expression_ )
	: _huginn( huginn_ ),
	_scope( scope_ ),
	_expressionsStack() {
	_expressionsStack.emplace( 1, expression_ );
	return;
}

void OCompiler::OScopeContext::clear( void ) {
	M_PROLOG
	_scope = make_pointer<HScope>();
	_expressionsStack.clear();
	_expressionsStack.emplace( 1, make_pointer<HExpression>() );
	return;
	M_EPILOG
}

HHuginn::HHuginn::expression_t const& OCompiler::OScopeContext::expression( void ) const {
	return ( _expressionsStack.top().back() );
}

HHuginn::HHuginn::expression_t& OCompiler::OScopeContext::expression( void ) {
	return ( _expressionsStack.top().back() );
}

OCompiler::OCompilationFrame::OCompilationFrame( HHuginn* huginn_ )
	: _context( huginn_ ),
	_contextsChain(),
	_else(),
	_forIdentifier(),
	_forPosition( 0 ) {
	return;
}

void OCompiler::OCompilationFrame::clear( void ) {
	M_PROLOG
	_contextsChain.clear();
	_else.reset();
	_forIdentifier.clear();
	_forPosition = 0;
	return;
	M_EPILOG
}

OCompiler::OFunctionContext::OFunctionContext( HHuginn* huginn_ )
	: _functionName(),
	_parameters(),
	_defaultValues(),
	_lastDefaultValuePosition( -1 ),
	_compilationStack(),
	_operations(),
	_valueTypes(),
	_lastDereferenceOperator( OPERATOR::NONE ) {
	_compilationStack.emplace( huginn_ );
	return;
}

OCompiler::OCompiler( HHuginn* huginn_ )
	: _functionContexts(),
	_huginn( huginn_ ) {
	return;
}

OCompiler::OFunctionContext& OCompiler::f( void ) {
	return ( _functionContexts.top() );
}

HHuginn::HHuginn::scope_t& OCompiler::current_scope( void ) {
	return ( f()._compilationStack.top()._context._scope );
}

HHuginn::HHuginn::expression_t& OCompiler::current_expression( void ) {
	return ( f()._compilationStack.top()._context.expression() );
}

void OCompiler::set_function_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	_functionContexts.emplace( _huginn );
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted keyword." ), position_.get() );
	}
	f()._functionName = name_;
	return;
	M_EPILOG
}

void OCompiler::set_lambda_name( executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::HErrorCoordinate ec( _huginn->get_coordinate( position_.get() ) );
	_functionContexts.emplace( _huginn );
	f()._functionName.assign( "@" ).append( ec.line() ).append( ":" ).append( ec.column() );
	return;
	M_EPILOG
}

HHuginn::function_t OCompiler::create_function( executing_parser::position_t ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	M_ASSERT( ! fc._functionName.is_empty() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HHuginn::function_t fun(
		hcore::call(
			&HFunction::execute,
			make_pointer<HFunction>(
				fc._functionName,
				fc._parameters,
				yaal::move( current_scope() ),
				fc._defaultValues
			),
			_1, _2, _3
		)
	);
	fc._compilationStack.pop();
	M_ASSERT( fc._compilationStack.get_size() == 1 );
	fc._compilationStack.top().clear();
	fc._parameters.clear();
	fc._defaultValues.clear();
	fc._lastDefaultValuePosition = -1;
	return ( fun );
	M_EPILOG
}

void OCompiler::create_lambda( executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::function_t fun( create_function( position_ ) );
	HHuginn::value_t fRef( make_pointer<HHuginn::HFunctionReference>( f()._functionName, fun ) );
	_functionContexts.pop();
	defer_store_direct( fRef, position_ );
	return;
	M_EPILOG
}

void OCompiler::verify_default_argument( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int lastDefaultValuePosition( fc._lastDefaultValuePosition );
	if ( ( lastDefaultValuePosition >= 0 ) && ( lastDefaultValuePosition < static_cast<int>( fc._parameters.get_size() - 1 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( "Missing default argument.", position_.get() );
	}
	return;
	M_EPILOG
}

void OCompiler::add_paramater( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	verify_default_argument( position_ );
	f()._parameters.push_back( name_ );
	return;
	M_EPILOG
}

void OCompiler::create_scope( executing_parser::position_t ) {
	M_PROLOG
	f()._compilationStack.emplace( _huginn );
	return;
	M_EPILOG
}

void OCompiler::commit_scope( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HHuginn::scope_t scope( current_scope() );
	fc._compilationStack.pop();
	M_ASSERT( ! fc._compilationStack.is_empty() );
	current_scope()->add_statement( scope );
	return;
	M_EPILOG
}

void OCompiler::reset_expression( void ) {
	M_PROLOG
	current_expression() = make_pointer<HExpression>();
	OFunctionContext& fc( f() );
	M_ASSERT( fc._operations.is_empty() );
	fc._valueTypes.clear();
	return;
	M_EPILOG
}

void OCompiler::start_subexpression( executing_parser::position_t position_ ) {
	M_PROLOG
	f()._compilationStack.top()._context._expressionsStack.emplace( 1, make_pointer<HExpression>( position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::add_subexpression( OPERATOR op_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._compilationStack.top()._context._expressionsStack.top().emplace_back( make_pointer<HExpression>( position_.get() ) );
	fc._operations.emplace( op_, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::commit_boolean( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._compilationStack.top()._context._expressionsStack.top().get_size() > 1 ) {
		HHuginn::value_t And( make_pointer<HBooleanEvaluator>( fc._compilationStack.top()._context._expressionsStack.top(), operator_ ) );
		fc._compilationStack.top()._context._expressionsStack.pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top() == HHuginn::TYPE::BOOLEAN );
		fc._valueTypes.pop();
		defer_store_direct( And, position_ );
		current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), operator_, _1, position_.get() ) );
		defer_action( operator_ == OPERATOR::BOOLEAN_AND ? &HExpression::boolean_and : &HExpression::boolean_or, position_ );
	} else {
		HHuginn::expression_t e( fc._compilationStack.top()._context.expression() );
		fc._compilationStack.top()._context._expressionsStack.pop();
		fc._compilationStack.top()._context.expression()->merge( *e );
	}
	return;
	M_EPILOG
}

void OCompiler::commit_ternary( executing_parser::position_t position_ ) {
	OFunctionContext& fc( f() );
	HFunction::expressions_t const& exprs( fc._compilationStack.top()._context._expressionsStack.top() );
	if ( exprs.get_size() > 1 ) {
		M_ASSERT( exprs.get_size() == 3 );
		HHuginn::value_t ternary( make_pointer<HHuginn::HTernaryEvaluator>( exprs[0], exprs[1], exprs[2] ) );
		fc._compilationStack.top()._context._expressionsStack.pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top() == HHuginn::TYPE::UNKNOWN );
		fc._valueTypes.pop();
		defer_store_direct( ternary, position_ );
		current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), OPERATOR::TERNARY, _1, position_.get() ) );
		defer_action( &HExpression::ternary, position_ );
	} else {
		HHuginn::expression_t e( fc._compilationStack.top()._context.expression() );
		fc._compilationStack.top()._context._expressionsStack.pop();
		fc._compilationStack.top()._context.expression()->merge( *e );
	}
}

void OCompiler::add_return_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_scope()->add_statement( make_pointer<HHuginn::HReturn>( current_expression() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_break_statement( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_scope()->add_statement( make_pointer<HHuginn::HBreak>() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_while_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HHuginn::scope_t scope( current_scope() );
	fc._compilationStack.pop();
	current_scope()->add_statement( make_pointer<HHuginn::HWhile>( current_expression(), scope ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::set_for_identifier( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	OCompilationFrame& cf( f()._compilationStack.top() );
	cf._forIdentifier = name_;
	cf._forPosition = position_.get();
	return;
	M_EPILOG
}

void OCompiler::add_for_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HHuginn::scope_t scope( current_scope() );
	fc._compilationStack.pop();
	OCompilationFrame const& cf( fc._compilationStack.top() );
	current_scope()->add_statement( make_pointer<HHuginn::HFor>( cf._forIdentifier, current_expression(), scope, cf._forPosition ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::commit_if_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HHuginn::scope_t scope( current_scope() );
	fc._compilationStack.pop();
	fc._compilationStack.top()._contextsChain.emplace_back( _huginn, scope, current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::commit_else_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HHuginn::scope_t scope( current_scope() );
	fc._compilationStack.pop();
	fc._compilationStack.top()._else = scope;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_if_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	HScope::statement_t ifStatement( make_pointer<HIf>( fc._compilationStack.top()._contextsChain, fc._compilationStack.top()._else ) );
	fc._compilationStack.top()._contextsChain.clear();
	fc._compilationStack.top()._else.reset();
	current_scope()->add_statement( ifStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_switch_statement( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	OCompilationFrame::contexts_t contexts( yaal::move( fc._compilationStack.top()._contextsChain ) );
	HHuginn::scope_t Default( fc._compilationStack.top()._else );
	fc._compilationStack.pop();
	HScope::statement_t switchStatement(
		make_pointer<HSwitch>(
			current_expression(),
			contexts,
			Default
		)
	);
	current_scope()->add_statement( switchStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::commit_expression( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_scope()->add_statement( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_default_value( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._compilationStack.is_empty() );
	fc._lastDefaultValuePosition = static_cast<int>( fc._parameters.get_size() - 1 );
	fc._defaultValues.push_back( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::mark_expression_position( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._compilationStack.is_empty() );
	current_expression()->set_position( position_.get() );
	return;
	M_EPILOG
}

void OCompiler::defer_oper( char operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OPERATOR o( OPERATOR::NONE );
	switch ( operator_ ) {
		case ( '+' ): o = OPERATOR::PLUS;        break;
		case ( '-' ): o = OPERATOR::MINUS;       break;
		case ( '*' ): o = OPERATOR::MULTIPLY;    break;
		case ( '/' ): o = OPERATOR::DIVIDE;      break;
		case ( '%' ): o = OPERATOR::MODULO;      break;
		case ( '^' ): o = OPERATOR::POWER;       break;
		case ( '(' ): o = OPERATOR::PARENTHESIS; break;
		case ( '|' ): o = OPERATOR::ABSOLUTE;    break;
		case ( '=' ): o = OPERATOR::ASSIGN;      break;
		case ( '!' ): o = OPERATOR::BOOLEAN_NOT; break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), o, _1, position_.get() ) );
	f()._operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::defer_str_oper( yaal::hcore::HString const& operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, OPERATOR> operator_lookup_t;
	static operator_lookup_t const operatorLookup( {
		{ "==", OPERATOR::EQUALS },
		{ "!=", OPERATOR::NOT_EQUALS },
		{ "<=", OPERATOR::LESS_OR_EQUAL },
		{ ">=", OPERATOR::GREATER_OR_EQUAL },
		{ "&&", OPERATOR::BOOLEAN_AND },
		{ "||", OPERATOR::BOOLEAN_OR },
		{ "^^", OPERATOR::BOOLEAN_XOR },
		{ "<",  OPERATOR::LESS },
		{ ">",  OPERATOR::GREATER }
	} );
	OPERATOR o( operatorLookup.at( operator_ ) );
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), o, _1, position_.get() ) );
	f()._operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::defer_oper_direct( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::oper, current_expression().raw(), operator_, _1, position_.get() ) );
	f()._operations.emplace( operator_, position_.get() );
	return;
	M_EPILOG
}

bool OCompiler::is_numeric( HHuginn::type_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::INTEGER ) || ( type_ == HHuginn::TYPE::REAL ) || ( type_ == HHuginn::TYPE::NUMBER ) );
}

bool OCompiler::is_comparable( HHuginn::type_t type_ ) {
	return (
		is_numeric( type_ )
		|| ( type_ == HHuginn::TYPE::STRING )
		|| ( type_ == HHuginn::TYPE::CHARACTER )
	);
}

bool OCompiler::is_boolean_congruent( HHuginn::type_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::BOOLEAN ) || ( type_ == HHuginn::TYPE::UNKNOWN ) || ( type_ == HHuginn::TYPE::REFERENCE ) );
}

bool OCompiler::is_unknown( HHuginn::type_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::NOT_BOOLEAN ) || ( type_ == HHuginn::TYPE::UNKNOWN ) || ( type_ == HHuginn::TYPE::REFERENCE ) );
}

bool OCompiler::is_numeric_congruent( HHuginn::type_t type_ ) {
	return ( is_numeric( type_ ) || is_unknown( type_ ) );
}

bool OCompiler::is_comparable_congruent( HHuginn::type_t type_ ) {
	return ( is_comparable( type_ ) || is_unknown( type_ ) );
}

bool OCompiler::is_reference_congruent( HHuginn::type_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::REFERENCE ) || ( type_ == HHuginn::TYPE::UNKNOWN ) );
}

bool OCompiler::is_integer_congruent( HHuginn::type_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::INTEGER ) || is_unknown( type_ ) );
}

bool OCompiler::is_summable( HHuginn::type_t type_ ) {
	return ( is_numeric_congruent( type_ ) || ( type_ == HHuginn::TYPE::STRING ) );
}

bool OCompiler::are_congruous( HHuginn::type_t t1_, HHuginn::type_t t2_ ) {
	bool congruous(
		( t1_ == t2_ )
		|| ( t1_ == HHuginn::TYPE::UNKNOWN )
		|| ( t2_ == HHuginn::TYPE::UNKNOWN )
		|| ( t1_ == HHuginn::TYPE::REFERENCE )
		|| ( t2_ == HHuginn::TYPE::REFERENCE )
	);
	if ( ! congruous ) {
		congruous = ( ( t1_ != HHuginn::TYPE::BOOLEAN ) && ( t2_ == HHuginn::TYPE::NOT_BOOLEAN ) )
			|| ( ( t2_ != HHuginn::TYPE::BOOLEAN ) && ( t1_ == HHuginn::TYPE::NOT_BOOLEAN ) );
	}
	return ( congruous );
}

HHuginn::HHuginn::type_t OCompiler::congruent( HHuginn::type_t t1_, HHuginn::type_t t2_ ) {
	HHuginn::type_t t( HHuginn::TYPE::NOT_BOOLEAN );
	if ( t1_ == t2_ ) {
		if ( ( t1_ != HHuginn::TYPE::UNKNOWN ) && ( t1_ != HHuginn::TYPE::REFERENCE ) ) {
			t = t1_;
		}
	} else if ( ! ( is_unknown( t1_ ) && is_unknown( t2_ ) ) ) {
		if ( is_unknown( t1_ ) ) {
			t = t2_;
		} else {
			t = t1_;
		}
	}
	return ( t );
}

void OCompiler::dispatch_plus( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::PLUS ) || ( o == OPERATOR::MINUS ) );
	defer_action( o == OPERATOR::PLUS ? &HExpression::plus : &HExpression::minus, position_ );
	fc._operations.pop();
	HHuginn::type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_summable( t1 ) && is_summable( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( o == OPERATOR::PLUS ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB], p );
	}
	fc._valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_mul( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::MULTIPLY ) || ( o == OPERATOR::DIVIDE ) || ( o == OPERATOR::MODULO ) );
	defer_action( o == OPERATOR::MULTIPLY ? &HExpression::mul : ( o == OPERATOR::DIVIDE ? &HExpression::div : &HExpression::mod ), position_ );
	fc._operations.pop();
	HHuginn::type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( o == OPERATOR::MULTIPLY ? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV], p );
	}
	fc._valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_power( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	defer_action( &HExpression::power, position_ );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::POWER ) ) {
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		HHuginn::type_t t1( fc._valueTypes.top() );
		fc._valueTypes.pop();
		HHuginn::type_t t2( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( ! ( are_congruous( t1, t2 ) && is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
			throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP], p );
		}
		fc._valueTypes.push( congruent( t1, t2 ) );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_compare( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::LESS ) || ( o == OPERATOR::GREATER ) || ( o == OPERATOR::LESS_OR_EQUAL ) || ( o == OPERATOR::GREATER_OR_EQUAL ) );
	switch ( o ) {
		case ( OPERATOR::LESS ):             { defer_action( &HExpression::less, position_ );             } break;
		case ( OPERATOR::GREATER ):          { defer_action( &HExpression::greater, position_ );          } break;
		case ( OPERATOR::LESS_OR_EQUAL ):    { defer_action( &HExpression::less_or_equal, position_ );    } break;
		case ( OPERATOR::GREATER_OR_EQUAL ): { defer_action( &HExpression::greater_or_equal, position_ ); } break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HHuginn::type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( are_congruous( t1, t2 ) && is_comparable_congruent( t1 ) && is_comparable_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_CMP], p );
	}
	fc._valueTypes.push( HHuginn::TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void OCompiler::dispatch_boolean( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	int p( po._position );
	if ( !! action_ ) {
		defer_action( action_, position_ );
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HHuginn::type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( is_boolean_congruent( t1 ) && is_boolean_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], p );
	}
	fc._valueTypes.push( HHuginn::TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void OCompiler::dispatch_ternary( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( fc._operations.top()._operator == OPERATOR::TERNARY );
	fc._operations.pop();
	M_ASSERT( fc._operations.top()._operator == OPERATOR::TERNARY );
	int p( fc._operations.top()._position );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 3 );
	fc._valueTypes.pop();
	fc._valueTypes.pop();
	HHuginn::type_t t0( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! is_boolean_congruent( t0 ) ) {
		throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
	}
	fc._valueTypes.push( HHuginn::TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void OCompiler::dispatch_equals( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::EQUALS ) || ( o == OPERATOR::NOT_EQUALS ) );
	defer_action( o == OPERATOR::EQUALS ? &HExpression::equals : &HExpression::not_equals, position_ );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HHuginn::type_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( ( o == OPERATOR::EQUALS ? "==" : "!=" ), t1, t2, p );
	}
	fc._valueTypes.push( HHuginn::TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void OCompiler::dispatch_assign( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	defer_action( &HExpression::set_variable, position_ );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::ASSIGN ) ) {
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		HHuginn::type_t t1( fc._valueTypes.top() );
		fc._valueTypes.pop();
		HHuginn::type_t t2( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( ! is_reference_congruent( t2 ) ) {
			throw HHuginn::HHuginnRuntimeException( "Setting a non reference location.", p );
		}
		fc._valueTypes.push( t1 );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_subscript( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPERATOR op( fc._operations.top()._operator );
	int range( 0 );
	bool nonInteger( false );
	int p( position_.get() );
	while ( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
		if ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) {
			++ range;
		} else {
			if ( ! ( nonInteger || is_integer_congruent( fc._valueTypes.top() ) || ( fc._valueTypes.top() == HHuginn::TYPE::NONE ) ) ) {
				nonInteger = true;
				p = fc._operations.top()._position;
			}
			fc._valueTypes.pop();
		}
		fc._operations.pop();
		op = fc._operations.top()._operator;
	}
	if ( ( range > 0 ) && nonInteger ) {
		throw HHuginn::HHuginnRuntimeException( "Range specifier is not an integer.", p );
	}
	M_ASSERT( fc._operations.top()._operator == OPERATOR::SUBSCRIPT );
	current_expression()->add_execution_step( hcore::call( &HExpression::subscript, current_expression().raw(), HExpression::SUBSCRIPT::VALUE, _1, position_.get() ) );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 1 );
	fc._valueTypes.pop();
	fc._valueTypes.push( HHuginn::TYPE::REFERENCE );
	fc._lastDereferenceOperator = OPERATOR::SUBSCRIPT;
	return;
	M_EPILOG
}

void OCompiler::dispatch_function_call( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) ) {
		fc._operations.pop();
		fc._valueTypes.pop();
	}
	if ( action_ == &HExpression::function_call ) {
		fc._valueTypes.pop();
	}
	fc._valueTypes.push( HHuginn::TYPE::UNKNOWN );
	M_ASSERT( fc._operations.top()._operator == OPERATOR::FUNCTION_CALL );
	defer_action( action_, position_ );
	fc._operations.pop();
	return;
	M_EPILOG
}

void OCompiler::dispatch_action( OPERATOR oper_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._lastDereferenceOperator = OPERATOR::NONE;
	OPositionedOperator po( ! fc._operations.is_empty() ? fc._operations.top() : OPositionedOperator( OPERATOR::NONE, 0 ) );
	OPERATOR o( po._operator );
	int p( po._position );
	switch ( oper_ ) {
		case ( OPERATOR::PLUS ):     { dispatch_plus( position_ );  } break;
		case ( OPERATOR::MULTIPLY ): { dispatch_mul( position_ );   } break;
		case ( OPERATOR::POWER ):    { dispatch_power( position_ ); } break;
		case ( OPERATOR::NEGATE ): {
			M_ASSERT( o == OPERATOR::NEGATE );
			M_ASSERT( ! fc._valueTypes.is_empty() );
			defer_action( &HExpression::negate, position_ );
			fc._operations.pop();
			if ( ! is_numeric_congruent( fc._valueTypes.top() ) ) {
				throw HHuginn::HHuginnRuntimeException( "Operand is not a numeric value.", p );
			}
		} break;
		case ( OPERATOR::SUBSCRIPT ):     { dispatch_subscript( position_ );     } break;
		case ( OPERATOR::ASSIGN ):        { dispatch_assign( position_ );        } break;
		case ( OPERATOR::FUNCTION_CALL ): { dispatch_function_call( &HExpression::function_call, position_ ); } break;
		case ( OPERATOR::MAKE_MAP ):      { dispatch_function_call( &HExpression::make_map, position_ ); } break;
		case ( OPERATOR::PARENTHESIS ):
		case ( OPERATOR::ABSOLUTE ): {
			M_ASSERT( ( o == OPERATOR::ABSOLUTE ) || ( o == OPERATOR::PARENTHESIS ) );
			defer_action( &HExpression::close_parenthesis, position_ );
			fc._operations.pop();
			if ( o == OPERATOR::ABSOLUTE ) {
				M_ASSERT( ! fc._valueTypes.is_empty() );
				if ( ! is_numeric_congruent( fc._valueTypes.top() ) ) {
					throw HHuginn::HHuginnRuntimeException( "Operand is not a numeric value.", p );
				}
			}
		} break;
		case ( OPERATOR::EQUALS ): { dispatch_equals( position_ );  } break;
		case ( OPERATOR::LESS ):   { dispatch_compare( position_ ); } break;
		case ( OPERATOR::BOOLEAN_AND ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_AND );
			dispatch_boolean( nullptr, position_ );
		} break;
		case ( OPERATOR::BOOLEAN_OR ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_OR );
			dispatch_boolean( nullptr, position_ );
		} break;
		case ( OPERATOR::BOOLEAN_XOR ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_XOR );
			dispatch_boolean( &HExpression::boolean_xor, position_ );
		} break;
		case ( OPERATOR::BOOLEAN_NOT ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_NOT );
			M_ASSERT( ! fc._valueTypes.is_empty() );
			defer_action( &HExpression::boolean_not, position_ );
			fc._operations.pop();
			if ( ! is_boolean_congruent( fc._valueTypes.top() ) ) {
				throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
			}
		} break;
		case ( OPERATOR::TERNARY ): { dispatch_ternary(); } break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	return;
	M_EPILOG
}

void OCompiler::defer_action( expression_action_t const& expressionAction_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( expressionAction_, current_expression().raw(), _1, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::defer_call( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	defer_get_reference( name_, position_ );
	defer_oper_direct( OPERATOR::FUNCTION_CALL, position_ );
	return;
	M_EPILOG
}

void OCompiler::make_reference( executing_parser::position_t position_ ) {
	M_PROLOG
	if ( f()._lastDereferenceOperator != OPERATOR::SUBSCRIPT ) {
		throw HHuginn::HHuginnRuntimeException( "Assignment to function result.", position_.get() );
	}
	current_expression()->pop_execution_step();
	current_expression()->add_execution_step( hcore::call( &HExpression::subscript, current_expression().raw(), HExpression::SUBSCRIPT::REFERENCE, _1, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::defer_get_reference( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::get_reference, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void OCompiler::defer_make_variable( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::make_variable, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::UNKNOWN );
	return;
	M_EPILOG
}

void OCompiler::defer_store_direct( HHuginn::value_t const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( value_->type() );
	return;
	M_EPILOG
}

void OCompiler::defer_store_real( double long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_real, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::REAL );
	return;
	M_EPILOG
}

void OCompiler::defer_store_integer( int long long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_integer, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::INTEGER );
	return;
	M_EPILOG
}

void OCompiler::defer_store_string( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_string, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::STRING );
	return;
	M_EPILOG
}

void OCompiler::defer_store_number( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_number, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::NUMBER );
	return;
	M_EPILOG
}

void OCompiler::defer_store_character( char value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_character, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::CHARACTER );
	return;
	M_EPILOG
}

void OCompiler::defer_store_boolean( bool value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_boolean, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::BOOLEAN );
	return;
	M_EPILOG
}

void OCompiler::defer_store_none( executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_none, current_expression().raw(), _1, position_.get() ) );
	f()._valueTypes.push( HHuginn::TYPE::NONE );
	return;
	M_EPILOG
}

void operands_type_mismatch( char const* op_, HHuginn::type_t t1_, HHuginn::type_t t2_, int pos_ ) {
	hcore::HString msg( "Operand types for `" );
	msg.append( op_ )
		.append( "' do not match: " )
		.append( t1_->name() )
		.append( " vs " )
		.append( t2_->name() )
		.append( "." ),
	throw HHuginn::HHuginnRuntimeException( msg, pos_ );
}

}

}

}

