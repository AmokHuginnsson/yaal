/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "compiler.hxx"
#include "expression.hxx"
#include "introexpression.hxx"
#include "introfunction.hxx"
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

HHuginn::statement_id_t const INVALID_STATEMENT_IDENTIFIER( -1 );
static int const NOT_TERMINATED( -1 );

OCompiler::OActiveScope::OActiveScope( HHuginn::scope_t& scope_, HHuginn::expression_t& expression_ )
	: _scope( yaal::move( scope_ ) )
	, _expression( yaal::move( expression_ ) ) {
	return;
}

OCompiler::OScopeContext::OScopeContext(
	OFunctionContext* functionContext_,
	HHuginn::statement_id_t statementId_,
	int fileId_,
	executing_parser::range_t range_,
	HHuginn::scope_t const& scope_
) : _parent( ! functionContext_->_scopeStack.is_empty() ? functionContext_->_scopeStack.top().raw() : nullptr )
	, _scope( scope_ )
	, _expressionsStack()
	, _variableTypes()
	, _assertExpressionEnd( 0 )
	, _scopeChain()
	, _auxScope()
	, _catches()
	, _terminatedAt( NOT_TERMINATED )
	, _statementId( statementId_ )
	, _functionContext( functionContext_ )
	, _localVariables()
	, _argumentIndexes()
	, _localVariableCount( UNINITIALIZED_LOCAL_VARIABLE_COUNT ) {
	if ( ! _scope ) {
		_scope = make_pointer<HScope>( statementId_, fileId_, range_ );
	}
	_expressionsStack.emplace( 1, functionContext_->_compiler->new_expression( fileId_ ) );
	return;
}

HHuginn::HHuginn::expression_t& OCompiler::OScopeContext::expression( void ) {
	return ( _expressionsStack.top().back() );
}

huginn::HClass const* OCompiler::OScopeContext::guess_type( OCompiler const* compiler_, HHuginn::identifier_id_t identifierId_ ) const {
	M_PROLOG
	huginn::HClass const* c( compiler_->type_to_class( HHuginn::TYPE::UNKNOWN ) );
	OScopeContext const* sc( this );
	while ( sc ) {
		variable_types_t::const_iterator it( sc->_variableTypes.find( identifierId_ ) );
		if ( it != sc->_variableTypes.end() ) {
			c = it->second;
			break;
		}
		sc = sc->_parent;
	}
	return c;
	M_EPILOG
}

void OCompiler::OScopeContext::note_type( HHuginn::identifier_id_t identifierId_, huginn::HClass const* class_ ) {
	M_PROLOG
	OScopeContext* sc( this );
	bool found( false );
	while ( sc ) {
		variable_types_t::iterator it( sc->_variableTypes.find( identifierId_ ) );
		if ( it != sc->_variableTypes.end() ) {
			sc->_variableTypes[identifierId_] = class_;
			found = true;
			break;
		}
		sc = sc->_parent;
	}
	if ( ! found ) {
		_variableTypes[identifierId_] = class_;
	}
	return;
	M_EPILOG
}

OCompiler::OFunctionContext::OFunctionContext(
	OCompiler* compiler_,
	HHuginn::identifier_id_t functionId_,
	HHuginn::statement_id_t statementId_,
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
	, _assignmentVariables()
	, _variableCount()
	, _loopCount( 0 )
	, _loopSwitchCount( 0 )
	, _shortCircuit( 0 )
	, _nestedCalls( 0 )
	, _lastDereferenceOperator( OPERATOR::NONE )
	, _isAssert( false )
	, _isLambda( isLambda_ )
	, _isVariadic( false )
	, _capturesNamedParameters( false )
	, _compiler( compiler_ ) {
	_scopeStack.emplace( make_pointer<OScopeContext>( this, statementId_, fileId_, executing_parser::HRange( 0, 0 ) ) );
	return;
}

OCompiler::expressions_stack_t& OCompiler::OFunctionContext::expressions_stack( void ) {
	M_PROLOG
	return ( _scopeStack.top()->_expressionsStack );
	M_EPILOG
}

OCompiler::OClassContext::OClassContext( void )
	: _classIdentifier( IDENTIFIER::INVALID )
	, _baseName( IDENTIFIER::INVALID )
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
	int range_,
	bool shortCircuit_
) : _operation( operation_ )
	, _expression( expression_ )
	, _scope( scope_ )
	, _classId( classId_ )
	, _index( index_ )
	, _identifier( identifier_ )
	, _position( range_ )
	, _shortCircuit( shortCircuit_ ) {
	return;
}

OCompiler::OIdentifierUse::OIdentifierUse( void )
	: _type( HHuginn::SYMBOL_KIND::UNKNOWN )
	, _readCount( 0 )
	, _readPosition( 0 )
	, _writeCount( 0 )
	, _writePosition( 0 ) {
	return;
}

void OCompiler::OIdentifierUse::read( int range_, HHuginn::SYMBOL_KIND symbolKind_ ) {
	if ( _readCount == 0 ) {
		if ( symbolKind_ != HHuginn::SYMBOL_KIND::UNKNOWN ) {
			_type = symbolKind_;
		}
		_readPosition = range_;
	}
	++ _readCount;
	return;
}

void OCompiler::OIdentifierUse::write( int range_, HHuginn::SYMBOL_KIND symbolKind_ ) {
	if ( _writeCount == 0 ) {
		_type = symbolKind_;
		_writePosition = range_;
	}
	++ _writeCount;
	return;
}

OCompiler::HMultiPassDispatcher::HMultiPassDispatcher( OCompiler* compiler_ )
	: _compiler( compiler_ )
	, _pass( PASS::SYMBOL_RESOLVER ) {
	return;
}

void OCompiler::HMultiPassDispatcher::set_pass( PASS pass_ ) {
	_pass = pass_;
	return;
}

void OCompiler::HMultiPassDispatcher::set_class_name( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "` is a restricted name." ), MAIN_FILE_ID, range_.start() );
	}
	HHuginn::identifier_id_t identifier( _compiler->_runtime->identifier_id( name_ ) );
	if ( _pass == PASS::FINAL ) {
		_compiler->set_class_name( identifier, range_ );
	} else {
		_compiler->_classIdentifiers.push_back( identifier );
	}
	return;
	M_EPILOG
}

OCompiler::OImportInfo::OImportInfo( void )
	: _package( IDENTIFIER::INVALID )
	, _alias( IDENTIFIER::INVALID )
	, _importedSymbols()
	, _position( 0 ) {
	return;
}

OCompiler::OImportInfo::OImportInfo( OImportInfo&& other_ )
	: _package( IDENTIFIER::INVALID )
	, _alias( IDENTIFIER::INVALID )
	, _importedSymbols()
	, _position( 0 ) {
	swap( other_ );
	return;
}

void OCompiler::OImportInfo::swap( OImportInfo& other_ ) {
	using yaal::swap;
	swap( _package, other_._package );
	swap( _alias, other_._alias );
	swap( _importedSymbols, other_._importedSymbols );
	swap( _position, other_._position );
	return;
}

void OCompiler::OImportInfo::reset( void ) {
	_package = IDENTIFIER::INVALID;
	_alias = IDENTIFIER::INVALID;
	_importedSymbols.clear();
	_position = 0;
	return;
}

OCompiler::OCompiler( HRuntime* runtime_ )
	: _functionContexts()
	, _classContext()
	, _classIdentifiers()
	, _classNoter( this )
	, _submittedClasses()
	, _submittedEnums()
	, _submittedImports()
	, _moduleName()
	, _importInfo()
	, _executionStepsBacklog()
	, _usedIdentifiers()
	, _capturesLog()
	, _lambdaDefinitionSites()
	, _introspector( nullptr )
	, _statementIdGenerator( INVALID_STATEMENT_IDENTIFIER )
	, _functionContextCache()
	, _scopeContextCache()
	, _isModule( false )
	, _preloaded( false )
	, _fileId( INVALID_FILE_ID )
	, _isIncremental( false )
	, _mainExecutedStatementCount( 0 )
	, _mainCompiledStatementCount( 0 )
	, _runtime( runtime_ ) {
	return;
}

void OCompiler::reset( int undoSteps_ ) {
	M_PROLOG
	_mainCompiledStatementCount -= undoSteps_;
	_mainExecutedStatementCount = _mainCompiledStatementCount;
	_isIncremental = true;
	_fileId = INVALID_FILE_ID;
	_preloaded = false;
	_isModule = false;
	_scopeContextCache.clear();
	_functionContextCache.clear();
	_statementIdGenerator = INVALID_STATEMENT_IDENTIFIER;
	_introspector = nullptr;
	_lambdaDefinitionSites.clear();
	_capturesLog.clear();
	_usedIdentifiers.clear();
	_executionStepsBacklog.clear();
	_importInfo.reset();
	_moduleName.clear();
	_submittedImports.clear();
	_submittedEnums.clear();
	_submittedClasses.clear();
	_classNoter.set_pass( HMultiPassDispatcher::PASS::SYMBOL_RESOLVER );
	_classIdentifiers.clear();
	_classContext.reset();
	_functionContexts.clear();
	return;
	M_EPILOG
}

void OCompiler::set_setup( HIntrospectorInterface* introspector_, bool preloaded_ ) {
	_introspector = introspector_;
	_preloaded = preloaded_;
	++ _fileId;
	return;
}

void OCompiler::resolve_symbols( void ) {
	M_PROLOG
	M_ASSERT( _submittedClasses.is_empty() );
	HHuginn::identifier_id_t lastClassId( IDENTIFIER::INVALID );
	HHuginn::class_t aClass;
	int maxLocalVariableCount( 0 );
	typedef yaal::hcore::HArray<OScopeContext*> local_scope_stack_t;
	local_scope_stack_t localScopeStack;
	for ( OExecutionStep& es : _executionStepsBacklog ) {
		try {
			if ( es._classId != lastClassId ) {
				if ( es._classId != IDENTIFIER::INVALID ) {
					aClass = _runtime->get_class( es._classId );
					M_ASSERT( !! aClass );
				} else {
					aClass.reset();
				}
				lastClassId = es._classId;
			}
			if (
				find_if(
					_submittedImports.begin(),
					_submittedImports.end(),
					[&es]( OImportInfo const& info_ ) { return ( info_._package == es._identifier ); }
				) != _submittedImports.end()
			) {
				throw HHuginn::HHuginnRuntimeException( "Making a direct reference to a package is forbidden.", _fileId, es._position );
			}
			do {
				if ( !! aClass ) {
					int index( aClass->field_index( es._identifier ) );
					if ( index >= 0 ) {
						if ( ! es._expression ) {
							throw HHuginn::HHuginnRuntimeException(
								"Method argument name `"_ys
									.append( _runtime->identifier_name( es._identifier ) )
									.append( "` conflicts with class `" )
									.append( _runtime->identifier_name( aClass->identifier_id() ) )
									.append( "` field name." ),
								_fileId,
								es._position
							);
						}
						es._expression->replace_execution_step(
							es._index,
							HExpression::OExecutionStep(
								es._expression.raw(),
								es._operation == OExecutionStep::OPERATION::USE ? &HExpression::get_field_direct : &HExpression::get_field_ref_direct,
								es._position,
								es._operation == OExecutionStep::OPERATION::USE ? HFrame::ACCESS::VALUE : HFrame::ACCESS::REFERENCE,
								index
							)
						);
						break;
					}
				}
				captures_log_t::const_iterator cli( _capturesLog.find( es._scope->_functionContext->_functionIdentifier ) );
				if ( cli != _capturesLog.end() ) {
					HFunction::parameter_names_t::const_iterator ci( find( cli->second.begin(), cli->second.end(), es._identifier ) );
					if ( ci != cli->second.end() ) {
						es._expression->replace_execution_step(
							es._index,
							HExpression::OExecutionStep(
								es._expression.raw(),
								es._operation == OExecutionStep::OPERATION::USE ? &HExpression::get_field_direct : &HExpression::get_field_ref_direct,
								es._position,
								es._operation == OExecutionStep::OPERATION::USE ? HFrame::ACCESS::VALUE : HFrame::ACCESS::REFERENCE,
								static_cast<int>( distance( cli->second.begin(), ci ) )
							)
						);
						break;
					}
				}
				OScopeContext* sc( es._scope.raw() );
				OScopeContext::OLocalVariable localVariable;
				OScopeContext* varScope( sc );
				while ( varScope ) {
					OScopeContext::local_variables_t::const_iterator it( varScope->_localVariables.find( es._identifier ) );
					if ( it != varScope->_localVariables.end() ) {
						localVariable = it->second;
						break;
					}
					varScope = varScope->_parent;
				}
				if ( es._operation == OExecutionStep::OPERATION::DEFINE ) {
					if ( !! _runtime->get_class( es._identifier ) ) {
						throw HHuginn::HHuginnRuntimeException(
							"`"_ys.append( _runtime->identifier_name( es._identifier ) ).append( "` is a class name." ),
							_fileId,
							es._position
						);
					}
					bool make( ! localVariable._definedBy && ( localVariable._index == -1 ) );
					if ( make ) {
						if ( es._shortCircuit ) {
							throw HHuginn::HHuginnRuntimeException(
								"`"_ys.append( _runtime->identifier_name( es._identifier ) ).append( "` is defined in short-circuitable context." ),
								_fileId,
								es._position
							);
						}
						localScopeStack.clear();
						OScopeContext* base( sc );
						while ( base->_parent && ( base->_localVariableCount == OScopeContext::UNINITIALIZED_LOCAL_VARIABLE_COUNT ) ) {
							localScopeStack.push_back( base );
							base = base->_parent;
						}
						if ( base->_localVariableCount == OScopeContext::UNINITIALIZED_LOCAL_VARIABLE_COUNT ) {
							base->_localVariableCount = 0;
						}
						for ( OScopeContext* lsc : localScopeStack ) {
							lsc->_localVariableCount = base->_localVariableCount;
						}
						/*
						 * There are two kinds of OScopeContexts:
						 * 1. Real scope context that is equivalent of {...} in program code.
						 * 2. Virtual scope context that is necessary for proper scoping of `for` and `while` and `if`
						 *    control variables e.g.:
						 *        for ( e : list ) { ... }
						 *    in this case `e` is in virtual scope.
						 * Finding indices of local variables must take into account both types of scopes
						 * because real scopes `{}` can be inlined in virtual scopes in run-time,
						 * like it happens in case of `if/else` statement.
						 * Inlining `if/else` real scopes has a side effect of allowing different
						 * variables from different scopes (different `else if` stanzas in the same `if/else` chain)
						 * to have the same index, this is possible because `[else] if` stanzas are mutually exclusive
						 * in given `if/else` chain.
						 */
						M_ASSERT( localVariable._index == -1 );
						localVariable._index = sc->_localVariableCount;
						++ sc->_localVariableCount;
						localVariable._definedBy = es._expression.raw();
						sc->_localVariables[es._identifier] = localVariable;
						if ( sc->_localVariableCount > maxLocalVariableCount ) {
							maxLocalVariableCount = sc->_localVariableCount;
						}
					}
					if ( !! es._expression ) {
						if ( _introspector ) {
							es._expression->replace_execution_step(
								es._index,
								HExpression::OExecutionStep(
									static_cast<HIntroExpression*>( es._expression.raw() ),
									static_cast<HExpression::OExecutionStep::action_t>( make ? &HIntroExpression::make_variable : &HIntroExpression::get_variable_reference ),
									es._position,
									HFrame::ACCESS::REFERENCE,
									localVariable._index,
									es._identifier
								)
							);
						} else {
							es._expression->replace_execution_step(
								es._index,
								HExpression::OExecutionStep(
									es._expression.raw(),
									make ? &HExpression::make_variable : &HExpression::get_variable_reference,
									es._position,
									HFrame::ACCESS::REFERENCE,
									localVariable._index
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
				} else if ( localVariable._index >= 0 ) {
					if ( localVariable._definedBy && ( es._expression.raw() == localVariable._definedBy ) ) {
						throw HHuginn::HHuginnRuntimeException(
							"Symbol `"_ys.append( _runtime->identifier_name( es._identifier ) ).append( "` is not yet defined in this expression." ),
							_fileId,
							es._position
						);
					}
					es._expression->replace_execution_step(
						es._index,
						HExpression::OExecutionStep(
							es._expression.raw(),
							es._operation == OExecutionStep::OPERATION::USE ? &HExpression::get_variable_value : &HExpression::get_variable_reference,
							es._position,
							es._operation == OExecutionStep::OPERATION::USE ? HFrame::ACCESS::VALUE : HFrame::ACCESS::REFERENCE,
							localVariable._index
						)
					);
					break;
				}
				if ( ( es._operation == OExecutionStep::OPERATION::USE ) && ! is_keyword( _runtime->identifier_name( es._identifier ) ) ) {
					HHuginn::value_t const* v( _runtime->get_global( es._identifier ) );
					if ( !! v ) {
						es._expression->replace_execution_step(
							es._index,
							HExpression::OExecutionStep(
								es._expression.raw(),
								&HExpression::store_external_reference,
								es._position,
								v
							)
						);
						break;
					}
				}
				throw HHuginn::HHuginnRuntimeException(
					"Symbol `"_ys
						.append( _runtime->identifier_name( es._identifier ) )
						.append( "` is not defined in this context (did you mean `" )
						.append( _runtime->suggestion( es._identifier ) )
						.append( "`?)." ),
					_fileId,
					es._position
				);
			} while ( false );
		} catch ( ... ) {
			/* For incremental mode. */
			if ( es._classId != IDENTIFIER::INVALID ) {
				_runtime->drop_class( es._classId );
			} else {
				lambda_definition_sites_t::const_iterator it( _lambdaDefinitionSites.find( es._scope->_functionContext->_functionIdentifier ) );
				if ( it != _lambdaDefinitionSites.end() ) {
					_runtime->drop_global( it->second );
				} else {
					_runtime->drop_global( es._scope->_functionContext->_functionIdentifier );
				}
			}
			throw;
		}
	}
	_runtime->set_max_local_variable_count( maxLocalVariableCount );
	_scopeContextCache.clear();
	_functionContextCache.clear();
	_executionStepsBacklog.clear();
	return;
	M_EPILOG
}

void OCompiler::check_name_import( HHuginn::identifier_id_t identifier_, executing_parser::range_t range_ ) {
	M_PROLOG
	submitted_imports_t::const_iterator it(
		find_if(
			_submittedImports.begin(),
			_submittedImports.end(),
			[&identifier_]( OImportInfo const& info_ ) {
				return (
					( identifier_ == info_._package )
					|| ( identifier_ == info_._alias )
					|| ( find( info_._importedSymbols.begin(), info_._importedSymbols.end(), identifier_ ) != info_._importedSymbols.end() )
				);
			}
		)
	);
	if ( it != _submittedImports.end() ) {
		hcore::HString const& name( _runtime->identifier_name( identifier_ ) );
		throw HHuginn::HHuginnRuntimeException(
			identifier_ == it->_package
				? "Package of the same name `"_ys.append( name ).append( "` is already imported." )
				: (
					identifier_ == it->_alias
						? "Package alias of the same name `"_ys.append( name ).append( "` is already defined." )
						: "Symbol `"_ys.append( name ).append( "` was already brought into the global namespace." )
				),
			_fileId,
			range_.start()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::check_name_class( HHuginn::identifier_id_t identifier_, bool testRuntime_, executing_parser::range_t range_ ) {
	M_PROLOG
	HHuginn::class_t c( testRuntime_ ? _runtime->get_class( identifier_ ) : HHuginn::class_t() );
	if ( ( _submittedClasses.count( identifier_ ) > 0 ) || ( !! c && ! ( _isIncremental || _preloaded ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Class of the same name `"_ys
				.append( _runtime->identifier_name( identifier_ ) )
				.append( "` is already defined." ),
			_fileId,
			range_.start()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::check_name_enum( HHuginn::identifier_id_t identifier_, bool testRuntime_, executing_parser::range_t range_ ) {
	M_PROLOG
	HHuginn::value_t const* v( _runtime->get_global( identifier_ ) );
	if (
		( testRuntime_ && v && is_enum_class( v ) )
		|| (
			find_if(
				_submittedEnums.begin(),
				_submittedEnums.end(),
				[&identifier_]( HHuginn::value_t const& enum_ ) {
					return ( identifier_ == enum_->get_class()->identifier_id() );
				}
			) != _submittedEnums.end()
		)
	) {
		throw HHuginn::HHuginnRuntimeException(
			"Enumeration of the same name `"_ys
				.append( _runtime->identifier_name( identifier_ ) )
				.append( "` is already defined." ),
			_fileId,
			range_.start()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::check_name_function( HHuginn::identifier_id_t identifier_, executing_parser::range_t range_ ) {
	M_PROLOG
	HHuginn::value_t const* fun( _runtime->get_global( identifier_ ) );
	if ( fun && ( (*fun)->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Function `"_ys
				.append( _runtime->identifier_name( identifier_ ) )
				.append( "` was already defined." ),
			_fileId,
			range_.start()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::set_function_name( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	HHuginn::identifier_id_t functionIdentifier( _runtime->identifier_id( name_ ) );
	bool isCtorDtor( ( functionIdentifier == IDENTIFIER::KEYWORD::CONSTRUCTOR ) || ( functionIdentifier == IDENTIFIER::KEYWORD::DESTRUCTOR ) );
	if ( is_restricted( name_ ) ) {
		if ( ! _classContext || ! isCtorDtor ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "` is a restricted name." ), _fileId, range_.start() );
		}
	}
	if ( ! _classContext ) {
		if ( ! _isIncremental ) {
			check_name_function( functionIdentifier, range_ );
		}
		check_name_enum( functionIdentifier, true, range_ );
		check_name_class( functionIdentifier, true, range_ );
		check_name_import( functionIdentifier, range_ );
	}
	if ( ! isCtorDtor ) {
		_usedIdentifiers[functionIdentifier].write(
			range_.start(),
			! _classContext ? HHuginn::SYMBOL_KIND::FUNCTION : HHuginn::SYMBOL_KIND::METHOD
		);
	}
	_functionContexts.emplace( make_resource<OFunctionContext>( this, functionIdentifier, ++ _statementIdGenerator, _fileId, false ) );
	if ( !! _classContext ) {
		add_field_name( name_, range_ );
	}
	create_scope_impl( _isIncremental && ( functionIdentifier == IDENTIFIER::STANDARD_FUNCTIONS::MAIN ), range_ );
	if ( _introspector ) {
		_introspector->symbol(
			!! _classContext
				? HIntrospectorInterface::symbols_t{ _runtime->identifier_name( _classContext->_classIdentifier ), name_ }
				: HIntrospectorInterface::symbols_t{ name_ },
			!! _classContext ? HHuginn::SYMBOL_KIND::METHOD : HHuginn::SYMBOL_KIND::FUNCTION,
			_fileId,
			range_.start()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::commit_enum( executing_parser::range_t range_ ) {
	M_PROLOG
	_classContext->_doc = &_runtime->huginn()->get_comment( range_.start() );
	enumeration::descriptions_t ed;
	for ( int i( 0 ); i < static_cast<int>( _classContext->_fieldNames.get_size() ); ++ i ) {
		ed.emplace_back( _classContext->_fieldNames[i], _classContext->_docs.at( i ) );
	}
	HHuginn::class_t enumDefinition(
		enumeration::create_class(
			_runtime,
			_runtime->identifier_name( _classContext->_classIdentifier ),
			ed,
			_classContext->_doc ? *_classContext->_doc : "",
			HHuginn::VISIBILITY::GLOBAL
		)
	);
	_submittedEnums.emplace_back(
		_runtime->object_factory()->create<enumeration::HEnumeration>( enumDefinition.raw() )
	);
	_classContext.reset();
	return;
	M_EPILOG
}

void OCompiler::set_enum_name( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "` is a restricted name." ), _fileId, range_.start() );
	}
	HHuginn::identifier_id_t enumIdentifier( _runtime->identifier_id( name_ ) );
	check_name_enum( enumIdentifier, false, range_ );
	check_name_class( enumIdentifier, true, range_ );
	HHuginn::value_t const* v( _runtime->get_global( enumIdentifier ) );
	if ( ! ( v && is_enum_class( v ) ) ) {
		check_name_function( enumIdentifier, range_ );
	}
	check_name_import( enumIdentifier, range_ );
	_classContext = make_resource<OClassContext>();
	_usedIdentifiers[enumIdentifier].write( range_.start(), HHuginn::SYMBOL_KIND::ENUM );
	_classContext->_classIdentifier = enumIdentifier;
	_classContext->_position = range_.start();
	if ( _introspector ) {
		_introspector->symbol( { name_ }, HHuginn::SYMBOL_KIND::ENUM, _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::set_class_name( HHuginn::identifier_id_t identifier_, executing_parser::range_t range_ ) {
	M_PROLOG
	check_name_class( identifier_, true, range_ );
	check_name_enum( identifier_, true, range_ );
	if ( ! _runtime->get_class( identifier_ ) ) {
		check_name_function( identifier_, range_ );
	}
	check_name_import( identifier_, range_ );
	_classContext = make_resource<OClassContext>();
	_usedIdentifiers[identifier_].write( range_.start(), HHuginn::SYMBOL_KIND::CLASS );
	_functionContexts.emplace( make_resource<OFunctionContext>( this, identifier_, ++ _statementIdGenerator, _fileId, false ) );
	_classContext->_classIdentifier = identifier_;
	_classContext->_position = range_.start();
	if ( _introspector ) {
		_introspector->symbol( { _runtime->identifier_name( identifier_ ) }, HHuginn::SYMBOL_KIND::CLASS, _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::set_base_name( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( is_builtin( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "` is a restricted keyword." ), _fileId, range_.start() );
	}
	HHuginn::identifier_id_t baseClassIdentifier( _runtime->identifier_id( name_ ) );
	_usedIdentifiers[baseClassIdentifier].read( range_.start() );
	_classContext->_baseName = baseClassIdentifier;
	_classContext->_basePosition = range_.start();
	return;
	M_EPILOG
}

void OCompiler::add_field_name( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( find( _classContext->_fieldNames.begin(), _classContext->_fieldNames.end(), name_ ) != _classContext->_fieldNames.end() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Field `"_ys.append( name_ ).append( "` is already defined in `" ).append( _runtime->identifier_name( _classContext->_classIdentifier ) ).append( "`." ),
			_fileId,
			range_.start()
		);
	}
	_classContext->_fieldNames.push_back( name_ );
	hcore::HString const& doc( _runtime->huginn()->get_comment( range_.start() ) );
	int idx( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ) );
	_classContext->_docs.insert( make_pair( idx, doc ) );
	return;
	M_EPILOG
}

void OCompiler::set_field_name( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "` is a restricted name." ), _fileId, range_.start() );
	}
	HHuginn::identifier_id_t fieldIdentifier( _runtime->identifier_id( name_ ) );
	_usedIdentifiers[fieldIdentifier].write( range_.start(), HHuginn::SYMBOL_KIND::FIELD );
	add_field_name( name_, range_ );
	if ( _introspector ) {
		_introspector->symbol( { _runtime->identifier_name( _classContext->_classIdentifier ), name_ }, HHuginn::SYMBOL_KIND::FIELD, _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::create_function( executing_parser::range_t range_ ) {
	M_PROLOG
	hcore::HString const& doc( _runtime->huginn()->get_comment( range_.start() ) );
	function_info_t fi( create_function_low( range_ ) );
	if ( !! _classContext ) {
		int idx( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ) );
		_classContext->_methods.insert( make_pair( idx, fi.second ) );
		_classContext->_docs.insert( make_pair( idx, doc ) );
	} else {
		if ( !! _runtime->get_class( fi.first ) ) {
			throw HHuginn::HHuginnRuntimeException( "Class of the same name `"_ys.append( _runtime->identifier_name( fi.first ) ).append( "` is already defined." ), _fileId, range_.start() );
		}
		_runtime->register_function( fi.first, fi.second, doc );
	}
	M_EPILOG
}

OCompiler::function_info_t OCompiler::create_function_low( executing_parser::range_t ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	M_ASSERT( fc._functionIdentifier != IDENTIFIER::INVALID );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::scope_t scope( pop_scope_context() );
	bool isDestructor( fc._functionIdentifier == IDENTIFIER::KEYWORD::DESTRUCTOR );
	bool isIncrementalMain( _isIncremental && ( fc._functionIdentifier == IDENTIFIER::STANDARD_FUNCTIONS::MAIN ) && ! _classContext );
	M_ASSERT( dynamic_cast<HScope*>( scope.raw() ) );
	HScope* realScope( static_cast<HScope*>( scope.raw() ) );
	if ( isIncrementalMain ) {
		for ( int i( _mainExecutedStatementCount ); i > 0; -- i ) {
			realScope->remove_statement( i - 1 );
		}
		_mainCompiledStatementCount += realScope->statement_count();
	}
	realScope->finalize_function();
	HHuginn::function_t fun(
		_introspector
			? HHuginn::function_t(
				hcore::call(
					isIncrementalMain ? &HIntroFunction::execute_incremental_main : ( isDestructor ? &HFunction::execute_destructor : &HIntroFunction::execute ),
					make_pointer<HIntroFunction>(
						fc._functionIdentifier,
						fc._parameters,
						scope,
						fc._defaultValues,
						fc._isVariadic,
						fc._capturesNamedParameters
					),
					_1, _2, _3, _4
				)
			)
			: HHuginn::function_t(
				hcore::call(
					isIncrementalMain ? &HFunction::execute_incremental_main : ( isDestructor ? &HFunction::execute_destructor : &HFunction::execute ),
					make_pointer<HFunction>(
						fc._functionIdentifier,
						fc._parameters,
						scope,
						fc._defaultValues,
						fc._isVariadic,
						fc._capturesNamedParameters
					),
					_1, _2, _3, _4
				)
			)
	);
	M_ASSERT( fc._scopeStack.get_size() == 1 );
	function_info_t fi( fc._functionIdentifier, fun );
	pop_function_context();
	return fi;
	M_EPILOG
}

void OCompiler::submit_class( executing_parser::range_t range_ ) {
	M_PROLOG
	pop_function_context();
	_classContext->_doc = &_runtime->huginn()->get_comment( range_.start() );
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
	while ( cc->_baseName != IDENTIFIER::INVALID ) {
		submitted_classes_t::const_iterator it( _submittedClasses.find( cc->_baseName ) );
		if ( it == _submittedClasses.end() ) {
			if ( ! _runtime->get_class( cc->_baseName ) ) {
				throw HHuginn::HHuginnRuntimeException(
					"Base class `"_ys.append( _runtime->identifier_name( cc->_baseName ) ).append( "` was not defined." ),
					_fileId,
					cc->_basePosition
				);
			} else {
				break;
			}
		}
		hierarchy.push_back( cc->_baseName );
		if ( ! names.insert( cc->_baseName ).second ) {
			hcore::HString hier( _runtime->identifier_name( identifierId_ ) );
			for ( HHuginn::identifier_id_t n : hierarchy ) {
				hier.append( "->" );
				hier.append( _runtime->identifier_name( n ) );
			}
			throw HHuginn::HHuginnRuntimeException(
				"Class derivation cycle detected `"_ys.append( hier ).append( "`." ),
				_fileId,
				cc->_basePosition
			);
		}
		cc = it->second.get();
	}
	return;
	M_EPILOG
}

void OCompiler::verify_default_argument( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int lastDefaultValuePosition( fc._lastDefaultValuePosition );
	if ( ( lastDefaultValuePosition >= 0 ) && ( lastDefaultValuePosition < static_cast<int>( fc._parameters.get_size() - 1 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( "Missing default argument.", _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::pop_function_context( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( fc._loopCount == 0 );
	M_ASSERT( fc._loopSwitchCount == 0 );
	M_ASSERT( fc._shortCircuit == 0 );
	while ( ! fc._scopeStack.is_empty() ) {
		pop_scope_context_low();
	}
	_functionContextCache.push_back( yaal::move( _functionContexts.top() ) );
	_functionContexts.pop();
	return;
	M_EPILOG
}

HHuginn::scope_t OCompiler::pop_scope_context( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OScopeContext& sc( *fc._scopeStack.top() );
	HScope* scope( dynamic_cast<HScope*>( sc._scope.raw() ) );
	if (
		scope
		&& ( _runtime->compiler_setup() & HHuginn::COMPILER::BE_STRICT )
		&& ( sc._terminatedAt != NOT_TERMINATED )
		&& ( scope->statement_count() > ( sc._terminatedAt + 1 ) )
	) {
		throw HHuginn::HHuginnRuntimeException(
			"Statement is unreachable.",
			_fileId,
			scope->statement_position_at( sc._terminatedAt + 1 )
		);
	}
	HHuginn::scope_t virtScope( sc._scope );
	pop_scope_context_low();
	return virtScope;
	M_EPILOG
}

void OCompiler::pop_scope_context_low( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OFunctionContext::scope_context_t& childScope( fc._scopeStack.top() );
	M_ASSERT( childScope->_argumentIndexes.is_empty() );
	_scopeContextCache.push_back( yaal::move( childScope ) );
	fc._scopeStack.pop();
	return;
	M_EPILOG
}

void OCompiler::terminate_scope( HScope::statement_t&& statement_ ) {
	M_PROLOG
	OScopeContext& sc( current_scope_context() );
	int terminatedAt( sc.add_statement( yaal::move( statement_ ) ) );
	if ( sc._terminatedAt == NOT_TERMINATED ) {
		sc._terminatedAt = terminatedAt;
	}
	return;
	M_EPILOG
}

void OCompiler::commit_assignable( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	mark_expression_position( range_ );
	M_ASSERT( ! fc._assignmentVariables.is_empty() );
	if ( fc._assignmentVariables.top()._identifier != IDENTIFIER::INVALID ) {
		note_type( fc._assignmentVariables.top()._identifier, type_to_class( HHuginn::TYPE::UNKNOWN ) );
	}
	fc._assignmentVariables.clear();
	return;
	M_EPILOG
}

void OCompiler::save_control_variable( executing_parser::range_t range_ ) {
	M_PROLOG
	commit_assignable( range_ );
	OFunctionContext& fc( f() );
	fc.expressions_stack().top().emplace_back( new_expression( _fileId ) );
	M_EPILOG
}

void OCompiler::commit_catch_control_variable( executing_parser::range_t range_ ) {
	M_PROLOG
	commit_assignable( range_ );
	return;
	M_EPILOG
}

void OCompiler::add_default_value( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	fc._lastDefaultValuePosition = static_cast<int>( fc._parameters.get_size() - 1 );
	fc._defaultValues.push_back( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_field_definition( executing_parser::range_t ) {
	M_PROLOG
	M_ASSERT( !! _classContext );
	int idx( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ) );
	_classContext->_fieldDefinitions.insert( make_pair( idx, current_expression() ) );
	reset_expression();
	return;
	M_EPILOG
}

}

}

}

