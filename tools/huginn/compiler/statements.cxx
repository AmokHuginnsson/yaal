/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/huginn/compiler.hxx"
#include "tools/huginn/if.hxx"
#include "tools/huginn/for.hxx"
#include "tools/huginn/while.hxx"
#include "tools/huginn/switch.hxx"
#include "tools/huginn/return.hxx"
#include "tools/huginn/throw.hxx"
#include "tools/huginn/break.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

void OCompiler::create_scope( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::statement_id_t sid( INVALID_STATEMENT_IDENTIFIER );
	if ( ! fc._inline ) {
		++ _statementIdGenerator;
		sid = _statementIdGenerator;
	} else {
		sid = fc._scopeStack.top()->_statementId;
	}
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, sid, _fileId, range_ ) );
	fc._inline = false;
	return;
	M_EPILOG
}

void OCompiler::commit_scope( executing_parser::range_t ) {
	M_PROLOG
	M_DEBUG_CODE( OFunctionContext& fc( f() ); );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( scope );
	return;
	M_EPILOG
}

void OCompiler::start_if_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._scopeStack.top()->_scopeChain.is_empty() ) {
		fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, range_ ) );
	}
	fc._inline = true;
	return;
	M_EPILOG
}

void OCompiler::commit_if_clause( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	fc._scopeStack.top()->_scopeChain.emplace_back( scope, current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_else_clause( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._inline = true;
	return;
	M_EPILOG
}

void OCompiler::commit_else_clause( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	fc._scopeStack.top()->_else = scope;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_if_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext& sc( *fc._scopeStack.top() );
	HScope::statement_t ifStatement( make_pointer<HIf>( sc._statementId, sc._scopeChain, sc._else, sc._hasLocalVariables || sc._hasLocalVariablesInDirectChildren, _fileId, range_ ) );
	sc._scopeChain.clear();
	sc._else.reset();
	pop_scope_context_low();
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( ifStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_for_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._inline = true;
	++ fc._loopCount;
	++ fc._loopSwitchCount;
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, range_ ) );
	return;
	M_EPILOG
}

void OCompiler::add_for_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	OScopeContext& sc( *fc._scopeStack.top() );
	HHuginn::expressions_t& exprs( fc.expressions_stack().top() );
	HHuginn::expression_t source( exprs.back() );
	exprs.pop_back();
	HScope::statement_t forStatement(
		make_pointer<HFor>( sc._statementId, yaal::move( exprs ), source, scope, sc._needsFrame || sc._hasLocalVariables || sc._hasLocalVariablesInDirectChildren, _fileId, range_ )
	);
	pop_scope_context_low();
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( forStatement );
	-- fc._loopCount;
	-- fc._loopSwitchCount;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_while_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._inline = true;
	++ fc._loopCount;
	++ fc._loopSwitchCount;
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, range_ ) );
	return;
	M_EPILOG
}

void OCompiler::add_while_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	OScopeContext& sc( current_scope_context() );
	HScope::statement_t whileStatement(
		make_pointer<HWhile>( sc._statementId, current_expression(), scope, sc._needsFrame || sc._hasLocalVariables || sc._hasLocalVariablesInDirectChildren, _fileId, range_ )
	);
	pop_scope_context_low();
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( whileStatement );
	-- fc._loopCount;
	-- fc._loopSwitchCount;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_switch_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ fc._loopSwitchCount;
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, range_ ) );
	return;
	M_EPILOG
}

void OCompiler::add_switch_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext::active_scopes_t contexts( yaal::move( fc._scopeStack.top()->_scopeChain ) );
	HHuginn::scope_t Default( fc._scopeStack.top()->_else );
	pop_scope_context_low();
	OScopeContext& sc( *fc._scopeStack.top() );
	HScope::statement_t switchStatement(
		make_pointer<HSwitch>(
			sc._statementId,
			current_expression(),
			contexts,
			Default,
			_fileId,
			range_
		)
	);
	-- fc._loopSwitchCount;
	pop_scope_context_low();
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( switchStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_catch_statement( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, range_ ) );
	OScopeContext& sc( current_scope_context() );
	sc._exceptionType = _runtime->identifier_id( name_ );
	return;
	M_EPILOG
}

void OCompiler::commit_catch( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	OScopeContext& sc( *fc._scopeStack.top() );
	HTryCatch::HCatch c( sc._statementId, sc._exceptionType, current_expression(), scope );
	pop_scope_context_low();
	M_ASSERT( ! fc._scopeStack.is_empty() );
	fc._scopeStack.top()->_catches.emplace_back( c );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_try_catch_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HTryCatch::catches_t catches( yaal::move( fc._scopeStack.top()->_catches ) );
	HHuginn::scope_t scope( pop_scope_context() );
	HScope::statement_t tryCatchStatement(
		make_pointer<HTryCatch>( scope, catches, _fileId, range_ )
	);
	current_scope()->add_statement( tryCatchStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_return_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	HHuginn::expression_t& e( current_expression() );
	if ( e->is_empty() ) {
		e->add_execution_step( HExpression::OExecutionStep( e.raw(), &HExpression::store_direct, range_.start(), _runtime->none_value(), HHuginn::TYPE::NONE ) );
	}
	terminate_scope( make_pointer<HReturn>( e, _fileId, range_ ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_throw_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	terminate_scope( make_pointer<HThrow>( current_expression(), _fileId, range_ ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_break_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	if ( fc._loopSwitchCount == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid context for `break` statement.", _fileId, range_.start() );
	}
	OScopeContext& sc( *fc._scopeStack.top() );
	sc._needsFrame = true;
	terminate_scope( make_pointer<HBreak>( HThread::STATE::BREAK, _fileId, range_ ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_continue_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	if ( fc._loopCount == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid context for `continue` statement.", _fileId, range_.start() );
	}
	terminate_scope( make_pointer<HBreak>( HThread::STATE::CONTINUE, _fileId, range_ ) );
	reset_expression();
	return;
	M_EPILOG
}

}

}

}

