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

HHuginn::identifier_id_t const INVALID_IDENTIFIER( -1 );
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
	int position_
) : _parent( ! functionContext_->_scopeStack.is_empty() ? functionContext_->_scopeStack.top().raw() : nullptr )
	, _scope( make_pointer<HScope>( statementId_, position_ ) )
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
	_expressionsStack.emplace( 1, make_pointer<HExpression>() );
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
	HHuginn::identifier_id_t functionId_,
	HStatement::statement_id_t statementId_,
	bool isLambda_
) : _functionIdentifier( functionId_ )
	, _parameters()
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
	, _inline( true ) {
	_scopeStack.emplace( make_pointer<OScopeContext>( this, statementId_, 0 ) );
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
	, _position( 0 )
	, _basePosition( 0 ) {
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

OCompiler::OCompiler( HHuginn* huginn_ )
	: _functionContexts()
	, _classContext()
	, _submittedClasses()
	, _submittedImports()
	, _importIdentifier( INVALID_IDENTIFIER )
	, _importAlias( INVALID_IDENTIFIER )
	, _executionStepsBacklog()
	, _usedIdentifiers()
	, _setup( HHuginn::COMPILER::BE_STRICT )
	, _statementIdGenerator( INVALID_STATEMENT_IDENTIFIER )
	, _scopeContextCache()
	, _huginn( huginn_ ) {
	return;
}

void OCompiler::set_setup( HHuginn::compiler_setup_t setup_ ) {
	_setup = setup_;
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
				aClass = _huginn->get_class( es._classId );
				M_ASSERT( !! aClass );
			} else {
				aClass.reset();
			}
			lastClassId = es._classId;
		}
		do {
			if ( !! aClass ) {
				int index( aClass->field_index( es._identifier ) );
				if ( index >= 0 ) {
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
			OScopeContext* sc( es._scope.raw() );
			HHuginn::identifier_id_t fi( sc->_functionId );
			int index( -1 );
			while ( sc ) {
				OScopeContext::local_variables_t::const_iterator it( sc->_variables.find( es._identifier ) );
				if ( it != sc->_variables.end() ) {
					index = it->second;
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
					 * There are two kind of OScopeContexts:
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
					M_ASSERT( index == -1 );
					index = static_cast<int>( sc->_variables.get_size() );
					if ( parent != sc ) {
						index += static_cast<int>( parent->_variables.get_size() );
					}
					sc->_variables[es._identifier] = index;
					if ( ( index + 1 ) > maxLocalVariableCount ) {
						maxLocalVariableCount = index + 1;
					}
				}
				if ( !! es._expression ) {
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::get_variable_direct,
							es._expression.raw(),
							HExpression::ACCESS::REFERENCE,
							sc->_statementId,
							index,
							_1,
							es._position
						)
					);
				}
				break;
			} else {
				if ( sc ) {
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::get_variable_direct,
							es._expression.raw(),
							HExpression::ACCESS::VALUE,
							sc->_statementId,
							index,
							_1,
							es._position
						)
					);
					break;
				}
			}
			if ( ( es._operation == OExecutionStep::OPERATION::USE ) && ! is_keyword( _huginn->identifier_name( es._identifier ) ) ) {
				HHuginn::function_t* callable( _huginn->get_function( es._identifier ) );
				if ( !! callable ) {
					es._expression->replace_execution_step(
						es._index,
						hcore::call(
							&HExpression::store_function,
							es._expression.raw(),
							es._identifier,
							callable,
							_1,
							es._position
						)
					);
					break;
				} else {
					HHuginn::value_t* p( _huginn->get_package( es._identifier ) );
					if ( !! p ) {
						es._expression->replace_execution_step(
							es._index,
							hcore::call(
								&HExpression::store_external_reference,
								es._expression.raw(),
								p,
								_1,
								es._position
							)
						);
						break;
					}
				}
			}
			throw HHuginn::HHuginnRuntimeException(
				"Symbol `"_ys.append( _huginn->identifier_name( es._identifier ) ).append( "' is not defined in this context." ), es._position
			);
		} while ( false );
	}
	_huginn->set_max_local_variable_count( maxLocalVariableCount );
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
		_huginn->identifier_id( "main" ),
		_huginn->identifier_id( "get_size" ),
		_huginn->identifier_id( "to_string" ),
		_huginn->identifier_id( "to_integer" ),
		_huginn->identifier_id( "to_real" ),
		_huginn->identifier_id( "to_number" ),
		_huginn->identifier_id( "to_character" ),
		_huginn->identifier_id( "to_boolean" )
	};
	HHuginn::identifier_id_t implicitDefinition[] = {
		KEYWORD::CONSTRUCTOR_IDENTIFIER,
		KEYWORD::THIS_IDENTIFIER,
		KEYWORD::SUPER_IDENTIFIER
	};
	for ( used_identifiers_t::value_type const& iu : _usedIdentifiers ) {
		HHuginn::identifier_id_t id( iu.first );
		OIdentifierUse const& use( iu.second );
		if ( use._readCount == 0 ) {
			if ( find( begin( implicitUse ), end( implicitUse ), id ) == end( implicitUse ) ) {
				throw HHuginn::HHuginnRuntimeException(
					use_name( use._type ).append( " `" ).append( _huginn->identifier_name( id ) ).append( "' is never used." ), use._writePosition
				);
			}
		} else if ( ( use._writeCount == 0 ) && ( use._type != OIdentifierUse::TYPE::VARIABLE ) && ( use._type != OIdentifierUse::TYPE::UNKNOWN ) ) {
			if ( find( begin( implicitDefinition ), end( implicitDefinition ), id ) == end( implicitDefinition ) ) {
				throw HHuginn::HHuginnRuntimeException(
					use_name( use._type ).append( " `" ).append( _huginn->identifier_name( id ) ).append( "' was never defined." ), use._readPosition
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

HHuginn::type_id_t OCompiler::guess_type( HHuginn::identifier_id_t identifierId_ ) {
	return ( f()._scopeStack.top()->guess_type( identifierId_ ) );
}

void OCompiler::note_type( HHuginn::identifier_id_t identifierId_, HHuginn::type_id_t type_ ) {
	f()._scopeStack.top()->note_type( identifierId_, type_ );
}

void OCompiler::set_function_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::identifier_id_t functionIdentifier( _huginn->identifier_id( name_ ) );
	bool isCtorDtor( ( functionIdentifier == KEYWORD::CONSTRUCTOR_IDENTIFIER ) || ( functionIdentifier == KEYWORD::DESTRUCTOR_IDENTIFIER ) );
	if ( is_restricted( name_ ) ) {
		if ( ! _classContext || ! isCtorDtor ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), position_.get() );
		}
	}
	if ( ! isCtorDtor ) {
		_usedIdentifiers[functionIdentifier].write(
			position_.get(),
			! _classContext ? OIdentifierUse::TYPE::FUNCTION : OIdentifierUse::TYPE::METHOD
		);
	}
	_functionContexts.emplace( make_resource<OFunctionContext>( functionIdentifier, ++ _statementIdGenerator, false ) );
	if ( !! _classContext ) {
		add_field_name( name_, position_ );
	}
	return;
	M_EPILOG
}

void OCompiler::set_import_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), position_.get() );
	}
	HHuginn::identifier_id_t importIdentifer( _huginn->identifier_id( name_ ) );
	if ( _submittedClasses.count( importIdentifer ) > 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Class `"_ys.append( name_ ).append( "' named is already defined." ), position_.get() );
	}
	if ( ! HPackageFactoryInstance::get_instance().is_type_valid( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "Package `"_ys.append( name_ ).append( "' does not exist." ), position_.get() );
	}
	_importIdentifier = importIdentifer;
	return;
	M_EPILOG
}

void OCompiler::set_import_alias( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), position_.get() );
	}
	HHuginn::identifier_id_t importAliasIdentifer( _huginn->identifier_id( name_ ) );
	if ( _submittedClasses.count( importAliasIdentifer ) > 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Class `"_ys.append( name_ ).append( "' named is already defined." ), position_.get() );
	}
	_importAlias = importAliasIdentifer;
	_usedIdentifiers[importAliasIdentifer].write( position_.get(), OIdentifierUse::TYPE::PACKAGE );
	return;
	M_EPILOG
}

void OCompiler::commit_import( executing_parser::position_t position_ ) {
	M_PROLOG
	for ( submitted_imports_t::value_type const& i : _submittedImports ) {
		if ( i.second == _importAlias ) {
			throw HHuginn::HHuginnRuntimeException(
				"Import alias `"_ys.append( _huginn->identifier_name( _importAlias ) ).append( "' is already defined." ),
				position_.get()
			);
		}
	}
	if ( ! _submittedImports.insert( make_pair( _importIdentifier, _importAlias ) ).second ) {
		throw HHuginn::HHuginnRuntimeException(
			"Package `"_ys.append( _huginn->identifier_name( _importIdentifier ) ).append( "' was already imported." ),
			position_.get()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::set_class_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	_classContext = make_resource<OClassContext>();
	HHuginn::identifier_id_t classIdentifer( _huginn->identifier_id( name_ ) );
	_usedIdentifiers[classIdentifer].write( position_.get(), OIdentifierUse::TYPE::CLASS );
	if ( is_restricted( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), position_.get() );
	}
	if ( _submittedClasses.count( classIdentifer ) > 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Class `"_ys.append( name_ ).append( "' is already defined." ), position_.get() );
	}
	_functionContexts.emplace( make_resource<OFunctionContext>( classIdentifer, ++ _statementIdGenerator, false ) );
	_classContext->_classIdentifier = classIdentifer;
	_classContext->_position = position_;
	return;
	M_EPILOG
}

void OCompiler::set_base_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( is_builtin( name_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted keyword." ), position_.get() );
	}
	HHuginn::identifier_id_t baseClassIdentifer( _huginn->identifier_id( name_ ) );
	_usedIdentifiers[baseClassIdentifer].read( position_.get() );
	_classContext->_baseName = baseClassIdentifer;
	_classContext->_basePosition = position_;
	return;
	M_EPILOG
}

void OCompiler::add_field_name( yaal::hcore::HString const& name_, executing_parser::position_t position_ ) {
	M_PROLOG
	if ( find( _classContext->_fieldNames.begin(), _classContext->_fieldNames.end(), name_ ) != _classContext->_fieldNames.end() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Field `"_ys.append( name_ ).append( "' is already defined in `" ).append( _huginn->identifier_name( _classContext->_classIdentifier ) ).append( "'." ),
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
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( name_ ).append( "' is a restricted name." ), position_.get() );
	}
	HHuginn::identifier_id_t fieldIdentifer( _huginn->identifier_id( name_ ) );
	_usedIdentifiers[fieldIdentifer].write( position_.get(), OIdentifierUse::TYPE::FIELD );
	add_field_name( name_, position_ );
	return;
	M_EPILOG
}

void OCompiler::set_lambda_name( executing_parser::position_t position_ ) {
	M_PROLOG
	HHuginn::HErrorCoordinate ec( _huginn->get_coordinate( position_.get() ) );
	HHuginn::identifier_id_t id( _huginn->identifier_id( to_string( "@" ).append( ec.line() ).append( ":" ).append( ec.column() ) ) );
	_functionContexts.emplace( make_resource<OFunctionContext>( id, ++ _statementIdGenerator, true ) );
	return;
	M_EPILOG
}

void OCompiler::create_function( executing_parser::position_t position_ ) {
	M_PROLOG
	function_info_t fi( create_function_low( position_ ) );
	if ( !! _classContext ) {
		_classContext->_methods.insert( make_pair( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ), fi.second ) );
	} else {
		_huginn->register_function( fi.first, fi.second );
	}
	M_EPILOG
}

OCompiler::function_info_t OCompiler::create_function_low( executing_parser::position_t ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	M_ASSERT( fc._functionIdentifier != INVALID_IDENTIFIER );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	HHuginn::function_t fun(
		hcore::call(
			&HFunction::execute,
			make_pointer<HFunction>(
				fc._functionIdentifier,
				static_cast<int>( fc._parameters.get_size() ),
				pop_scope_context(),
				fc._defaultValues
			),
			_1, _2, _3, _4
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
	if ( ! _submittedClasses.insert( make_pair( _classContext->_classIdentifier, yaal::move( _classContext ) ) ).second ) {
		throw HHuginn::HHuginnRuntimeException(
			"`"_ys.append( _huginn->identifier_name( _classContext->_classIdentifier ) ).append( "' is already defined." ),
			position_.get()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::track_name_cycle( HHuginn::identifier_id_t identifierId_ ) {
	M_PROLOG
	typedef yaal::hcore::HHashSet<HHuginn::identifier_id_t> names_t;
	typedef yaal::hcore::HArray<HHuginn::identifier_id_t> hierarhy_t;
	names_t names;
	hierarhy_t hierarhy;
	OClassContext const* cc( _submittedClasses.at( identifierId_ ).get() );
	names.insert( identifierId_ );
	while ( cc->_baseName != INVALID_IDENTIFIER ) {
		submitted_classes_t::const_iterator it( _submittedClasses.find( cc->_baseName ) );
		if ( it == _submittedClasses.end() ) {
			throw HHuginn::HHuginnRuntimeException(
				"Base class `"_ys.append( _huginn->identifier_name( cc->_baseName ) ).append( "' was not defined." ),
				cc->_basePosition.get()
			);
		}
		hierarhy.push_back( cc->_baseName );
		if ( ! names.insert( cc->_baseName ).second ) {
			hcore::HString hier( _huginn->identifier_name( identifierId_ ) );
			for ( HHuginn::identifier_id_t n : hierarhy ) {
				hier.append( "->" );
				hier.append( _huginn->identifier_name( n ) );
			}
			throw HHuginn::HHuginnRuntimeException( "Class derivation cycle detected `"_ys.append( hier ).append( "'." ), cc->_basePosition.get() );
		}
		cc = it->second.get();
	}
	return;
	M_EPILOG
}

void OCompiler::create_lambda( executing_parser::position_t position_ ) {
	M_PROLOG
	function_info_t fi( create_function_low( position_ ) );
	HHuginn::value_t fRef( make_pointer<HHuginn::HFunctionReference>( fi.first, fi.second ) );
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
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t parameterIdentifer( _huginn->identifier_id( name_ ) );
	if ( find( fc._parameters.begin(), fc._parameters.end(), parameterIdentifer ) != fc._parameters.end() ) {
		throw HHuginn::HHuginnRuntimeException( "Parameter `"_ys.append( name_ ).append( "' was already defined." ), position_.get() );
	}
	verify_default_argument( position_ );
	_usedIdentifiers[parameterIdentifer].write( position_.get(), OIdentifierUse::TYPE::VARIABLE );
	_executionStepsBacklog.emplace_back(
		OExecutionStep::OPERATION::DEFINE,
		HHuginn::expression_t(),
		fc._scopeStack.top(),
		!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : INVALID_IDENTIFIER,
		-1,
		parameterIdentifer,
		position_.get()
	);
	fc._parameters.push_back( parameterIdentifer );
	return;
	M_EPILOG
}

void OCompiler::create_scope( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( ! fc._inline ) {
		++ _statementIdGenerator;
	}
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, _statementIdGenerator, position_.get() ) );
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
	current_expression() = make_pointer<HExpression>();
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
		fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, position_.get() ) );
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
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::start_switch_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	++ fc._loopSwitchCount;
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::start_subexpression( executing_parser::position_t position_ ) {
	M_PROLOG
	f().expressions_stack().emplace( 1, make_pointer<HExpression>( position_.get() ) );
	return;
	M_EPILOG
}

void OCompiler::add_subexpression( OPERATOR op_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc.expressions_stack().top().emplace_back( make_pointer<HExpression>( position_.get() ) );
	fc._operations.emplace( op_, position_.get() );
	return;
	M_EPILOG
}

void OCompiler::commit_boolean( OPERATOR operator_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc.expressions_stack().top().get_size() > 1 ) {
		HHuginn::value_t And( make_pointer<HBooleanEvaluator>( _huginn, fc.expressions_stack().top(), operator_ ) );
		fc.expressions_stack().pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top() == HHuginn::TYPE::BOOLEAN );
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
		HHuginn::value_t ternary( make_pointer<HHuginn::HTernaryEvaluator>( exprs[0], exprs[1], exprs[2] ) );
		fc.expressions_stack().pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && fc._valueTypes.top() == HHuginn::TYPE::UNKNOWN );
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
		e->add_execution_step( hcore::call( &HExpression::store_direct, e.raw(), _huginn->none_value(), _1, position_.get() ) );
	}
	terminate_scope( make_pointer<HReturn>( e, position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_throw_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	terminate_scope( make_pointer<HThrow>( _huginn, current_expression(), position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_break_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	if ( fc._loopSwitchCount == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid context for `break' statement.", position_.get() );
	}
	terminate_scope( make_pointer<HBreak>( HFrame::STATE::BREAK, position_.get() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::add_continue_statement( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._scopeStack.is_empty() );
	if ( fc._loopCount == 0 ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid context for `continue' statement.", position_.get() );
	}
	terminate_scope( make_pointer<HBreak>( HFrame::STATE::CONTINUE, position_.get() ) );
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
		make_pointer<HWhile>( sc._statementId, current_expression(), scope, position_.get() )
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

void OCompiler::save_control_variable( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	current_expression()->set_position( position_.get() );
	fc._variables.clear();
	fc.expressions_stack().top().emplace_back( make_pointer<HExpression>() );
	return;
	M_EPILOG
}

void OCompiler::commit_catch_control_variable( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	mark_expression_position( position_ );
	fc._variables.clear();
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
	fc._scopeStack.emplace( make_pointer<OScopeContext>( &fc, ++ _statementIdGenerator, position_.get() ) );
	OScopeContext& sc( current_scope_context() );
	sc._exceptionType = _huginn->identifier_id( name_ );
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
		make_pointer<HFor>( sc._statementId, current_expression(), source, scope, position_.get() )
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
	HScope::statement_t ifStatement( make_pointer<HIf>( sc._statementId, sc._scopeChain, sc._else, position_.get() ) );
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
	HStatement::statement_id_t statementId( fc._scopeStack.top()->_statementId );
	HHuginn::scope_t scope( pop_scope_context() );
	HScope::statement_t tryCatchStatement(
		make_pointer<HTryCatch>( statementId, scope, catches, position_.get() )
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

void OCompiler::add_field_definition( executing_parser::position_t ) {
	M_PROLOG
	M_ASSERT( !! _classContext );
	_classContext->_fieldDefinitions.insert( make_pair( static_cast<int>( _classContext->_fieldNames.get_size() - 1 ), current_expression() ) );
	reset_expression();
	return;
	M_EPILOG
}

void OCompiler::mark_expression_position( executing_parser::position_t position_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
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

bool OCompiler::is_comparable( HHuginn::type_id_t type_ ) {
	return (
		is_numeric( type_ )
		|| ( type_ == HHuginn::TYPE::STRING )
		|| ( type_ == HHuginn::TYPE::CHARACTER )
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
	return ( is_numeric_congruent( type_ ) || ( type_ == HHuginn::TYPE::STRING ) );
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
	HHuginn::type_id_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( op_to_str( o ), t2, t1, position_.get() );
	}
	if ( ! ( is_summable( t1 ) && is_summable( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			HString( o == OPERATOR::PLUS ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB] )
			.append( type_name( t2 ) )
			.append( ", " )
			.append( type_name( t1 ) ),
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
	HHuginn::type_id_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( op_to_str( o ), t2, t1, position_.get() );
	}
	if ( ! ( is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			HString( o == OPERATOR::MULTIPLY ? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV] )
			.append( type_name( t2 ) )
			.append( ", " )
			.append( type_name( t1 ) ),
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
		HHuginn::type_id_t t1( fc._valueTypes.top() );
		fc._valueTypes.pop();
		HHuginn::type_id_t t2( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( ! are_congruous( t1, t2 ) ) {
			operands_type_mismatch( op_to_str( OPERATOR::POWER ), t2, t1, p );
		}
		if ( ! ( is_numeric_congruent( t1 ) && is_numeric_congruent( t2 ) ) ) {
			throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP], p );
		}
		fc._valueTypes.push( congruent( t1, t2 ) );
		hasPower = true;
	}
	if ( hasPower ) {
		defer_action( &HExpression::power, position_ );
		current_expression()->commit_oper( OPERATOR::POWER );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_factorial( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._valueTypes.is_empty() );
	defer_action( &HExpression::factorial, position_ );
	if ( ! is_numeric_congruent( fc._valueTypes.top() ) ) {
		throw HHuginn::HHuginnRuntimeException( "Operand is not a numeric value.", position_.get() );
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
	if ( ! is_numeric_congruent( fc._valueTypes.top() ) ) {
		throw HHuginn::HHuginnRuntimeException( "Operand is not a numeric value.", p );
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
	HHuginn::type_id_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( os, t2, t1, p );
	}
	if ( ! ( is_comparable_congruent( t1 ) && is_comparable_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_CMP], p );
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
	HHuginn::type_id_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! ( is_boolean_congruent( t1 ) && is_boolean_congruent( t2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL], p );
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
	HHuginn::type_id_t t0( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! is_boolean_congruent( t0 ) ) {
		throw HHuginn::HHuginnRuntimeException( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL], p );
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
	HHuginn::type_id_t t1( fc._valueTypes.top() );
	fc._valueTypes.pop();
	HHuginn::type_id_t t2( fc._valueTypes.top() );
	fc._valueTypes.pop();
	if ( ! are_congruous( t1, t2 ) ) {
		operands_type_mismatch( op_to_str( o ), t2, t1, p );
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
		HHuginn::type_id_t t1( fc._valueTypes.top() );
		fc._valueTypes.pop();
		HHuginn::type_id_t t2( fc._valueTypes.top() );
		fc._valueTypes.pop();
		HHuginn::identifier_id_t name( fc._variables.top() );
		HHuginn::type_id_t realDestType( t2 );
		if ( realDestType == HHuginn::TYPE::UNKNOWN ) {
			realDestType = guess_type( name );
		}
		if ( name != INVALID_IDENTIFIER ) {
			note_type( name, t1 );
		}
		if ( o == OPERATOR::ASSIGN ) {
			if ( name != INVALID_IDENTIFIER ) {
				_usedIdentifiers[name].write( position_.get(), OIdentifierUse::TYPE::VARIABLE );
			}
		} else if ( are_congruous( t1, realDestType ) ) {
			_usedIdentifiers[name].read( position_.get(), OIdentifierUse::TYPE::VARIABLE );
		} else {
			operands_type_mismatch( op_to_str( o ), realDestType, t1, p );
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
			throw HHuginn::HHuginnRuntimeException( "Setting a non reference location.", p );
		}
		fc._valueTypes.push( congruent( t1, t2 ) );
		hasAssign = true;
	}
	if ( hasAssign ) {
		defer_action( &HExpression::set_variable, position_ );
		current_expression()->commit_oper( OPERATOR::ASSIGN );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_subscript( executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int p( position_.get() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		throw HHuginn::HHuginnRuntimeException( "`assert' is a restricted keyword.", p );
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
			if ( ! ( nonInteger || is_integer_congruent( fc._valueTypes.top() ) || ( fc._valueTypes.top() == HHuginn::TYPE::NONE ) ) ) {
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
			throw HHuginn::HHuginnRuntimeException( "Range specifier is not an integer.", p );
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
				_huginn->object_factory()->create_string( _huginn->get_snippet( from, len ).trim() ),
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
	if ( action_ == &HExpression::function_call ) {
		fc._valueTypes.pop();
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::UNKNOWN ) );
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
		throw HHuginn::HHuginnRuntimeException( "`assert' is a restricted keyword.", position_.get() );
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
			current_expression()->commit_oper( o );
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
	OFunctionContext& fc( f() );
	if ( ( fc._lastDereferenceOperator != OPERATOR::SUBSCRIPT ) && ( fc._lastDereferenceOperator != OPERATOR::MEMBER_ACCESS ) ) {
		throw HHuginn::HHuginnRuntimeException( "Assignment to function result.", position_.get() );
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
			hcore::call( &HExpression::get_field, current_expression().raw(), HExpression::ACCESS::REFERENCE, fc._lastMemberName, _1, position_.get() )
		);
	}
	fc._variables.push( INVALID_IDENTIFIER );
	return;
	M_EPILOG
}

void OCompiler::defer_get_reference( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t refIdentifier( _huginn->identifier_id( value_ ) );
	_usedIdentifiers[refIdentifier].read( position_.get() );
	bool keyword( false );
	bool isAssert( refIdentifier == KEYWORD::ASSERT_IDENTIFIER );
	if ( ( keyword = huginn::is_keyword( value_ ) ) ) {
		if ( isAssert ) {
			fc._isAssert = isAssert;
		}
		if ( ( ( value_ != KEYWORD::THIS ) && ( value_ != KEYWORD::SUPER ) && ! isAssert ) || ( isAssert && ! current_expression()->is_empty() ) ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "' is a restricted keyword." ), position_.get() );
		} else if ( ! isAssert && ! _classContext ) {
			throw HHuginn::HHuginnRuntimeException( "Keyword `"_ys.append( value_ ).append( "' can be used only in class context." ), position_.get() );
		}
	}
	HHuginn::expression_t& expression( current_expression() );
	if ( ( ! keyword || isAssert ) && huginn::is_builtin( value_ ) ) {
		/*
		 * We can do it here (as opposed to *::resolve_symbols()) because built-ins must exist,
		 * hence h->get_function() always succeeds, and built-ins cannot be overriden
		 * so they meaning stays always the same.
		 */
		current_expression()->add_execution_step(
			hcore::call(
				&HExpression::store_direct,
				current_expression().raw(),
				make_pointer<HHuginn::HFunctionReference>( refIdentifier, *_huginn->get_function( refIdentifier ) ),
				_1,
				position_.get()
			)
		);
	} else if ( refIdentifier == KEYWORD::THIS_IDENTIFIER ) {
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
	fc._valueTypes.push( guess_type( refIdentifier ) );
	return;
	M_EPILOG
}

void OCompiler::defer_get_field_reference( yaal::hcore::HString const& value_, executing_parser::position_t position_ ) {
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t refIdentifier( _huginn->identifier_id( value_ ) );
	_usedIdentifiers[refIdentifier].read( position_.get(), OIdentifierUse::TYPE::FIELD );
	if ( huginn::is_keyword( value_ ) ) {
		if ( refIdentifier != KEYWORD::CONSTRUCTOR_IDENTIFIER ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "' is a restricted keyword." ), position_.get() );
		} else if ( ! _classContext ) {
			throw HHuginn::HHuginnRuntimeException( "Keyword `"_ys.append( value_ ).append( "' can be used only in class context." ), position_.get() );
		}
	}
	HExpression& expression( *current_expression() );
	expression.add_execution_step(
		hcore::call( &HExpression::get_field, current_expression().raw(), HExpression::ACCESS::VALUE, refIdentifier, _1, position_.get() )
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
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "' is a restricted name." ), position_.get() );
	}
	HHuginn::identifier_id_t varIdentifier( _huginn->identifier_id( value_ ) );
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
	fc._variables.push( varIdentifier );
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
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _huginn->object_factory()->create_real( value_ ), _1, position_.get() ) );
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
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _huginn->object_factory()->create_integer( value_ ), _1, position_.get() ) );
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
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _huginn->object_factory()->create_string( value_ ), _1, position_.get() ) );
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
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _huginn->object_factory()->create_number( value_ ), _1, position_.get() ) );
	} else {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_number, current_expression().raw(), value_, _1, position_.get() ) );
	}
	fc._valueTypes.push( type_id( HHuginn::TYPE::NUMBER ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_character( char value_, executing_parser::position_t position_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		current_expression()->add_execution_step( hcore::call( &HExpression::store_direct, current_expression().raw(), _huginn->object_factory()->create_character( value_ ), _1, position_.get() ) );
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

