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
#include "expression.hxx"
#include "introexpression.hxx"
#include "introfunction.hxx"
#include "if.hxx"
#include "for.hxx"
#include "while.hxx"
#include "switch.hxx"
#include "return.hxx"
#include "throw.hxx"
#include "break.hxx"
#include "booleanevaluator.hxx"
#include "helper.hxx"
#include "keyword.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HStatement::statement_id_t const INVALID_STATEMENT_IDENTIFIER( -1 );
static int const NOT_TERMINATED( -1 );

OCompiler::OActiveScope::OActiveScope( HHuginn::scope_t& scope_, HHuginn::expression_t& expression_ )
	: _scope( yaal::move( scope_ ) )
	, _expression( yaal::move( expression_ ) ) {
	return;
}

OCompiler::OScopeContext::OScopeContext(
	OFunctionContext* functionContext_,
	HStatement::statement_id_t statementId_,
	int fileId_,
	int position_
) : _parent( ! functionContext_->_scopeStack.is_empty() ? functionContext_->_scopeStack.top().raw() : nullptr )
	, _scope( make_pointer<HScope>( statementId_, fileId_, position_ ) )
	, _expressionsStack()
	, _variableTypes()
	, _exceptionType( INVALID_IDENTIFIER )
	, _assertExpressionEnd( 0 )
	, _scopeChain()
	, _else()
	, _catches()
	, _terminatedAt( NOT_TERMINATED )
	, _statementId( statementId_ )
	, _functionId( functionContext_->_functionIdentifier )
	, _variables() {
	_expressionsStack.emplace( 1, functionContext_->_compiler->new_expression( fileId_ ) );
	return;
}

HHuginn::HHuginn::expression_t& OCompiler::OScopeContext::expression( void ) {
	return ( _expressionsStack.top().back() );
}

HHuginn::type_id_t OCompiler::OScopeContext::guess_type( HHuginn::identifier_id_t identifierId_ ) const {
	M_PROLOG
	HHuginn::type_id_t t( type_id( HHuginn::TYPE::UNKNOWN ) );
	OScopeContext const* sc( this );
	while ( sc ) {
		variable_types_t::const_iterator it( sc->_variableTypes.find( identifierId_ ) );
		if ( it != sc->_variableTypes.end() ) {
			t = it->second;
			break;
		}
		sc = sc->_parent;
	}
	return ( t );
	M_EPILOG
}

void OCompiler::OScopeContext::note_type( HHuginn::identifier_id_t identifierId_, HHuginn::type_id_t type_ ) {
	M_PROLOG
	OScopeContext* sc( this );
	bool found( false );
	while ( sc ) {
		variable_types_t::iterator it( sc->_variableTypes.find( identifierId_ ) );
		if ( it != sc->_variableTypes.end() ) {
			sc->_variableTypes[identifierId_] = type_;
			found = true;
			break;
		}
		sc = sc->_parent;
	}
	if ( ! found ) {
		_variableTypes[identifierId_] = type_;
	}
	return;
	M_EPILOG
}

OCompiler::OFunctionContext::OFunctionContext(
	OCompiler* compiler_,
	HHuginn::identifier_id_t functionId_,
	HStatement::statement_id_t statementId_,
	int fileId_,
	bool isLambda_
) : _functionIdentifier( functionId_ )
	, _parameters()
	, _captures()
	, _defaultValues()
	, _lastDefaultValuePosition( -1 )
	, _scopeStack()
	, _operations()
	, _valueTypes()
	, _variables()
	, _loopCount( 0 )
	, _loopSwitchCount( 0 )
	, _nestedCalls( 0 )
	, _lastDereferenceOperator( OPERATOR::NONE )
	, _isAssert( false )
	, _lastMemberName()
	, _isLambda( isLambda_ )
	, _inline( true )
	, _compiler( compiler_ ) {
	_scopeStack.emplace( make_pointer<OScopeContext>( this, statementId_, fileId_, 0 ) );
	return;
}

OCompiler::expressions_stack_t& OCompiler::OFunctionContext::expressions_stack( void ) {
	M_PROLOG
	return ( _scopeStack.top()->_expressionsStack );
	M_EPILOG
}

OCompiler::OClassContext::OClassContext( void )
	: _classIdentifier( INVALID_IDENTIFIER )
	, _baseName( INVALID_IDENTIFIER )
	, _fieldNames()
	, _fieldDefinitions()
	, _methods()
	, _docs()
	, _position( 0 )
	, _basePosition( 0 )
	, _doc( nullptr ) {
	return;
}

OCompiler::OExecutionStep::OExecutionStep(
	OPERATION operation_,
	HHuginn::expression_t const& expression_,
	OFunctionContext::scope_context_t const& scope_,
	HHuginn::identifier_id_t classId_,
	int index_,
	HHuginn::identifier_id_t identifier_,
	int position_
) : _operation( operation_ )
	, _expression( expression_ )
	, _scope( scope_ )
	, _classId( classId_ )
	, _index( index_ )
	, _identifier( identifier_ )
	, _position( position_ ) {
	return;
}

OCompiler::OIdentifierUse::OIdentifierUse( void )
	: _type( TYPE::UNKNOWN )
	, _readCount( 0 )
	, _readPosition( 0 )
	, _writeCount( 0 )
	, _writePosition( 0 ) {
	return;
}

void OCompiler::OIdentifierUse::read( int position_, TYPE type_ ) {
	if ( _readCount == 0 ) {
		_type = type_;
		_readPosition = position_;
	}
	++ _readCount;
	return;
}

void OCompiler::OIdentifierUse::write( int position_, TYPE type_ ) {
	if ( _writeCount == 0 ) {
		_type = type_;
		_writePosition = position_;
	}
	++ _writeCount;
	return;
}

OCompiler::OClassNoter::OClassNoter( OCompiler* compiler_ )
	: _compiler( compiler_ )
	, _passThrough( false ) {
	return;
}

void OCompiler::OClassNoter::note( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::identifier_id_t identifier( _compiler->_runtime->identifier_id( name_ ) );
	if ( _passThrough ) {
		_compiler->set_class_name( identifier, position_ );
	} else {
		_compiler->_classIdentifiers.push_back( identifier );
	}
	return;
	M_EPILOG
}

OCompiler::OImportInfo::OImportInfo( void )
	: _package( INVALID_IDENTIFIER )
	, _alias( INVALID_IDENTIFIER )
	, _position( 0 ) {
	return;
}

OCompiler::OImportInfo::OImportInfo( OImportInfo&& other_ )
	: _package( INVALID_IDENTIFIER )
	, _alias( INVALID_IDENTIFIER )
	, _position( 0 ) {
	swap( other_ );
	return;
}

void OCompiler::OImportInfo::swap( OImportInfo& other_ ) {
	using yaal::swap;
	swap( _package, other_._package );
	swap( _alias, other_._alias );
	swap( _position, other_._position );
	return;
}

void OCompiler::OImportInfo::reset( void ) {
	_package = INVALID_IDENTIFIER;
	_alias = INVALID_IDENTIFIER;
	_position = 0;
	return;
}

OCompiler::OCompiler( HRuntime* runtime_ )
	: _functionContexts()
	, _classContext()
	, _classIdentifiers()
	, _classNoter( this )
	, _submittedClasses()
	, _submittedImports()
	, _importInfo()
	, _executionStepsBacklog()
	, _usedIdentifiers()
	, _capturesLog()
	, _setup( HHuginn::COMPILER::BE_STRICT )
	, _introspector( nullptr )
	, _statementIdGenerator( INVALID_STATEMENT_IDENTIFIER )
	, _scopeContextCache()
	, _isModule( false )
	, _fileId( INVALID_FILE_ID )
	, _isIncremental( false )
	, _mainStatementCount( 0 )
	, _runtime( runtime_ ) {
	return;
}

void OCompiler::reset( int undoSteps_ ) {
	M_PROLOG
	_fileId = INVALID_FILE_ID;
	_isModule = false;
	_mainStatementCount -= undoSteps_;
	_isIncremental = true;
	_scopeContextCache.clear();
	_statementIdGenerator = INVALID_STATEMENT_IDENTIFIER;
	_introspector = nullptr;
	_capturesLog.clear();
	_usedIdentifiers.clear();
	_executionStepsBacklog.clear();
	_importInfo.reset();
	_submittedImports.clear();
	_submittedClasses.clear();
	_classNoter._passThrough = false;
	_classIdentifiers.clear();
	_classContext.reset();
	_functionContexts.clear();
	return;
	M_EPILOG
}

void OCompiler::set_setup( HHuginn::compiler_setup_t setup_, HIntrospectorInterface* introspector_ ) {
	_setup = setup_;
	_introspector = introspector_;
	++ _fileId;
	return;
}

void OCompiler::resolve_symbols( void ) {
	M_PROLOG
	M_ASSERT( _submittedClasses.is_empty() );
	HHuginn::identifier_id_t lastClassId( INVALID_IDENTIFIER );
	HHuginn::class_t aClass;
	int maxLocalVariableCount( 0 );
	for ( OExecutionStep& es : _executionStepsBacklog ) {
		if ( es._classId != lastClassId ) {
			if ( es._classId != INVALID_IDENTIFIER ) {
				aClass = _runtime->get_class( es._classId );
				M_ASSERT( !! aClass );
			} else {
				aClass.reset();
			}
			lastClassId = es._classId;
		}
		if ( find_if( _submittedImports.begin(), _submittedImports.end(), [&es]( OImportInfo const& info_ ) { return ( info_._package == es._identifier ); } ) != _submittedImports.end() ) {
			throw HHuginn::HHuginnRuntimeException( "Making a direct reference to a package is forbidden.", MAIN_FILE_ID, es._position );
		}
		do {
			if ( !! aClass ) {
				int index( aClass->field_index( es._identifier ) );
				if ( index >= 0 ) {
					if ( ! es._expression ) {
						throw HHuginn::HHuginnRuntimeException(
							"Method argument name `"_ys
								.append( _runtime->identifier_name( es._identifier) )
								.append( "' conflicts with class `" )
								.append( _runtime->identifier_name( aClass->identifier_id() ) )
								.append( "' field name." ),
							MAIN_FILE_ID,
							es._position
						);
					}
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::get_field_direct,
							es._expression.raw(),
							es._operation == OExecutionStep::OPERATION::USE ? HExpression::ACCESS::VALUE : HExpression::ACCESS::REFERENCE,
							index,
							_1,
							es._position
						)
					);
					break;
				}
			}
			captures_log_t::const_iterator cli( _capturesLog.find( es._scope->_functionId ) );
			if ( cli != _capturesLog.end() ) {
				OFunctionContext::parameter_names_t::const_iterator ci( find( cli->second.begin(), cli->second.end(), es._identifier ) );
				if ( ci != cli->second.end() ) {
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::get_field_direct,
							es._expression.raw(),
							es._operation == OExecutionStep::OPERATION::USE ? HExpression::ACCESS::VALUE : HExpression::ACCESS::REFERENCE,
							static_cast<int>( distance( cli->second.begin(), ci ) ),
							_1,
							es._position
						)
					);
					break;
				}
			}
			OScopeContext* sc( es._scope.raw() );
			HHuginn::identifier_id_t fi( sc->_functionId );
			OScopeContext::OLocalVariable localVariable;
			while ( sc ) {
				OScopeContext::local_variables_t::const_iterator it( sc->_variables.find( es._identifier ) );
				if ( it != sc->_variables.end() ) {
					localVariable = it->second;
					break;
				}
				if ( sc->_parent && ( sc->_parent->_functionId == fi ) ) {
					sc = sc->_parent;
				} else {
					sc = nullptr;
				}
			}
			if ( es._operation == OExecutionStep::OPERATION::DEFINE ) {
				if ( ! sc ) {
					sc = es._scope.raw();
					/*
					 * There are two kinds of OScopeContexts:
					 * 1. Real scope context that is equivalent of {...} in program code.
					 * 2. Virtual scope context that is necessary for proper scoping of `for' and `while' and `if'
					 *    control variables e.g.:
					 *        for ( e : list ) { ... }
					 *    in this case `e' is in virtual scope.
					 * Finding indices of local variables must take into account both types of scopes
					 * because real scopes `{}' can be inlined in virtual scopes in run-time,
					 * like it happens in case of `if/else' statement.
					 * Inlining `if/else' real scopes has a side effect of allowing different
					 * variables from different scopes (different `else if' stanzas in the same `if/else' chain)
					 * to have the same index, this is possible because `[else] if' stanzas are mutually exclusive
					 * in given `if/else' chain.
					 */
					OScopeContext* parent( sc );
					while ( parent->_parent && ( parent->_parent->_statementId == parent->_statementId ) ) {
						parent = parent->_parent;
					}
					M_ASSERT( localVariable._index == -1 );
					localVariable._index = static_cast<int>( sc->_variables.get_size() );
					if ( parent != sc ) {
						localVariable._index += static_cast<int>( parent->_variables.get_size() );
					}
					localVariable._definedBy = es._expression.raw();
					sc->_variables[es._identifier] = localVariable;
					if ( ( localVariable._index + 1 ) > maxLocalVariableCount ) {
						maxLocalVariableCount = localVariable._index + 1;
					}
				}
				if ( !! es._expression ) {
					if ( _introspector ) {
						es._expression->replace_execution_step(
							es._index,
							hcore::call(
								&HIntroExpression::get_variable_direct_note,
								static_cast<HIntroExpression*>( es._expression.raw() ),
								HExpression::ACCESS::REFERENCE,
								sc->_statementId,
								localVariable._index,
								_1,
								es._identifier,
								es._position
							)
						);
					} else {
						es._expression->replace_execution_step(
							es._index,
							hcore::call(
								&HExpression::get_variable_direct,
								es._expression.raw(),
								HExpression::ACCESS::REFERENCE,
								sc->_statementId,
								localVariable._index,
								_1,
								es._position
							)
						);
					}
				} else {
					/*
					 * Function arguments would be handled here,
					 * but they are added explicitly from HFunction::execute().
					 */
				}
				break;
			} else {
				if ( sc ) {
					if ( es._expression.raw() == localVariable._definedBy ) {
						throw HHuginn::HHuginnRuntimeException(
							"Symbol `"_ys.append( _runtime->identifier_name( es._identifier ) ).append( "' is not yet defined in this expression." ),
							MAIN_FILE_ID,
							es._position
						);
					}
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::get_variable_direct,
							es._expression.raw(),
							es._operation == OExecutionStep::OPERATION::USE ? HExpression::ACCESS::VALUE : HExpression::ACCESS::REFERENCE,
							sc->_statementId,
							localVariable._index,
							_1,
							es._position
						)
					);
					break;
				}
			}
			if ( ( es._operation == OExecutionStep::OPERATION::USE ) && ! is_keyword( _runtime->identifier_name( es._identifier ) ) ) {
				HHuginn::value_t* callable( _runtime->get_function( es._identifier ) );
				if ( !! callable ) {
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::store_external_reference,
							es._expression.raw(),
							HHuginn::value_ref_t( *callable ),
							_1,
							es._position
						)
					);
					break;
				} else {
					HHuginn::value_t* p( _runtime->get_package( es._identifier ) );
					if ( !! p ) {
						es._expression->replace_execution_step(
							es._index,
							hcore::call(
								&HExpression::store_external_reference,
								es._expression.raw(),
								HHuginn::value_ref_t( *p ),
								_1,
								es._position
							)
						);
						break;
					}
				}
			}
			throw HHuginn::HHuginnRuntimeException(
				"Symbol `"_ys
					.append( _runtime->identifier_name( es._identifier ) )
					.append( "' is not defined in this context (did you mean `" )
					.append( _runtime->suggestion( es._identifier ) )
					.append( "'?)." ),
				MAIN_FILE_ID,
				es._position
			);
		} while ( false );
	}
	_runtime->set_max_local_variable_count( maxLocalVariableCount );
	_scopeContextCache.clear();
	_executionStepsBacklog.clear();
	return;
	M_EPILOG
}

namespace {
HString use_name( OCompiler::OIdentifierUse::TYPE type_ ) {
	HString name;
	switch ( type_ ) {
		case ( OCompiler::OIdentifierUse::TYPE::UNKNOWN ):  name = "Symbol";   break;
		case ( OCompiler::OIdentifierUse::TYPE::CLASS ):    name = "Class";    break;
		case ( OCompiler::OIdentifierUse::TYPE::FIELD ):    name = "Field";    break;
		case ( OCompiler::OIdentifierUse::TYPE::METHOD ):   name = "Method";   break;
		case ( OCompiler::OIdentifierUse::TYPE::FUNCTION ): name = "Function"; break;
		case ( OCompiler::OIdentifierUse::TYPE::VARIABLE ): name = "Variable"; break;
		case ( OCompiler::OIdentifierUse::TYPE::PACKAGE ):  name = "Package";  break;
	}
	return ( name );
}
}

void OCompiler::detect_misuse( void ) const {
	M_PROLOG
	HHuginn::identifier_id_t implicitUse[] = {
		STANDARD_FUNCTIONS::MAIN_IDENTIFIER,
		_runtime->identifier_id( INTERFACE::GET_SIZE ),
		_runtime->identifier_id( INTERFACE::ITERATOR ),
		_runtime->identifier_id( INTERFACE::IS_VALID ),
		_runtime->identifier_id( INTERFACE::NEXT ),
		_runtime->identifier_id( INTERFACE::VALUE ),
		_runtime->identifier_id( INTERFACE::HASH ),
		_runtime->identifier_id( INTERFACE::EQUALS ),
		_runtime->identifier_id( INTERFACE::LESS ),
		_runtime->identifier_id( INTERFACE::ADD ),
		_runtime->identifier_id( INTERFACE::SUBTRACT ),
		_runtime->identifier_id( INTERFACE::MULTIPLY ),
		_runtime->identifier_id( INTERFACE::DIVIDE ),
		_runtime->identifier_id( INTERFACE::MODULO ),
		_runtime->identifier_id( INTERFACE::POWER ),
		_runtime->identifier_id( "to_string" ),
		_runtime->identifier_id( "to_integer" ),
		_runtime->identifier_id( "to_real" ),
		_runtime->identifier_id( "to_number" ),
		_runtime->identifier_id( "to_character" ),
		_runtime->identifier_id( "to_boolean" )
	};
	for ( used_identifiers_t::value_type const& iu : _usedIdentifiers ) {
		HHuginn::identifier_id_t id( iu.first );
		OIdentifierUse const& use( iu.second );
		if ( use._readCount == 0 ) {
			if ( ( find( begin( implicitUse ), end( implicitUse ), id ) == end( implicitUse ) )
				&& ( ! _isModule
					|| ( ( use._type == OIdentifierUse::TYPE::VARIABLE )
					|| ( use._type == OIdentifierUse::TYPE::UNKNOWN ) )
				)
			) {
				throw HHuginn::HHuginnRuntimeException(
					use_name( use._type )
						.append( " `" )
						.append( _runtime->identifier_name( id ) )
						.append( "' is never used (did you mean `" )
						.append( _runtime->suggestion( id ) )
						.append( "'?)." ),
					MAIN_FILE_ID,
					use._writePosition
				);
			}
		}
	}
	return;
	M_EPILOG
}

OCompiler::OFunctionContext& OCompiler::f( void ) {
	return ( *_functionContexts.top() );
}

HHuginn::HHuginn::scope_t& OCompiler::current_scope( void ) {
	return ( f()._scopeStack.top()->_scope );
}

HHuginn::HHuginn::expression_t& OCompiler::current_expression( void ) {
	return ( f()._scopeStack.top()->expression() );
}

OCompiler::OScopeContext& OCompiler::current_scope_context( void ) {
	return ( *f()._scopeStack.top() );
}

HHuginn::expression_t OCompiler::new_expression( int fileId_, int position_ ) {
	return (
		_introspector
			? pointer_static_cast<HExpression>( make_pointer<HIntroExpression>( _introspector, fileId_, position_ ) )
			: make_pointer<HExpression>( fileId_, position_ )
	);
}

HHuginn::type_id_t OCompiler::guess_type( HHuginn::identifier_id_t identifierId_ ) {
	HHuginn::type_id_t t( f()._scopeStack.top()->guess_type( identifierId_ ) );
	if ( t == HHuginn::TYPE::UNKNOWN ) {
		if ( _runtime->get_function( identifierId_ ) || ( find( _classIdentifiers.begin(), _classIdentifiers.end(), identifierId_ ) != _classIdentifiers.end() ) ) {
			t = type_id( HHuginn::TYPE::FUNCTION_REFERENCE );
		}
	}
	return ( t );
}

void OCompiler::note_type( HHuginn::identifier_id_t identifierId_, HHuginn::type_id_t type_ ) {
	f()._scopeStack.top()->note_type( identifierId_, type_ );
}

void OCompiler::set_function_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::identifier_id_t functionIdentifier( _runtime->identifier_id( name_ ) );
	bool isCtorDtor( ( functionIdentifier == KEYWORD::CONSTRUCTOR_IDENTIFIER ) || ( functionIdentifier == KEYWORD::DESTRUCTOR_IDENTIFIER ) );
	if ( is_restricted( name_ ) ) {
		if ( ! _classContext || ! isCtorDtor ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), MAIN_FILE_ID, position_.get() );
		}
	}
	if ( ! _classContext ) {
		HHuginn::value_t* fun( _runtime->get_function( functionIdentifier ) );
		if ( fun && ! _isIncremental ) {
			throw HHuginn::HHuginnRuntimeException( "Function `"_ys.append( name_ ).append( "' was already defined." ), MAIN_FILE_ID, position_.get() );
		}
	}
	if ( ! isCtorDtor ) {
		_usedIdentifiers[functionIdentifier].write(
			position_.get(),
			! _classContext ? OIdentifierUse::TYPE::FUNCTION : OIdentifierUse::TYPE::METHOD
		);
	}
	_functionContexts.emplace( make_resource<OFunctionContext>( this, functionIdentifier, ++ _statementIdGenerator, _fileId, false ) );
	if ( !! _classContext ) {
		add_field_name( name_, position_ );
	}
	create_scope( position_ );
	return;
	M_EPILOG
}

void OCompiler::set_import_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::identifier_id_t importIdentifier( _runtime->identifier_id( name_ ) );
	if ( _submittedClasses.count( importIdentifier ) > 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Class `"_ys.append( name_ ).append( "' named is already defined." ), MAIN_FILE_ID, position_.get() );
	}
	if ( find_if( _submittedImports.begin(), _submittedImports.end(), [&importIdentifier]( OImportInfo const& info_ ) { return ( info_._package == importIdentifier ); } ) != _submittedImports.end() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Package `"_ys.append( _runtime->identifier_name( importIdentifier ) ).append( "' was already imported." ),
			MAIN_FILE_ID,
			position_.get()
		);
	}
	_importInfo._package = importIdentifier;
	_importInfo._position = position_.get();
	return;
	M_EPILOG
}

void OCompiler::set_import_alias( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::identifier_id_t importAliasIdentifier( _runtime->identifier_id( name_ ) );
	if ( _submittedClasses.count( importAliasIdentifier ) > 0 ) {
		throw HHuginn::HHuginnRuntimeException(
			"Class `"_ys.append( name_ ).append( "' named is already defined." ),
			MAIN_FILE_ID,
			position_.get()
		);
	}
	if ( find_if( _submittedImports.begin(), _submittedImports.end(), [&importAliasIdentifier]( OImportInfo const& info_ ) { return ( info_._alias == importAliasIdentifier ); } ) != _submittedImports.end() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Import alias `"_ys.append( _runtime->identifier_name( importAliasIdentifier ) ).append( "' is already defined." ),
			MAIN_FILE_ID,
			position_.get()
		);
	}
	_importInfo._alias = importAliasIdentifier;
	_usedIdentifiers[importAliasIdentifier].write( position_.get(), OIdentifierUse::TYPE::PACKAGE );
	return;
	M_EPILOG
}

void OCompiler::commit_import( executing_parser::position_t ) {
	M_PROLOG
	_submittedImports.emplace_back( yaal::move( _importInfo ) );
	return;
	M_EPILOG
}

void OCompiler::set_class_name( HHuginn::identifier_id_t identifier_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( ( _submittedClasses.count( identifier_ ) > 0 ) || ( !! _runtime->get_class( identifier_ ) && ! _isIncremental ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Class `"_ys.append( _runtime->identifier_name( identifier_ ) ).append( "' is already defined." ),
			MAIN_FILE_ID,
			position_.get()
		);
	}
	_classContext = make_resource<OClassContext>();
	_usedIdentifiers[identifier_].write( position_.get(), OIdentifierUse::TYPE::CLASS );
	_functionContexts.emplace( make_resource<OFunctionContext>( this, identifier_, ++ _statementIdGenerator, _fileId, false ) );
	_classContext->_classIdentifier = identifier_;
	_classContext->_position = position_;
	return;
	M_EPILOG
}

void OCompiler::set_base_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_builtin( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted keyword." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::identifier_id_t baseClassIdentifier( _runtime->identifier_id( name_ ) );
	_usedIdentifiers[baseClassIdentifier].read( position_.get() );
	_classContext->_baseName = baseClassIdentifier;
	_classContext->_basePosition = position_;
	return;
	M_EPILOG
}

void OCompiler::add_field_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( find( _classContext->_fieldNames.begin(), _classContext->_fieldNames.end(), name_ ) != _classContext->_fieldNames.end() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Field `"_ys.append( name_ ).append( "' is already defined in `" ).append( _runtime->identifier_name( _classContext->_classIdentifier ) ).append( "'." ),
			MAIN_FILE_ID,
			position_.get()
		);
	}
	_classContext->_fieldNames.push_back( name_ );
	return;
	M_EPILOG
}

void OCompiler::set_field_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::identifier_id_t fieldIdentifier( _runtime->identifier_id( name_ ) );
	_usedIdentifiers[fieldIdentifier].write( position_.get(), OIdentifierUse::TYPE::FIELD );
	add_field_name( name_, position_ );
	return;
	M_EPILOG
}

void OCompiler::set_lambda_name( executing_parser::position_t position_ ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	HHuginn::HErrorCoordinate ec( _runtime->huginn()->get_coordinate( MAIN_FILE_ID, position_.get() ) );
	using yaal::hcore::to_string;
	HHuginn::identifier_id_t id( _runtime->identifier_id( to_string( "@" ).append( ec.line() ).append( ":" ).append( ec.column() ) ) );
	_functionContexts.emplace( make_resource<OFunctionContext>( this, id, ++ _statementIdGenerator, _fileId, true ) );
	if ( ! fc._captures.is_empty() ) {
		_capturesLog.insert( make_pair( f()._functionIdentifier, yaal::move( fc._captures ) ) );
	}
	create_scope( position_ );
	return;
	M_EPILOG
}

void OCompiler::create_function( executing_parser::position_t position_ ) {
	M_PROLOG
	HString const& doc( _runtime->huginn()->get_comment( position_.get() ) );
	function_info_t fi( create_function_low( position_ ) );
	if ( !! _classContext ) {
		int idx( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ) );
		_classContext->_methods.insert( make_pair( idx, fi.second ) );
		_classContext->_docs.insert( make_pair( idx, doc ) );
	} else {
		if ( !! _runtime->get_class( fi.first ) ) {
			throw HHuginn::HHuginnRuntimeException( "Class of the same name `"_ys.append( _runtime->identifier_name( fi.first ) ).append( "' is already defined." ), MAIN_FILE_ID, position_.get() );
		}
		_runtime->register_function( fi.first, fi.second, doc );
	}
	M_EPILOG
}

OCompiler::function_info_t OCompiler::create_function_low( executing_parser::position_t ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	M_ASSERT( fc._functionIdentifier != INVALID_IDENTIFIER );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	bool isIncrementalMain( _isIncremental && ( fc._functionIdentifier == STANDARD_FUNCTIONS::MAIN_IDENTIFIER ) && ! _classContext );
	if ( isIncrementalMain ) {
		for ( int i( _mainStatementCount ); i > 0; -- i ) {
			scope->remove_statement( i - 1 );
		}
		_mainStatementCount += scope->statement_count();
	}
	HHuginn::function_t fun(
		_introspector
			? HHuginn::function_t(
				hcore::call(
					isIncrementalMain ? &HIntroFunction::execute_incremental_main : &HIntroFunction::execute,
					make_pointer<HIntroFunction>(
						fc._functionIdentifier,
						fc._parameters,
						scope,
						fc._defaultValues
					),
					_1, _2, _3, _4
				)
			)
			: HHuginn::function_t(
				hcore::call(
					isIncrementalMain ? &HFunction::execute_incremental_main : &HFunction::execute,
					make_pointer<HFunction>(
						fc._functionIdentifier,
						static_cast<int>( fc._parameters.get_size() ),
						scope,
						fc._defaultValues
					),
					_1, _2, _3, _4
				)
			)
	);
	M_ASSERT( fc._scopeStack.get_size() == 1 );
	function_info_t fi( fc._functionIdentifier, fun );
	pop_function_context();
	return ( fi );
	M_EPILOG
}

void OCompiler::submit_class( executing_parser::position_t position_ ) {
	M_PROLOG
	pop_function_context();
	_classContext->_doc = &_runtime->huginn()->get_comment( position_.get() );
	bool inserted( _submittedClasses.insert( make_pair( _classContext->_classIdentifier, yaal::move( _classContext ) ) ).second );
	M_ASSERT( inserted );
	static_cast<void>( inserted );
	return;
	M_EPILOG
}

void OCompiler::track_name_cycle( HHuginn::identifier_id_t identifierId_ ) {
	M_PROLOG
	typedef yaal::hcore::HHashSet<HHuginn::identifier_id_t> names_t;
	typedef yaal::hcore::HArray<HHuginn::identifier_id_t> hierarchy_t;
	names_t names;
	hierarchy_t hierarchy;
	OClassContext const* cc( _submittedClasses.at( identifierId_ ).get() );
	names.insert( identifierId_ );
	while ( cc->_baseName != INVALID_IDENTIFIER ) {
		submitted_classes_t::const_iterator it( _submittedClasses.find( cc->_baseName ) );
		if ( it == _submittedClasses.end() ) {
			throw HHuginn::HHuginnRuntimeException(
				"Base class `"_ys.append( _runtime->identifier_name( cc->_baseName ) ).append( "' was not defined." ),
				MAIN_FILE_ID,
				cc->_basePosition.get()
			);
		}
		hierarchy.push_back( cc->_baseName );
		if ( ! names.insert( cc->_baseName ).second ) {
			hcore::HString hier( _runtime->identifier_name( identifierId_ ) );
			for ( HHuginn::identifier_id_t n : hierarchy ) {
				hier.append( "->" );
				hier.append( _runtime->identifier_name( n ) );
			}
			throw HHuginn::HHuginnRuntimeException(
				"Class derivation cycle detected `"_ys.append( hier ).append( "'." ),
				MAIN_FILE_ID,
				cc->_basePosition.get()
			);
		}
		cc = it->second.get();
	}
	return;
	M_EPILOG
}

void OCompiler::create_lambda( executing_parser::position_t position_ ) {
	M_PROLOG
	function_info_t fi( create_function_low( position_ ) );
	HHuginn::value_t fRef( _runtime->object_factory()->create_function_reference( fi.first, fi.second, "Lambda: "_ys.append( _runtime->identifier_name( fi.first ) ) ) );
	defer_store_direct( fRef, position_ );
	if ( _capturesLog.find( fi.first ) != _capturesLog.end() ) {
		defer_action( &HExpression::create_closure, position_ );
		current_expression()->commit_oper( OPERATOR::FUNCTION_CALL );
		f()._valueTypes.top()._type = _boundMethodClass_.type_id();
	}
	return;
	M_EPILOG
}

void OCompiler::verify_default_argument( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int lastDefaultValuePosition( fc._lastDefaultValuePosition );
	if ( ( lastDefaultValuePosition >= 0 ) && ( lastDefaultValuePosition < static_cast<int>( fc._parameters.get_size() - 1 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( "Missing default argument.", MAIN_FILE_ID, position_.get() );
	}
	return;
	M_EPILOG
}

void OCompiler::add_parameter( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t parameterIdentifier( _runtime->identifier_id( name_ ) );
	captures_log_t::const_iterator cli( _capturesLog.find( fc._functionIdentifier ) );
	if ( ( cli != _capturesLog.end() ) && ( find( cli->second.begin(), cli->second.end(), parameterIdentifier ) != cli->second.end() ) ) {
		throw HHuginn::HHuginnRuntimeException( "Symbol `"_ys.append( name_ ).append( "' is a already used as a capture." ), MAIN_FILE_ID, position_.get() );
	}
	if ( find( fc._parameters.begin(), fc._parameters.end(), parameterIdentifier ) != fc._parameters.end() ) {
		throw HHuginn::HHuginnRuntimeException( "Parameter `"_ys.append( name_ ).append( "' was already defined." ), MAIN_FILE_ID, position_.get() );
	}
	verify_default_argument( position_ );
	_usedIdentifiers[parameterIdentifier].write( position_.get(), OIdentifierUse::TYPE::VARIABLE );
	_executionStepsBacklog.emplace_back(
		OExecutionStep::OPERATION::DEFINE,
		HHuginn::expression_t(),
		fc._scopeStack.top(),
		!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : INVALID_IDENTIFIER,
		-1,
		parameterIdentifier,
		position_.get()
	);
	fc._parameters.push_back( parameterIdentifier );
	return;
	M_EPILOG
}

void OCompiler::add_capture( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t captureIdentifier( _runtime->identifier_id( name_ ) );
	if ( find( fc._captures.begin(), fc._captures.end(), captureIdentifier ) != fc._captures.end() ) {
		throw HHuginn::HHuginnRuntimeException( "Capture `"_ys.append( name_ ).append( "' was already defined." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::expression_t& expression( current_expression() );
	if ( fc._captures.is_empty() ) {
		expression->oper( OPERATOR::FUNCTION_CALL, position_.get() );
	}
	defer_get_reference( name_, position_ );
	fc._valueTypes.pop();
	expression->oper( OPERATOR::FUNCTION_ARGUMENT, position_.get() );
	fc._captures.push_back( captureIdentifier );
	return;
	M_EPILOG
}

void OCompiler::create_scope( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( ! fc._inline ) {
		++ _statementIdGenerator;
	}
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, _fileId, position_.get() ) );
	fc._inline = false;
	return;
	M_EPILOG
}

void OCompiler::commit_scope( executing_parser::position_t ) {
	M_PROLOG
	M_DEBUG_CODE( OFunctionContext& fc( f() ); );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( scope );
	return;
	M_EPILOG
}

void OCompiler::commit_catch( executing_parser::position_t ) {
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

void OCompiler::reset_expression( void ) {
	M_PROLOG
	current_expression() = new_expression( _fileId );
	OFunctionContext& fc( f() );
	M_ASSERT( fc._operations.is_empty() );
	M_ASSERT( fc._variables.is_empty() );
	fc._valueTypes.clear();
	return;
	M_EPILOG
}

void OCompiler::pop_function_context( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( fc._loopCount == 0 );
	M_ASSERT( fc._loopSwitchCount == 0 );
	while ( ! fc._scopeStack.is_empty() ) {
		pop_scope_context_low();
	}
	_functionContexts.pop();
	return;
	M_EPILOG
}

HHuginn::scope_t OCompiler::pop_scope_context( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OScopeContext& sc( *fc._scopeStack.top() );
	HHuginn::scope_t scope( yaal::move( sc._scope ) );
	if (
		( _setup & HHuginn::COMPILER::BE_STRICT )
		&& ( sc._terminatedAt != NOT_TERMINATED )
		&& ( scope->statement_count() > ( sc._terminatedAt + 1 ) )
	) {
		throw HHuginn::HHuginnRuntimeException(
			"Statement is unreachable.",
			MAIN_FILE_ID,
			scope->statement_position_at( sc._terminatedAt + 1 )
		);
	}
	pop_scope_context_low();
	return ( scope );
	M_EPILOG
}

void OCompiler::pop_scope_context_low( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	_scopeContextCache.push_back( yaal::move( fc._scopeStack.top() ) );
	fc._scopeStack.pop();
	return;
	M_EPILOG
}

void OCompiler::terminate_scope( HScope::statement_t&& statement_ ) {
	M_PROLOG
	OScopeContext& sc( current_scope_context() );
	int terminatedAt( sc._scope->add_statement( statement_ ) );
	if ( sc._terminatedAt == NOT_TERMINATED ) {
		sc._terminatedAt = terminatedAt;
	}
	return;
	M_EPILOG
}

void OCompiler::start_if_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._scopeStack.top()->_scopeChain.is_empty() ) {
		fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, position_.get() ) );
	}
	fc._inline = true;
	return;
	M_EPILOG
}

void OCompiler::start_else_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._inline = true;
	return;
	M_EPILOG
}

void OCompiler::start_loop_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._inline = true;
	++ fc._loopCount;
	++ fc._loopSwitchCount;
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::start_switch_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ fc._loopSwitchCount;
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::start_subexpression( executing_parser::position_t position_ ) {
	M_PROLOG
	f().expressions_stack().emplace( 1, new_expression( _fileId, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::add_subexpression( OPERATOR op_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc.expressions_stack().top().emplace_back( new_expression( _fileId, position_.get() ) );
	fc._operations.emplace( op_, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::commit_boolean( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc.expressions_stack().top().get_size() > 1 ) {
		HHuginn::value_t And( _runtime->object_factory()->create<HBooleanEvaluator>( _runtime, fc.expressions_stack().top(), operator_ ) );
		fc.expressions_stack().pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top()._type == HHuginn::TYPE::BOOLEAN );
		fc._valueTypes.pop();
		defer_store_direct( And, position_ );
		HExpression& expression( *current_expression() );
		expression.oper( operator_, position_.get() );
		defer_action( operator_ == OPERATOR::BOOLEAN_AND ? &HExpression::boolean_and : &HExpression::boolean_or, position_ );
		expression.commit_oper( operator_ );
	} else {
		HHuginn::expression_t e( fc._scopeStack.top()->expression() );
		fc.expressions_stack().pop();
		fc._scopeStack.top()->expression()->merge( *e );
	}
	return;
	M_EPILOG
}

void OCompiler::commit_ternary( executing_parser::position_t position_ ) {
	OFunctionContext& fc( f() );
	HFunction::expressions_t const& exprs( fc.expressions_stack().top() );
	if ( exprs.get_size() > 1 ) {
		M_ASSERT( exprs.get_size() == 3 );
		HHuginn::value_t ternary( _runtime->object_factory()->create<HHuginn::HTernaryEvaluator>( exprs[0], exprs[1], exprs[2] ) );
		fc.expressions_stack().pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top()._type == HHuginn::TYPE::UNKNOWN );
		fc._valueTypes.pop();
		defer_store_direct( ternary, position_ );
		HExpression& expression( *current_expression() );
		expression.oper( OPERATOR::TERNARY, position_.get() );
		defer_action( &HExpression::ternary, position_ );
		expression.commit_oper( OPERATOR::TERNARY );
	} else {
		HHuginn::expression_t e( fc._scopeStack.top()->expression() );
		fc.expressions_stack().pop();
		fc._scopeStack.top()->expression()->merge( *e );
	}
}

void OCompiler::add_return_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	HHuginn::expression_t& e( current_expression() );
	if ( e->is_empty() ) {
		e->add_execution_step( hcore::call( &HExpression::store_direct, e.raw(), _runtime->none_value(), _1, position_.get() ) );
	}
	terminate_scope( make_pointer<HReturn>( e, _fileId, position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_throw_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	terminate_scope( make_pointer<HThrow>( current_expression(), _fileId, position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_break_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	if ( fc._loopSwitchCount == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid context for `break' statement.", MAIN_FILE_ID, position_.get() );
	}
	terminate_scope( make_pointer<HBreak>( HFrame::STATE::BREAK, _fileId, position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_continue_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	if ( fc._loopCount == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid context for `continue' statement.", MAIN_FILE_ID, position_.get() );
	}
	terminate_scope( make_pointer<HBreak>( HFrame::STATE::CONTINUE, _fileId, position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_while_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	OScopeContext& sc( current_scope_context() );
	HScope::statement_t whileStatement(
		make_pointer<HWhile>( sc._statementId, current_expression(), scope, _fileId, position_.get() )
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

void OCompiler::commit_assignable( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	mark_expression_position( position_ );
	M_ASSERT( ! fc._variables.is_empty() );
	if ( fc._variables.top()._identifier != INVALID_IDENTIFIER ) {
		note_type( fc._variables.top()._identifier, type_id( HHuginn::TYPE::UNKNOWN ) );
	}
	fc._variables.clear();
	return;
	M_EPILOG
}

void OCompiler::save_control_variable( executing_parser::position_t position_ ) {
	M_PROLOG
	commit_assignable( position_ );
	OFunctionContext& fc( f() );
	fc.expressions_stack().top().emplace_back( new_expression( _fileId ) );
	M_EPILOG
}

void OCompiler::commit_catch_control_variable( executing_parser::position_t position_ ) {
	M_PROLOG
	commit_assignable( position_ );
	OFunctionContext& fc( f() );
	fc._inline = true;
	return;
	M_EPILOG
}

void OCompiler::start_function_call( executing_parser::position_t position_ ) {
	M_PROLOG
	defer_oper_direct( OPERATOR::FUNCTION_CALL, position_ );
	OFunctionContext& fc( f() );
	if ( fc._isAssert ) {
		++ fc._nestedCalls;
	}
	return;
	M_EPILOG
}

void OCompiler::close_function_call( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._isAssert ) {
		-- fc._nestedCalls;
		OScopeContext& sc( *fc._scopeStack.top() );
		if ( ( fc._nestedCalls == 0 ) && ( sc._assertExpressionEnd == 0 ) ) {
			sc._assertExpressionEnd = position_.get();
		}
	}
	return;
	M_EPILOG
}

void OCompiler::set_type_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, _fileId, position_.get() ) );
	OScopeContext& sc( current_scope_context() );
	sc._exceptionType = _runtime->identifier_id( name_ );
	return;
	M_EPILOG
}

void OCompiler::add_for_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	OScopeContext& sc( *fc._scopeStack.top() );
	HHuginn::expression_t source( fc.expressions_stack().top().back() );
	fc.expressions_stack().top().pop_back();
	HScope::statement_t forStatement(
		make_pointer<HFor>( sc._statementId, current_expression(), source, scope, _fileId, position_.get() )
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

void OCompiler::commit_if_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	fc._scopeStack.top()->_scopeChain.emplace_back( scope, current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::commit_else_clause( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() ); /* don't squash! pop_scope_context() changes fc._scopeStack */
	fc._scopeStack.top()->_else = scope;
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_if_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	OScopeContext& sc( *fc._scopeStack.top() );
	HScope::statement_t ifStatement( make_pointer<HIf>( sc._statementId, sc._scopeChain, sc._else, _fileId, position_.get() ) );
	sc._scopeChain.clear();
	sc._else.reset();
	pop_scope_context_low();
	M_ASSERT( ! fc._scopeStack.is_empty() );
	current_scope()->add_statement( ifStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_try_catch_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HTryCatch::catches_t catches( yaal::move( fc._scopeStack.top()->_catches ) );
	HHuginn::scope_t scope( pop_scope_context() );
	HScope::statement_t tryCatchStatement(
		make_pointer<HTryCatch>( scope, catches, _fileId, position_.get() )
	);
	current_scope()->add_statement( tryCatchStatement );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_switch_statement( executing_parser::position_t position_ ) {
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
			position_.get()
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

void OCompiler::commit_expression( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	current_scope()->add_statement( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_default_value( executing_parser::position_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	fc._lastDefaultValuePosition = static_cast<int>( fc._parameters.get_size() - 1 );
	fc._defaultValues.push_back( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_field_definition( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( !! _classContext );
	HString const& doc( _runtime->huginn()->get_comment( position_.get() ) );
	int idx( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ) );
	_classContext->_fieldDefinitions.insert( make_pair( idx, current_expression() ) );
	_classContext->_docs.insert( make_pair( idx, doc ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::mark_expression_position( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	current_expression()->set_position( _fileId, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::defer_oper( code_point_t operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OPERATOR o( OPERATOR::NONE );
	switch ( operator_.get() ) {
		case ( '+' ): o = OPERATOR::PLUS;        break;
		case ( '-' ): o = OPERATOR::MINUS;       break;
		case ( '*' ): o = OPERATOR::MULTIPLY;    break;
		case ( '/' ): o = OPERATOR::DIVIDE;      break;
		case ( '%' ): o = OPERATOR::MODULO;      break;
		case ( '^' ): o = OPERATOR::POWER;       break;
		case ( '(' ): o = OPERATOR::PARENTHESIS; break;
		case ( '|' ): o = OPERATOR::ABSOLUTE;    break;
		case ( '!' ): o = OPERATOR::BOOLEAN_NOT; break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->oper( o, position_.get() );
	f()._operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::defer_str_oper( yaal::hcore::HString const& operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, OPERATOR> operator_lookup_t;
	static operator_lookup_t const operatorLookup( {
		{ op_to_str( OPERATOR::ASSIGN ),           OPERATOR::ASSIGN },
		{ op_to_str( OPERATOR::PLUS_ASSIGN ),      OPERATOR::PLUS_ASSIGN },
		{ op_to_str( OPERATOR::MINUS_ASSIGN ),     OPERATOR::MINUS_ASSIGN },
		{ op_to_str( OPERATOR::MULTIPLY_ASSIGN ),  OPERATOR::MULTIPLY_ASSIGN },
		{ op_to_str( OPERATOR::DIVIDE_ASSIGN ),    OPERATOR::DIVIDE_ASSIGN },
		{ op_to_str( OPERATOR::MODULO_ASSIGN ),    OPERATOR::MODULO_ASSIGN },
		{ op_to_str( OPERATOR::POWER_ASSIGN ),     OPERATOR::POWER_ASSIGN },
		{ op_to_str( OPERATOR::EQUALS ),           OPERATOR::EQUALS },
		{ op_to_str( OPERATOR::NOT_EQUALS ),       OPERATOR::NOT_EQUALS },
		{ op_to_str( OPERATOR::LESS_OR_EQUAL ),    OPERATOR::LESS_OR_EQUAL },
		{ op_to_str( OPERATOR::GREATER_OR_EQUAL ), OPERATOR::GREATER_OR_EQUAL },
		{ op_to_str( OPERATOR::BOOLEAN_AND ),      OPERATOR::BOOLEAN_AND },
		{ op_to_str( OPERATOR::BOOLEAN_OR ),       OPERATOR::BOOLEAN_OR },
		{ op_to_str( OPERATOR::BOOLEAN_XOR ),      OPERATOR::BOOLEAN_XOR },
		{ op_to_str( OPERATOR::LESS ),             OPERATOR::LESS },
		{ op_to_str( OPERATOR::GREATER ),          OPERATOR::GREATER }
	} );
	OPERATOR o( operatorLookup.at( operator_ ) );
	current_expression()->oper( o, position_.get() );
	f()._operations.emplace( o, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::defer_oper_direct( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::expression_t& expression( current_expression() );
	expression->oper( operator_, position_.get() );
	fc._operations.emplace( operator_, position_.get() );
	/*
	 * We want to support assert() statement.
	 * We need to know where assert's condition expression ends.
	 * It can end at the close of assert statement if user did not add any description message,
	 * this case is handled in ::close_function_call().
	 * It can end at first comma separating assert's arguments.
	 * To find this place we wait for `defer` of function argument operation
	 * but for SECOND argument of this call.
	 * Total number of operations on the stack (including call itself) is 3.
	 */
	static int const ASSERT_SECOND_ARGUMENT_OPERATION_COUNT( 1 /*function call*/ + 1 /*assert's condition*/ + 1 /*assert's user message*/ );
	if (
		( operator_ == OPERATOR::FUNCTION_ARGUMENT )
		&& fc._isAssert
		&& ( fc._nestedCalls == 1 )
		&& ( fc._operations.size() == ASSERT_SECOND_ARGUMENT_OPERATION_COUNT )
	) {
		OScopeContext& sc( *fc._scopeStack.top() );
		sc._assertExpressionEnd = position_.get();
	}
	return;
	M_EPILOG
}

bool OCompiler::is_numeric( HHuginn::type_id_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::INTEGER ) || ( type_ == HHuginn::TYPE::REAL ) || ( type_ == HHuginn::TYPE::NUMBER ) );
}

bool OCompiler::is_collection( HHuginn::type_id_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::LIST ) || ( type_ == HHuginn::TYPE::DEQUE ) || ( type_ == HHuginn::TYPE::DICT ) || ( type_ == HHuginn::TYPE::LOOKUP ) || ( type_ == HHuginn::TYPE::ORDER ) || ( type_ == HHuginn::TYPE::SET ) );
}

bool OCompiler::is_comparable( HHuginn::type_id_t type_ ) {
	return (
		is_numeric( type_ )
		|| ( type_ == HHuginn::TYPE::STRING )
		|| ( type_ == HHuginn::TYPE::CHARACTER )
		|| ( type_ == HHuginn::TYPE::FUNCTION_REFERENCE )
		|| ( type_ == HHuginn::TYPE::LIST )
		|| ( type_ == HHuginn::TYPE::DEQUE )
	);
}

bool OCompiler::is_boolean_congruent( HHuginn::type_id_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::BOOLEAN ) || ( type_ == HHuginn::TYPE::UNKNOWN ) || ( type_ == HHuginn::TYPE::REFERENCE ) );
}

bool OCompiler::is_unknown( HHuginn::type_id_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::NOT_BOOLEAN ) || ( type_ == HHuginn::TYPE::UNKNOWN ) || ( type_ == HHuginn::TYPE::REFERENCE ) );
}

bool OCompiler::is_numeric_congruent( HHuginn::type_id_t type_ ) {
	return ( is_numeric( type_ ) || is_unknown( type_ ) );
}

bool OCompiler::is_comparable_congruent( HHuginn::type_id_t type_ ) {
	return ( is_comparable( type_ ) || is_unknown( type_ ) );
}

bool OCompiler::is_reference_congruent( HHuginn::type_id_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::REFERENCE ) || ( type_ == HHuginn::TYPE::UNKNOWN ) );
}

bool OCompiler::is_integer_congruent( HHuginn::type_id_t type_ ) {
	return ( ( type_ == HHuginn::TYPE::INTEGER ) || is_unknown( type_ ) );
}

bool OCompiler::is_summable( HHuginn::type_id_t type_ ) {
	return ( is_numeric_congruent( type_ ) || ( type_ == HHuginn::TYPE::STRING ) || is_collection( type_ ) );
}

bool OCompiler::are_congruous( HHuginn::type_id_t t1_, HHuginn::type_id_t t2_ ) {
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

HHuginn::HHuginn::type_id_t OCompiler::congruent( HHuginn::type_id_t t1_, HHuginn::type_id_t t2_ ) {
	HHuginn::type_id_t t( type_id( HHuginn::TYPE::NOT_BOOLEAN ) );
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
	current_expression()->commit_oper( o );
	fc._operations.pop();
	HHuginn::type_id_t t1( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( op_to_str( o ), t2, t1, MAIN_FILE_ID, position_.get() );
	}
	if ( ! ( is_summable( t1 ) && is_summable( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			HString( o == OPERATOR::PLUS ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB] )
				.append( type_name( t2 ) )
				.append( ", " )
				.append( type_name( t1 ) ),
			MAIN_FILE_ID,
			p
		);
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
	current_expression()->commit_oper( o );
	fc._operations.pop();
	HHuginn::type_id_t t1( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( op_to_str( o ), t2, t1, MAIN_FILE_ID, position_.get() );
	}
	if ( ! ( is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			HString( o == OPERATOR::MULTIPLY ? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV] )
				.append( type_name( t2 ) )
				.append( ", " )
				.append( type_name( t1 ) ),
			MAIN_FILE_ID,
			p
		);
	}
	fc._valueTypes.push( congruent( t1, t2 ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_power( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	bool hasPower( false );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::POWER ) ) {
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		HHuginn::type_id_t t1( fc._valueTypes.top()._type );
		fc._valueTypes.pop();
		HHuginn::type_id_t t2( fc._valueTypes.top()._type );
		fc._valueTypes.pop();
		if ( ! are_congruous( t1, t2 ) ) {
			operands_type_mismatch( op_to_str( OPERATOR::POWER ), t2, t1, MAIN_FILE_ID, p );
		}
		if ( ! ( is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP] )
					.append( type_name( t2 ) )
					.append( ", " )
					.append( type_name( t1 ) ),
				MAIN_FILE_ID,
				p
			);
		}
		fc._valueTypes.push( congruent( t1, t2 ) );
		hasPower = true;
	}
	if ( hasPower ) {
		defer_action( &HExpression::power, position_ );
		current_expression()->commit_oper( OPERATOR::POWER_TERM );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_factorial( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._valueTypes.is_empty() );
	defer_action( &HExpression::factorial, position_ );
	if ( ! is_numeric_congruent( fc._valueTypes.top()._type ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_NUM] )
				.append( type_name( fc._valueTypes.top()._type ) ),
			MAIN_FILE_ID,
			position_.get()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_negate( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	OPositionedOperator const& po(  fc._operations.top() );
	int p( po._position  );
	M_ASSERT( po._operator == OPERATOR::NEGATE );
	M_ASSERT( ! fc._valueTypes.is_empty() );
	defer_action( &HExpression::negate, position_ );
	current_expression()->commit_oper( OPERATOR::NEGATE );
	fc._operations.pop();
	if ( ! is_numeric_congruent( fc._valueTypes.top()._type ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_NUM] )
				.append( type_name( fc._valueTypes.top()._type ) ),
			MAIN_FILE_ID,
			p
		);
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
	char const* os( op_to_str( o ) );
	switch ( o ) {
		case ( OPERATOR::LESS ):             { defer_action( &HExpression::less, position_ );             } break;
		case ( OPERATOR::GREATER ):          { defer_action( &HExpression::greater, position_ );          } break;
		case ( OPERATOR::LESS_OR_EQUAL ):    { defer_action( &HExpression::less_or_equal, position_ );    } break;
		case ( OPERATOR::GREATER_OR_EQUAL ): { defer_action( &HExpression::greater_or_equal, position_ ); } break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->commit_oper( o );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HHuginn::type_id_t t1( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( os, t2, t1, MAIN_FILE_ID, p );
	}
	if ( ! ( is_comparable_congruent( t1 ) && is_comparable_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_CMP] )
				.append( type_name( t2 ) )
				.append( ", " )
				.append( type_name( t1 ) ),
			MAIN_FILE_ID,
			p
		);
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::BOOLEAN ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_boolean( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	int p( po._position );
	if ( !! action_ ) {
		M_ASSERT( action_ == &HExpression::boolean_xor );
		defer_action( action_, position_ );
		current_expression()->commit_oper( po._operator );
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HHuginn::type_id_t t1( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	if ( ! ( is_boolean_congruent( t1 ) && is_boolean_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL] )
				.append( type_name( t2 ) )
				.append( ", " )
				.append( type_name( t1 ) ),
			MAIN_FILE_ID,
			p
		);
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::BOOLEAN ) );
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
	HHuginn::type_id_t t0( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	if ( ! is_boolean_congruent( t0 ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( type_name( t0 ) ),
			MAIN_FILE_ID,
			p
		);
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::UNKNOWN ) );
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
	current_expression()->commit_oper( o );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HHuginn::type_id_t t1( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top()._type );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( op_to_str( o ), t2, t1, MAIN_FILE_ID, p );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::BOOLEAN ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_assign( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	bool hasAssign( false );
	while ( ! fc._operations.is_empty() ) {
		OPERATOR o( fc._operations.top()._operator );
		if ( ( o < OPERATOR::ASSIGN ) || ( o > OPERATOR::POWER_ASSIGN ) ) {
			break;
		}
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		M_ASSERT( ! fc._variables.is_empty() );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		HHuginn::type_id_t t1( fc._valueTypes.top()._type );
		fc._valueTypes.pop();
		HHuginn::type_id_t t2( fc._valueTypes.top()._type );
		fc._valueTypes.pop();
		OFunctionContext::OVariableRef varRef( fc._variables.top() );
		HHuginn::type_id_t realDestType( t2 );
		if ( realDestType == HHuginn::TYPE::UNKNOWN ) {
			realDestType = guess_type( varRef._identifier );
		}
		if ( varRef._identifier != INVALID_IDENTIFIER ) {
			note_type( varRef._identifier, t1 );
		}
		if ( varRef._identifier != INVALID_IDENTIFIER ) {
			if ( o == OPERATOR::ASSIGN ) {
				_usedIdentifiers[varRef._identifier].write( position_.get(), OIdentifierUse::TYPE::VARIABLE );
			} else if ( are_congruous( t1, realDestType ) ) {
				_usedIdentifiers[varRef._identifier].read( position_.get(), OIdentifierUse::TYPE::VARIABLE );
				M_ASSERT( varRef._executionStepIndex >= 0 );
				_executionStepsBacklog[varRef._executionStepIndex]._operation = OExecutionStep::OPERATION::UPDATE;
			} else {
				operands_type_mismatch( op_to_str( o ), realDestType, t1, MAIN_FILE_ID, p );
			}
		}
		switch ( o ) {
			case ( OPERATOR::PLUS_ASSIGN ):
			case ( OPERATOR::MINUS_ASSIGN ): {
				if ( ! ( is_summable( t1 ) && is_summable( realDestType ) ) ) {
					throw HHuginn::HHuginnRuntimeException(
						HString( o == OPERATOR::PLUS_ASSIGN ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB] )
							.append( type_name( realDestType ) )
							.append( ", " )
							.append( type_name( t1 ) ),
						MAIN_FILE_ID,
						p
					);
				}
			} break;
			case ( OPERATOR::MULTIPLY_ASSIGN ):
			case ( OPERATOR::DIVIDE_ASSIGN ):
			case ( OPERATOR::MODULO_ASSIGN ):
			case ( OPERATOR::POWER_ASSIGN ): {
				if ( ! ( is_numeric_congruent( t1 ) && is_numeric_congruent( realDestType ) ) ) {
					throw HHuginn::HHuginnRuntimeException(
						HString(
							o == OPERATOR::MULTIPLY_ASSIGN
								? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL]
								: (
									o == OPERATOR::POWER_ASSIGN ? _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV]
								)
						)
							.append( type_name( realDestType ) )
							.append( ", " )
							.append( type_name( t1 ) ),
						MAIN_FILE_ID,
						p
					);
				}
			} break;
			case ( OPERATOR::ASSIGN ): break;
			default: {
				M_ASSERT( ! "bad code path"[0] );
			} break;
		}
		fc._variables.pop();
		if ( ! is_reference_congruent( t2 ) ) {
			throw HHuginn::HHuginnRuntimeException( "Setting a non reference location.", MAIN_FILE_ID, p );
		}
		fc._valueTypes.push( congruent( t1, t2 ) );
		hasAssign = true;
	}
	if ( hasAssign ) {
		defer_action( &HExpression::set_variable, position_ );
		current_expression()->commit_oper( OPERATOR::ASSIGN_TERM );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_subscript( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int p( position_.get() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		throw HHuginn::HHuginnRuntimeException( "`assert' is a restricted keyword.", MAIN_FILE_ID, p );
	}
	M_ASSERT( ! fc._operations.is_empty() );
	HHuginn::expression_t& expression( current_expression() );
	OPERATOR op( fc._operations.top()._operator );
	int range( 0 );
	bool nonInteger( false );
	while ( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
		if ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) {
			++ range;
		} else {
			if ( ! ( nonInteger || is_integer_congruent( fc._valueTypes.top()._type ) || ( fc._valueTypes.top()._type == HHuginn::TYPE::NONE ) ) ) {
				nonInteger = true;
				p = fc._operations.top()._position;
			}
			fc._valueTypes.pop();
		}
		fc._operations.pop();
		op = fc._operations.top()._operator;
	}
	M_ASSERT( fc._operations.top()._operator == OPERATOR::SUBSCRIPT );
	if ( range > 0 ) {
		if ( nonInteger ) {
			throw HHuginn::HHuginnRuntimeException( "Range specifier is not an integer.", MAIN_FILE_ID, p );
		}
		expression->add_execution_step( hcore::call( &HExpression::range, expression.raw(), _1, position_.get() ) );
		expression->commit_oper( OPERATOR::RANGE );
	} else {
		expression->add_execution_step( hcore::call( &HExpression::subscript, expression.raw(), HExpression::ACCESS::VALUE, _1, position_.get() ) );
		expression->commit_oper( OPERATOR::SUBSCRIPT );
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 1 );
	fc._valueTypes.pop();
	fc._valueTypes.push( type_id( HHuginn::TYPE::REFERENCE ) );
	fc._lastDereferenceOperator = OPERATOR::SUBSCRIPT;
	return;
	M_EPILOG
}

namespace {
HHuginn::type_id_t function_ref_to_type_id( HHuginn::identifier_id_t identifierId_ ) {
	HHuginn::type_id_t t( type_id( HHuginn::TYPE::UNKNOWN ) );
	typedef HHashMap<HHuginn::identifier_id_t, HHuginn::type_id_t> fun_to_type_t;
	static fun_to_type_t const funToType = {
		{ BUILTIN::INTEGER_IDENTIFIER,   type_id( HHuginn::TYPE::INTEGER ) },
		{ BUILTIN::REAL_IDENTIFIER,      type_id( HHuginn::TYPE::REAL ) },
		{ BUILTIN::NUMBER_IDENTIFIER,    type_id( HHuginn::TYPE::NUMBER ) },
		{ BUILTIN::STRING_IDENTIFIER,    type_id( HHuginn::TYPE::STRING ) },
		{ BUILTIN::CHARACTER_IDENTIFIER, type_id( HHuginn::TYPE::CHARACTER ) },
		{ BUILTIN::BOOLEAN_IDENTIFIER,   type_id( HHuginn::TYPE::BOOLEAN ) },
		{ BUILTIN::SIZE_IDENTIFIER,      type_id( HHuginn::TYPE::INTEGER ) },
		{ BUILTIN::LIST_IDENTIFIER,      type_id( HHuginn::TYPE::LIST ) },
		{ BUILTIN::DEQUE_IDENTIFIER,     type_id( HHuginn::TYPE::DEQUE ) },
		{ BUILTIN::DICT_IDENTIFIER,      type_id( HHuginn::TYPE::DICT ) },
		{ BUILTIN::LOOKUP_IDENTIFIER,    type_id( HHuginn::TYPE::LOOKUP ) },
		{ BUILTIN::ORDER_IDENTIFIER,     type_id( HHuginn::TYPE::ORDER ) },
		{ BUILTIN::SET_IDENTIFIER,       type_id( HHuginn::TYPE::SET ) },
		{ BUILTIN::OBSERVE_IDENTIFIER,   type_id( HHuginn::TYPE::OBSERVER ) },
		{ BUILTIN::TYPE_IDENTIFIER,      type_id( HHuginn::TYPE::FUNCTION_REFERENCE ) }
	};
	fun_to_type_t::const_iterator it( funToType.find( identifierId_ ) );
	if ( it != funToType.end() ) {
		t = it->second;
	}
	return ( t );
}
}

void OCompiler::dispatch_function_call( expression_action_t const& action_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		int from( position_.get() + 1 );
		OScopeContext& sc( *fc._scopeStack.top() );
		int len( sc._assertExpressionEnd - from );
		sc._assertExpressionEnd = 0;
		current_expression()->oper( OPERATOR::FUNCTION_ARGUMENT, position_.get() );
		current_expression()->add_execution_step(
			hcore::call(
				&HExpression::store_direct, current_expression().raw(),
				_runtime->object_factory()->create_string( _runtime->huginn()->get_snippet( from, len ).trim() ),
				_1,
				position_.get()
			)
		);
		fc._isAssert = false;
	}
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) ) {
		fc._operations.pop();
		fc._valueTypes.pop();
	}
	HHuginn::type_id_t t( type_id( HHuginn::TYPE::UNKNOWN ) );
	if ( action_ == &HExpression::function_call ) {
		OFunctionContext::OValueDesc vd( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( vd._type == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			t = function_ref_to_type_id( vd._identifier );
		}
	} else {
		t = type_id( HHuginn::TYPE::DICT );
	}
	fc._valueTypes.push( t );
	M_ASSERT( fc._operations.top()._operator == OPERATOR::FUNCTION_CALL );
	defer_action( action_, position_ );
	current_expression()->commit_oper( OPERATOR::FUNCTION_CALL );
	fc._operations.pop();
	return;
	M_EPILOG
}

void OCompiler::dispatch_member_access( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		throw HHuginn::HHuginnRuntimeException( "`assert' is a restricted keyword.", MAIN_FILE_ID, position_.get() );
	}
	fc._valueTypes.pop();
	fc._valueTypes.push( type_id( HHuginn::TYPE::REFERENCE ) );
	fc._operations.pop();
	fc._lastDereferenceOperator = OPERATOR::MEMBER_ACCESS;
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
		case ( OPERATOR::PLUS ):          { dispatch_plus( position_ );          } break;
		case ( OPERATOR::MULTIPLY ):      { dispatch_mul( position_ );           } break;
		case ( OPERATOR::POWER ):         { dispatch_power( position_ );         } break;
		case ( OPERATOR::FACTORIAL ):     { dispatch_factorial( position_ );     } break;
		case ( OPERATOR::NEGATE ):        { dispatch_negate( position_ );        } break;
		case ( OPERATOR::SUBSCRIPT ):     { dispatch_subscript( position_ );     } break;
		case ( OPERATOR::ASSIGN ):        { dispatch_assign( position_ );        } break;
		case ( OPERATOR::MEMBER_ACCESS ): { dispatch_member_access( position_ ); } break;
		case ( OPERATOR::FUNCTION_CALL ): { dispatch_function_call( &HExpression::function_call, position_ ); } break;
		case ( OPERATOR::MAKE_DICT ):     { dispatch_function_call( &HExpression::make_dict, position_ ); } break;
		case ( OPERATOR::PARENTHESIS ):
		case ( OPERATOR::ABSOLUTE ): {
			M_ASSERT( ( o == OPERATOR::ABSOLUTE ) || ( o == OPERATOR::PARENTHESIS ) );
			defer_action( &HExpression::close_parenthesis, position_ );
			current_expression()->commit_oper( o );
			fc._operations.pop();
			if ( o == OPERATOR::ABSOLUTE ) {
				M_ASSERT( ! fc._valueTypes.is_empty() );
				if ( ! is_numeric_congruent( fc._valueTypes.top()._type ) ) {
					throw HHuginn::HHuginnRuntimeException(
						hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_NUM] )
							.append( type_name( fc._valueTypes.top()._type ) ),
						MAIN_FILE_ID,
						p
					);
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
			current_expression()->commit_oper( o );
			fc._operations.pop();
			HHuginn::type_id_t t( fc._valueTypes.top()._type );
			if ( ! is_boolean_congruent( t ) ) {
				throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( type_name( t ) ), MAIN_FILE_ID, p );
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
	OFunctionContext& fc( f() );
	if ( ( fc._lastDereferenceOperator != OPERATOR::SUBSCRIPT ) && ( fc._lastDereferenceOperator != OPERATOR::MEMBER_ACCESS ) ) {
		throw HHuginn::HHuginnRuntimeException( "Assignment to function result.", MAIN_FILE_ID, position_.get() );
	}
	if ( fc._lastDereferenceOperator == OPERATOR::SUBSCRIPT ) {
		current_expression()->pop_execution_step();
		current_expression()->add_execution_step(
			hcore::call( &HExpression::subscript, current_expression().raw(), HExpression::ACCESS::REFERENCE, _1, position_.get() )
		);
	} else {
		HExpression& expression( *current_expression() );
		expression.pop_execution_step();
		expression.add_execution_step(
			hcore::call( &HExpression::get_field, current_expression().raw(), HExpression::ACCESS::REFERENCE, fc._lastMemberName, _1, _fileId )
		);
	}
	fc._variables.emplace( INVALID_IDENTIFIER, -1 );
	return;
	M_EPILOG
}

void OCompiler::defer_get_reference( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t refIdentifier( _runtime->identifier_id( value_ ) );
	_usedIdentifiers[refIdentifier].read( position_.get() );
	bool keyword( false );
	bool isAssert( refIdentifier == KEYWORD::ASSERT_IDENTIFIER );
	bool isFieldDefinition( !! _classContext && ( _functionContexts.get_size() == 1 ) );
	if ( ( keyword = huginn::is_keyword( value_ ) ) ) {
		if ( isAssert ) {
			fc._isAssert = isAssert;
		}
		if ( ( ( value_ != KEYWORD::THIS ) && ( value_ != KEYWORD::SUPER ) && ! isAssert ) || ( isAssert && ! current_expression()->is_empty() ) ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "' is a restricted keyword." ), MAIN_FILE_ID, position_.get() );
		} else if ( ! isAssert && ! _classContext ) {
			throw HHuginn::HHuginnRuntimeException( "Keyword `"_ys.append( value_ ).append( "' can be used only in class context." ), MAIN_FILE_ID, position_.get() );
		}
	}
	if ( _isIncremental && ( refIdentifier == STANDARD_FUNCTIONS::MAIN_IDENTIFIER ) ) {
		throw HHuginn::HHuginnRuntimeException( "Referencing main() function in incremental mode is forbidden.", MAIN_FILE_ID, position_.get() );
	}
	HHuginn::expression_t& expression( current_expression() );
	if ( ( ! keyword || isAssert ) && huginn::is_builtin( value_ ) ) {
		/*
		 * We can do it here (as opposed to *::resolve_symbols()) because built-ins must exist,
		 * hence h->get_function() always succeeds, and built-ins cannot be overridden
		 * so they meaning stays always the same.
		 */
		current_expression()->add_execution_step(
			hcore::call(
				&HExpression::store_direct,
				current_expression().raw(),
				*_runtime->get_function( refIdentifier ),
				_1,
				position_.get()
			)
		);
	} else {
		if ( isFieldDefinition ) {
			throw HHuginn::HHuginnRuntimeException( "Dereferencing symbol `"_ys.append( value_ ).append( "' in field definition is forbidden." ), MAIN_FILE_ID, position_.get() );
		}
		if ( refIdentifier == KEYWORD::THIS_IDENTIFIER ) {
			expression->add_execution_step(
				hcore::call( &HExpression::get_this, expression.raw(), _1, position_.get() )
			);
		} else if ( refIdentifier == KEYWORD::SUPER_IDENTIFIER ) {
			expression->add_execution_step(
				hcore::call( &HExpression::get_super, expression.raw(), _1, position_.get() )
			);
		} else {
			int index( expression->add_execution_step( HExpression::execution_step_t() ) );
			_executionStepsBacklog.emplace_back(
				OExecutionStep::OPERATION::USE,
				expression,
				fc._scopeStack.top(),
				!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : INVALID_IDENTIFIER,
				index,
				refIdentifier,
				position_.get()
			);
		}
	}
	fc._valueTypes.push( { guess_type( refIdentifier ), refIdentifier } );
	return;
	M_EPILOG
}

void OCompiler::defer_get_field_reference( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t refIdentifier( _runtime->identifier_id( value_ ) );
	_usedIdentifiers[refIdentifier].read( position_.get(), OIdentifierUse::TYPE::FIELD );
	if ( huginn::is_keyword( value_ ) ) {
		if ( refIdentifier != KEYWORD::CONSTRUCTOR_IDENTIFIER ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "' is a restricted keyword." ), MAIN_FILE_ID, position_.get() );
		} else if ( ! _classContext ) {
			throw HHuginn::HHuginnRuntimeException( "Keyword `"_ys.append( value_ ).append( "' can be used only in class context." ), MAIN_FILE_ID, position_.get() );
		}
	}
	HExpression& expression( *current_expression() );
	expression.add_execution_step(
		hcore::call( &HExpression::get_field, current_expression().raw(), HExpression::ACCESS::VALUE, refIdentifier, _1, _fileId )
	);
	expression.commit_oper( OPERATOR::MEMBER_ACCESS );
	fc._lastMemberName = refIdentifier;
	fc._valueTypes.pop();
	fc._valueTypes.push( type_id( HHuginn::TYPE::REFERENCE ) );
}

void OCompiler::defer_make_variable( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( huginn::is_restricted( value_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "' is a restricted name." ), MAIN_FILE_ID, position_.get() );
	}
	bool isFieldDefinition( !! _classContext && ( _functionContexts.get_size() == 1 ) );
	if ( isFieldDefinition ) {
		throw HHuginn::HHuginnRuntimeException( "Defining symbol `"_ys.append( value_ ).append( "' in field definition is forbidden." ), MAIN_FILE_ID, position_.get() );
	}
	HHuginn::identifier_id_t varIdentifier( _runtime->identifier_id( value_ ) );
	HHuginn::expression_t& expression( current_expression() );
	int index( expression->add_execution_step( HExpression::execution_step_t() ) );
	_executionStepsBacklog.emplace_back(
		OExecutionStep::OPERATION::DEFINE,
		expression,
		fc._scopeStack.top(),
		!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : INVALID_IDENTIFIER,
		index,
		varIdentifier,
		position_.get()
	);
	fc._valueTypes.push( type_id( HHuginn::TYPE::UNKNOWN ) );
	fc._variables.emplace( varIdentifier, static_cast<int>( _executionStepsBacklog.get_size() - 1 ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_direct( HHuginn::value_t const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), value_, _1, position_.get() ) );
	f()._valueTypes.push( value_->type_id() );
	return;
	M_EPILOG
}

OPERATOR _copyConstContext_[] = {
	OPERATOR::ASSIGN,
	OPERATOR::FUNCTION_CALL,
	OPERATOR::FUNCTION_ARGUMENT,
	OPERATOR::SUBSCRIPT
};

void OCompiler::defer_store_real( double long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _runtime->object_factory()->create_real( value_ ), _1, position_.get() ) );
	} else {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_real, current_expression().raw(), value_, _1, position_.get() ) );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::REAL ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_integer( int long long value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _runtime->object_factory()->create_integer( value_ ), _1, position_.get() ) );
	} else {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_integer, current_expression().raw(), value_, _1, position_.get() ) );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::INTEGER ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_string( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _runtime->object_factory()->create_string( value_ ), _1, position_.get() ) );
	} else {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_string, current_expression().raw(), value_, _1, position_.get() ) );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::STRING ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_number( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _runtime->object_factory()->create_number( value_ ), _1, position_.get() ) );
	} else {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_number, current_expression().raw(), value_, _1, position_.get() ) );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::NUMBER ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_character( code_point_t value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _runtime->object_factory()->create_character( value_ ), _1, position_.get() ) );
	} else {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_character, current_expression().raw(), value_, _1, position_.get() ) );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::CHARACTER ) );
	return;
	M_EPILOG
}

}

}

}

