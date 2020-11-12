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

void OCompiler::create_scope_impl( bool incremental_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ _statementIdGenerator;
	HHuginn::scope_t scope;
	if ( incremental_ ) {
		scope = make_pointer<HIncrementalScope>( _statementIdGenerator, _fileId, range_ );
	}
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, range_, scope ) );
	return;
	M_EPILOG
}

void OCompiler::create_scope( executing_parser::range_t range_ ) {
	M_PROLOG
	create_scope_impl( false, range_ );
	return;
	M_EPILOG
}

void OCompiler::commit_scope( executing_parser::range_t ) {
	M_PROLOG
	M_DEBUG_CODE( OFunctionContext& fc( f() ); );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope_context().add_statement( scope );
	return;
	M_EPILOG
}

void OCompiler::start_if_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._scopeStack.top()->_scopeChain.is_empty() ) {
		++ _statementIdGenerator;
		HHuginn::scope_t ifScope( make_pointer<HIf>( _statementIdGenerator, _fileId, range_ ) );
		fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, range_, ifScope ) );
	}
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

void OCompiler::commit_else_clause( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	fc._scopeStack.top()->_auxScope = scope;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_if_statement( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext& sc( *fc._scopeStack.top() );
	OScopeContext::active_scopes_t scopeChain( yaal::move( sc._scopeChain ) );
	HHuginn::scope_t auxScope( yaal::move( sc._auxScope ) );
	HScope::statement_t ifStatement( pop_scope_context() );
	M_ASSERT( dynamic_cast<HIf*>( ifStatement.raw() ) );
	static_cast<HIf*>( ifStatement.raw() )->init( yaal::move( scopeChain ), yaal::move( auxScope ) );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope_context().add_statement( yaal::move( ifStatement ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_for_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ fc._loopCount;
	++ fc._loopSwitchCount;
	++ _statementIdGenerator;
	HHuginn::scope_t forScope( make_pointer<HFor>( _statementIdGenerator, _fileId, range_ ) );
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, range_, forScope ) );
	return;
	M_EPILOG
}

void OCompiler::add_for_statement( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	HHuginn::expressions_t& exprs( fc.expressions_stack().top() );
	HHuginn::expression_t source( exprs.back() );
	exprs.pop_back();
	HScope::statement_t forStatement( pop_scope_context() );
	M_ASSERT( dynamic_cast<HFor*>( forStatement.raw() ) );
	static_cast<HFor*>( forStatement.raw() )->init( yaal::move( exprs ), source, scope );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope_context().add_statement( yaal::move( forStatement ) );
	-- fc._loopCount;
	-- fc._loopSwitchCount;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::start_while_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ fc._loopCount;
	++ fc._loopSwitchCount;
	++ _statementIdGenerator;
	HHuginn::scope_t whileScope( make_pointer<HWhile>( _statementIdGenerator, _fileId, range_ ) );
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, range_, whileScope ) );
	return;
	M_EPILOG
}

void OCompiler::add_while_statement( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	HHuginn::expression_t expr( current_expression() );
	HScope::statement_t whileStatement( pop_scope_context() );
	M_ASSERT( dynamic_cast<HWhile*>( whileStatement.raw() ) );
	static_cast<HWhile*>( whileStatement.raw() )->init( expr, scope );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope_context().add_statement( yaal::move( whileStatement ) );
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
	++ _statementIdGenerator;
	HHuginn::scope_t switchScope( make_pointer<HSwitch>( _statementIdGenerator, _fileId, range_ ) );
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, range_, switchScope ) );
	return;
	M_EPILOG
}

void OCompiler::add_switch_statement( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext& sc( *fc._scopeStack.top() );
	OScopeContext::active_scopes_t contexts( yaal::move( sc._scopeChain ) );
	HHuginn::scope_t Default( sc._auxScope );
	pop_scope_context_low();
	HHuginn::expression_t expr( current_expression() );
	-- fc._loopSwitchCount;
	HScope::statement_t switchStatement( pop_scope_context() );
	M_ASSERT( dynamic_cast<HSwitch*>( switchStatement.raw() ) );
	static_cast<HSwitch*>( switchStatement.raw() )->init( expr, contexts, Default );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope_context().add_statement( yaal::move( switchStatement ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::commit_try( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext& sc( *fc._scopeStack.top() );
	sc._auxScope = scope;
	return;
	M_EPILOG
}

void OCompiler::start_catch_statement( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ _statementIdGenerator;
	HHuginn::scope_t catchScope( make_pointer<HTryCatch::HCatch>( _statementIdGenerator, _runtime->identifier_id( name_ ), _fileId, range_ ) );
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, range_, catchScope ) );
	return;
	M_EPILOG
}

void OCompiler::commit_catch( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	HHuginn::expression_t expr( current_expression() );
	HScope::statement_t catchStatement( pop_scope_context() );
	M_ASSERT( dynamic_cast<HTryCatch::HCatch*>( catchStatement.raw() ) );
	static_cast<HTryCatch::HCatch*>( catchStatement.raw() )->init( expr, scope );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	fc._scopeStack.top()->_catches.emplace_back( catchStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_try_catch_statement( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext& sc( *fc._scopeStack.top() );
	HTryCatch::catches_t catches( yaal::move( sc._catches ) );
	HScope::statement_t tryCatchStatement(
		make_pointer<HTryCatch>( yaal::move( sc._auxScope ), yaal::move( catches ), _fileId, range_ )
	);
	current_scope_context().add_statement( yaal::move( tryCatchStatement ) );
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

