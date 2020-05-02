/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/huginn/compiler.hxx"
#include "tools/huginn/booleanevaluator.hxx"
#include "tools/huginn/ternaryevaluator.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

namespace {

OPERATOR _copyConstContext_[] = {
	OPERATOR::ASSIGN,
	OPERATOR::FUNCTION_CALL,
	OPERATOR::FUNCTION_ARGUMENT,
	OPERATOR::SUBSCRIPT,
	OPERATOR::TERNARY
};

inline HHuginn::TYPE compiled_type_id( HClass const* class_ ) {
	return ( class_ ? type_tag( class_->type_id() ) : HHuginn::TYPE::UNKNOWN );
}

}

void OCompiler::dispatch_action( OPERATOR oper_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._lastDereferenceOperator = OPERATOR::NONE;
	OPositionedOperator po( ! fc._operations.is_empty() ? fc._operations.top() : OPositionedOperator( OPERATOR::NONE, 0 ) );
	OPERATOR o( po._operator );
	int p( po._position );
	switch ( oper_ ) {
		case ( OPERATOR::PLUS ):          { dispatch_plus( range_ );          } break;
		case ( OPERATOR::MULTIPLY ):      { dispatch_mul( range_ );           } break;
		case ( OPERATOR::POWER ):         { dispatch_power( range_ );         } break;
		case ( OPERATOR::FACTORIAL ):     { dispatch_factorial( range_ );     } break;
		case ( OPERATOR::NEGATE ):        { dispatch_negate( range_ );        } break;
		case ( OPERATOR::SUBSCRIPT ):     { dispatch_subscript( range_ );     } break;
		case ( OPERATOR::ASSIGN ):        { dispatch_assign( range_ );        } break;
		case ( OPERATOR::MEMBER_ACCESS ): { dispatch_member_access( range_ ); } break;
		case ( OPERATOR::FUNCTION_CALL ): { dispatch_function_call( &HExpression::function_call, range_ ); } break;
		case ( OPERATOR::MAKE_PARTIAL ):  { dispatch_function_call( &HExpression::make_partial, range_ ); }  break;
		case ( OPERATOR::MAKE_DICT ):     { dispatch_function_call( &HExpression::make_dict, range_ ); }     break;
		case ( OPERATOR::MAKE_LOOKUP ):   { dispatch_function_call( &HExpression::make_lookup, range_ ); }   break;
		case ( OPERATOR::PARENTHESIS ):
		case ( OPERATOR::MODULUS ): {
			M_ASSERT( ( o == OPERATOR::MODULUS ) || ( o == OPERATOR::PARENTHESIS ) );
			defer_action( &HExpression::close_parenthesis, range_ );
			current_expression()->commit_oper( o, _fileId, p );
			fc._operations.pop();
			if ( o == OPERATOR::MODULUS ) {
				M_ASSERT( ! fc._valueTypes.is_empty() );
				if ( ! is_numeric_congruent( fc._valueTypes.top()._class ) ) {
					throw HHuginn::HHuginnRuntimeException(
						hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_NUM] )
							.append( a_type_name( fc._valueTypes.top()._class ) ),
						_fileId,
						p
					);
				}
			}
		} break;
		case ( OPERATOR::EQUALS ): { dispatch_equals( range_ );  } break;
		case ( OPERATOR::LESS ):   { dispatch_compare( range_ ); } break;
		case ( OPERATOR::BOOLEAN_AND ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_AND );
			dispatch_boolean( nullptr, range_ );
		} break;
		case ( OPERATOR::BOOLEAN_OR ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_OR );
			dispatch_boolean( nullptr, range_ );
		} break;
		case ( OPERATOR::BOOLEAN_XOR ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_XOR );
			dispatch_boolean( &HExpression::boolean_xor, range_ );
		} break;
		case ( OPERATOR::BOOLEAN_NOT ): {
			M_ASSERT( o == OPERATOR::BOOLEAN_NOT );
			M_ASSERT( ! fc._valueTypes.is_empty() );
			defer_action( &HExpression::boolean_not, range_ );
			current_expression()->commit_oper( o, _fileId, p );
			fc._operations.pop();
			HClass const* c( fc._valueTypes.top()._class );
			if ( ! is_boolean_congruent( c ) ) {
				throw HHuginn::HHuginnRuntimeException(
					hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( a_type_name( c ) ),
					_fileId,
					p
				);
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

void OCompiler::defer_oper( code_point_t operator_, executing_parser::range_t range_ ) {
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
		case ( '|' ): o = OPERATOR::MODULUS;    break;
		case ( '!' ): o = OPERATOR::BOOLEAN_NOT; break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->oper( o, range_.start() );
	f()._operations.emplace( o, range_.start() );
	return;
	M_EPILOG
}

void OCompiler::defer_str_oper( yaal::hcore::HString const& operator_, executing_parser::range_t range_ ) {
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
		{ op_to_str( OPERATOR::IS_ELEMENT_OF ),    OPERATOR::IS_ELEMENT_OF },
		{ op_to_str( OPERATOR::IS_NOT_ELEMENT_OF ),OPERATOR::IS_NOT_ELEMENT_OF },
		{ op_to_str( OPERATOR::BOOLEAN_AND ),      OPERATOR::BOOLEAN_AND },
		{ op_to_str( OPERATOR::BOOLEAN_OR ),       OPERATOR::BOOLEAN_OR },
		{ op_to_str( OPERATOR::BOOLEAN_XOR ),      OPERATOR::BOOLEAN_XOR },
		{ op_to_str( OPERATOR::LESS ),             OPERATOR::LESS },
		{ op_to_str( OPERATOR::GREATER ),          OPERATOR::GREATER },
		{ op_to_symbol_str( OPERATOR::NOT_EQUALS ),       OPERATOR::NOT_EQUALS },
		{ op_to_symbol_str( OPERATOR::LESS_OR_EQUAL ),    OPERATOR::LESS_OR_EQUAL },
		{ op_to_symbol_str( OPERATOR::GREATER_OR_EQUAL ), OPERATOR::GREATER_OR_EQUAL },
		{ op_to_symbol_str( OPERATOR::BOOLEAN_AND ), OPERATOR::BOOLEAN_AND },
		{ op_to_symbol_str( OPERATOR::BOOLEAN_OR ),  OPERATOR::BOOLEAN_OR },
		{ op_to_symbol_str( OPERATOR::BOOLEAN_XOR ), OPERATOR::BOOLEAN_XOR }
	} );
	OFunctionContext& fc( f() );
	OPERATOR o( operatorLookup.at( operator_ ) );
	if ( ( o == OPERATOR::ASSIGN ) && ( fc._variableCount.top() > 1 ) ) {
		o = OPERATOR::ASSIGN_PACK;
	}
	current_expression()->oper( o, range_.start() );
	fc._operations.emplace( o, range_.start() );
	return;
	M_EPILOG
}

void OCompiler::defer_oper_direct( OPERATOR operator_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::expression_t& expression( current_expression() );
	expression->oper( operator_, range_.start() );
	fc._operations.emplace( operator_, range_.start() );
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
		sc._assertExpressionEnd = range_.start();
	}
	return;
	M_EPILOG
}

void OCompiler::make_reference( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( ( fc._lastDereferenceOperator != OPERATOR::SUBSCRIPT ) && ( fc._lastDereferenceOperator != OPERATOR::MEMBER_ACCESS ) ) {
		throw HHuginn::HHuginnRuntimeException(
			fc._lastDereferenceOperator == OPERATOR::RANGE ? "Assignment to slice view." : "Assignment to function result.",
			_fileId,
			range_.start()
		);
	}
	HExpression* expr( current_expression().raw() );
	if ( fc._lastDereferenceOperator == OPERATOR::SUBSCRIPT ) {
		expr->pop_execution_step();
		expr->add_execution_step(
			HExpression::OExecutionStep( expr, &HExpression::subscript, range_.start(), HFrame::ACCESS::REFERENCE )
		);
	} else {
		expr->execution_step( expr->execution_step_count() - 1 )._access = HFrame::ACCESS::REFERENCE;
	}
	fc._valueTypes.pop();
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::REFERENCE ) );
	fc._variables.emplace( IDENTIFIER::INVALID, -1 );
	return;
	M_EPILOG
}

void OCompiler::defer_get_reference( yaal::hcore::HString const& value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t refIdentifier( _runtime->identifier_id( value_ ) );
	_usedIdentifiers[refIdentifier].read( range_.start() );
	bool keyword( false );
	bool isAssert( refIdentifier == IDENTIFIER::KEYWORD::ASSERT );
	bool isFieldDefinition( !! _classContext && ( _functionContexts.get_size() == 1 ) );
	HHuginn::expression_t& expression( current_expression() );
	HExpression* expr( expression.raw() );
	if ( ( keyword = huginn::is_keyword( value_ ) ) ) {
		if ( isAssert ) {
			fc._isAssert = isAssert;
		}
		if ( ( ( value_ != KEYWORD::THIS ) && ( value_ != KEYWORD::SUPER ) && ! isAssert ) || ( isAssert && ! expr->is_empty() ) ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "` is a restricted keyword." ), _fileId, range_.start() );
		} else if ( ! isAssert && ! _classContext ) {
			throw HHuginn::HHuginnRuntimeException( "Keyword `"_ys.append( value_ ).append( "` can be used only in class context." ), _fileId, range_.start() );
		}
	}
	if ( _isIncremental && ( refIdentifier == IDENTIFIER::STANDARD_FUNCTIONS::MAIN ) ) {
		throw HHuginn::HHuginnRuntimeException( "Referencing main() function in incremental mode is forbidden.", _fileId, range_.start() );
	}
	if ( ( ! keyword || isAssert ) && huginn::is_builtin( value_ ) ) {
		/*
		 * We can do it here (as opposed to *::resolve_symbols()) because built-ins must exist,
		 * hence h->get_function() always succeeds, and built-ins cannot be overridden
		 * so their meaning stays always the same.
		 */
		expr->add_execution_step(
			HExpression::OExecutionStep(
				expr,
				&HExpression::store_direct,
				range_.start(),
				*_runtime->get_global( refIdentifier )
			)
		);
	} else {
		if ( isFieldDefinition ) {
			throw HHuginn::HHuginnRuntimeException( "Dereferencing symbol `"_ys.append( value_ ).append( "` in field definition is forbidden." ), _fileId, range_.start() );
		}
		if ( refIdentifier == IDENTIFIER::KEYWORD::THIS ) {
			expr->add_execution_step(
				HExpression::OExecutionStep( expr, &HExpression::get_this, range_.start() )
			);
		} else if ( refIdentifier == IDENTIFIER::KEYWORD::SUPER ) {
			expr->add_execution_step(
				HExpression::OExecutionStep( expr, &HExpression::get_super, range_.start() )
			);
		} else {
			int index( expr->add_execution_step( HExpression::OExecutionStep() ) );
			_executionStepsBacklog.emplace_back(
				OExecutionStep::OPERATION::USE,
				expression,
				fc._scopeStack.top(),
				!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : IDENTIFIER::INVALID,
				index,
				refIdentifier,
				range_.start()
			);
		}
	}
	fc._valueTypes.push( { guess_type( refIdentifier ), refIdentifier } );
	return;
	M_EPILOG
}

void OCompiler::defer_get_field_reference( yaal::hcore::HString const& value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t refIdentifier( _runtime->identifier_id( value_ ) );
	_usedIdentifiers[refIdentifier].read( range_.start(), HHuginn::SYMBOL_KIND::FIELD );
	if ( huginn::is_keyword( value_ ) ) {
		if ( refIdentifier != IDENTIFIER::KEYWORD::CONSTRUCTOR ) {
			throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "` is a restricted keyword." ), _fileId, range_.start() );
		} else if ( ! _classContext ) {
			throw HHuginn::HHuginnRuntimeException( "Keyword `"_ys.append( value_ ).append( "` can be used only in class context." ), _fileId, range_.start() );
		}
	}
	HExpression* expr( current_expression().raw() );
	int p( range_.start() );
	expr->add_execution_step(
		HExpression::OExecutionStep( expr, &HExpression::get_field, p, HFrame::ACCESS::VALUE, refIdentifier )
	);
	expr->commit_oper( OPERATOR::MEMBER_ACCESS, _fileId, p );
	fc._valueTypes.pop();
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::UNKNOWN ) );
	return;
	M_EPILOG
}

void OCompiler::defer_make_variable( yaal::hcore::HString const& value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( huginn::is_restricted( value_ ) ) {
		throw HHuginn::HHuginnRuntimeException( "`"_ys.append( value_ ).append( "` is a restricted name." ), _fileId, range_.start() );
	}
	bool isFieldDefinition( !! _classContext && ( _functionContexts.get_size() == 1 ) );
	if ( isFieldDefinition ) {
		throw HHuginn::HHuginnRuntimeException( "Defining symbol `"_ys.append( value_ ).append( "` in field definition is forbidden." ), _fileId, range_.start() );
	}
	HHuginn::identifier_id_t varIdentifier( _runtime->identifier_id( value_ ) );
	HHuginn::expression_t& expression( current_expression() );
	int index( expression->add_execution_step( HExpression::OExecutionStep() ) );
	OFunctionContext::scope_context_t sc( fc._scopeStack.top() );
	sc->_hasLocalVariables = true;
	_executionStepsBacklog.emplace_back(
		OExecutionStep::OPERATION::DEFINE,
		expression,
		sc,
		!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : IDENTIFIER::INVALID,
		index,
		varIdentifier,
		range_.start(),
		fc._shortCircuit > 0
	);
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::UNKNOWN ) );
	fc._variables.emplace( varIdentifier, static_cast<int>( _executionStepsBacklog.get_size() - 1 ) );
	return;
	M_EPILOG
}

void OCompiler::defer_action( HExpression::OExecutionStep::action_t const& expressionAction_, executing_parser::range_t range_ ) {
	M_PROLOG
	HExpression* expr( current_expression().raw() );
	expr->add_execution_step( HExpression::OExecutionStep( expr, expressionAction_, range_.start() ) );
	return;
	M_EPILOG
}

void OCompiler::defer_call( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	defer_get_reference( name_, range_ );
	current_scope_context()._argumentIndexes.emplace();
	defer_oper_direct( OPERATOR::FUNCTION_CALL, range_ );
	return;
	M_EPILOG
}

void OCompiler::defer_store_direct( HHuginn::value_t const& value_, executing_parser::range_t range_ ) {
	M_PROLOG
	HExpression* expr( current_expression().raw() );
	expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_direct, range_.start(), value_ ) );
	f()._valueTypes.push( value_->get_class() );
	return;
	M_EPILOG
}

void OCompiler::dispatch_assign( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	bool hasAssign( false );
	bool assignPack( false );
	int varCount( 0 );
	while ( ! fc._operations.is_empty() ) {
		OPERATOR o( fc._operations.top()._operator );
		if ( ( o < OPERATOR::ASSIGN ) || ( o > OPERATOR::POWER_ASSIGN ) ) {
			break;
		}
		int p( fc._operations.top()._position );
		fc._operations.pop();
		if ( assignPack ) {
			throw HHuginn::HHuginnRuntimeException( "Assignee pack must be final assignment.", _fileId, p );
		}
		varCount = fc._variableCount.top();
		if ( varCount == 1 ) {
			M_ASSERT( fc._valueTypes.get_size() >= 2 );
			M_ASSERT( ! fc._variables.is_empty() );
			M_ASSERT( o != OPERATOR::ASSIGN_PACK );
			HClass const* srcType( fc._valueTypes.top()._class );
			fc._valueTypes.pop();
			HClass const* destType( fc._valueTypes.top()._class );
			fc._valueTypes.pop();
			OFunctionContext::OVariableRef varRef( fc._variables.top() );
			HClass const* realDestType( destType );
			if ( compiled_type_id( realDestType ) == HHuginn::TYPE::UNKNOWN ) {
				realDestType = guess_type( varRef._identifier );
			}
			if ( varRef._identifier != IDENTIFIER::INVALID ) {
				note_type( varRef._identifier, srcType );
			}
			if ( varRef._identifier != IDENTIFIER::INVALID ) {
				if ( o == OPERATOR::ASSIGN ) {
					_usedIdentifiers[varRef._identifier].write( range_.start(), HHuginn::SYMBOL_KIND::VARIABLE );
				} else if ( are_congruous( srcType, realDestType ) ) {
					_usedIdentifiers[varRef._identifier].read( range_.start(), HHuginn::SYMBOL_KIND::VARIABLE );
					M_ASSERT( varRef._executionStepIndex >= 0 );
					_executionStepsBacklog[varRef._executionStepIndex]._operation = OExecutionStep::OPERATION::UPDATE;
				} else {
					operands_type_mismatch( op_to_str( o ), realDestType, srcType, _fileId, p );
				}
			}
			bool setCongruous( false );
			switch ( o ) {
				case ( OPERATOR::PLUS_ASSIGN ):
				case ( OPERATOR::MINUS_ASSIGN ): {
					if ( ! ( is_summable( srcType ) && is_summable( realDestType ) ) ) {
						throw HHuginn::HHuginnRuntimeException(
							hcore::HString( o == OPERATOR::PLUS_ASSIGN ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB] )
								.append( a_type_name( realDestType ) )
								.append( ", " )
								.append( a_type_name( srcType ) ),
							_fileId,
							p
						);
					}
				} break;
				case ( OPERATOR::MULTIPLY_ASSIGN ): {
					setCongruous = is_set_congruent( srcType ) && is_set_congruent( realDestType );
				} /* fallthrough */
				case ( OPERATOR::DIVIDE_ASSIGN ):
				case ( OPERATOR::MODULO_ASSIGN ):
				case ( OPERATOR::POWER_ASSIGN ): {
					if ( ! ( ( is_numeric_congruent( srcType ) && is_numeric_congruent( realDestType ) ) || setCongruous ) ) {
						throw HHuginn::HHuginnRuntimeException(
							hcore::HString(
								o == OPERATOR::MULTIPLY_ASSIGN
									? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL]
									: (
										o == OPERATOR::POWER_ASSIGN ? _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV]
									)
							)
								.append( a_type_name( realDestType ) )
								.append( ", " )
								.append( a_type_name( srcType ) ),
							_fileId,
							p
						);
					}
				} break;
				case ( OPERATOR::ASSIGN ): break;
				default: {
					M_ASSERT( ! "bad code path"[0] );
				} break;
			}
			if ( ! is_reference_congruent( destType ) ) {
				throw HHuginn::HHuginnRuntimeException( "Setting a non reference location.", _fileId, p );
			}
			fc._variables.pop();
			fc._valueTypes.push( congruent( srcType, destType ) );
		} else if ( o != OPERATOR::ASSIGN_PACK ) {
			M_ASSERT( o != OPERATOR::ASSIGN );
			throw HHuginn::HHuginnRuntimeException( "Mutating variable pack is not supported.", _fileId, p );
		} else {
			M_ASSERT( fc._valueTypes.get_size() >= ( varCount + 1 ) );
			M_ASSERT( fc._variables.get_size() >= varCount );
			fc._valueTypes.pop(); /* src value */
			for ( int i( 0 ); i < varCount; ++ i ) {
				fc._valueTypes.pop();
				OFunctionContext::OVariableRef varRef( fc._variables.top() );
				if ( varRef._identifier != IDENTIFIER::INVALID ) {
					_usedIdentifiers[varRef._identifier].write( range_.start(), HHuginn::SYMBOL_KIND::VARIABLE );
				}
				fc._variables.pop();
			}
			fc._valueTypes.push( type_to_class( HHuginn::TYPE::TUPLE ) );
			assignPack = true;
		}
		fc._variableCount.pop();
		hasAssign = true;
	}
	if ( hasAssign ) {
		HExpression* expr( current_expression().raw() );
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::set_variable, range_.start(), HFrame::ACCESS::REFERENCE, varCount ) );
		expr->commit_oper( OPERATOR::ASSIGN_TERM, _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::start_assignable( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._variableCount.push( static_cast<int>( fc._variables.get_size() ) );
	return;
	M_EPILOG
}

void OCompiler::finish_assignable( executing_parser::range_t ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int& varCount( fc._variableCount.top() );
	varCount = static_cast<int>( fc._variables.get_size() ) - varCount;
	return;
	M_EPILOG
}

void OCompiler::dispatch_subscript( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int p( range_.start() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		throw HHuginn::HHuginnRuntimeException( "`assert` is a restricted keyword.", _fileId, p );
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
			if ( ! ( nonInteger || is_integer_congruent( fc._valueTypes.top()._class ) || ( compiled_type_id( fc._valueTypes.top()._class ) == HHuginn::TYPE::NONE ) ) ) {
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
			throw HHuginn::HHuginnRuntimeException( "Range specifier is not an integer.", _fileId, p );
		}
		expression->add_execution_step( HExpression::OExecutionStep( expression.raw(), &HExpression::range, range_.start() ) );
		expression->commit_oper( OPERATOR::RANGE, _fileId, p );
		fc._lastDereferenceOperator = OPERATOR::RANGE;
	} else {
		expression->add_execution_step( HExpression::OExecutionStep( expression.raw(), &HExpression::subscript, range_.start() ) );
		expression->commit_oper( OPERATOR::SUBSCRIPT, _fileId, p );
		fc._lastDereferenceOperator = OPERATOR::SUBSCRIPT;
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 1 );
	fc._valueTypes.pop();
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::UNKNOWN ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_member_access( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		throw HHuginn::HHuginnRuntimeException( "`assert` is a restricted keyword.", _fileId, range_.start() );
	}
	fc._operations.pop();
	fc._lastDereferenceOperator = OPERATOR::MEMBER_ACCESS;
	return;
	M_EPILOG
}

void OCompiler::dispatch_function_call( HExpression::OExecutionStep::action_t const& action_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HExpression* expr( current_expression().raw() );
	if ( fc._isAssert && ( fc._nestedCalls == 0 ) ) {
		int from( range_.start() + 1 );
		OScopeContext& sc( *fc._scopeStack.top() );
		int len( sc._assertExpressionEnd - from );
		sc._assertExpressionEnd = 0;
		expr->oper( OPERATOR::FUNCTION_ARGUMENT, range_.start() );
		expr->add_execution_step(
			HExpression::OExecutionStep(
				expr,
				&HExpression::store_direct,
				range_.start(),
				_runtime->object_factory()->create_string( yaal::move( _runtime->huginn()->get_snippet( from, len ).trim() ) )
			)
		);
		fc._isAssert = false;
	}
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) ) {
		fc._operations.pop();
		fc._valueTypes.pop();
	}
	HClass const* c( type_to_class( HHuginn::TYPE::UNKNOWN ) );
	if ( ( action_ == &HExpression::function_call ) || ( action_ == &HExpression::make_partial ) ) {
		OFunctionContext::OValueDesc vd( fc._valueTypes.top() );
		fc._valueTypes.pop();
		if ( compiled_type_id( vd._class ) == HHuginn::TYPE::FUNCTION_REFERENCE ) {
			c = function_ref_to_class( vd._identifier );
		}
	} else if ( action_ == &HExpression::make_dict ) {
		c = type_to_class( HHuginn::TYPE::DICT );
	} else {
		c = type_to_class( HHuginn::TYPE::LOOKUP );
	}
	fc._valueTypes.push( c );
	M_ASSERT( fc._operations.top()._operator == OPERATOR::FUNCTION_CALL );
	defer_function_call( action_, range_ );
	expr->commit_oper( OPERATOR::FUNCTION_CALL, _fileId, range_.start() );
	fc._operations.pop();
	return;
	M_EPILOG
}

void OCompiler::defer_function_call( HExpression::OExecutionStep::action_t action_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( commit_unbound( action_, range_ ) ) {
		return;
	}
	if ( action_ == &HExpression::make_partial ) {
		action_ = &HExpression::function_call;
	}
	defer_action( action_, range_ );
	return;
	M_EPILOG
}

void OCompiler::start_function_call( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._lastDereferenceOperator == OPERATOR::MEMBER_ACCESS ) {
		fc._lastDereferenceOperator = OPERATOR::NONE;
		HExpression* expr( current_expression().raw() );
		expr->execution_step( expr->execution_step_count() - 1 )._access = HFrame::ACCESS::BOUND_CALL;
	}
	fc._scopeStack.top()->_argumentIndexes.emplace();
	defer_oper_direct( OPERATOR::FUNCTION_CALL, range_ );
	if ( fc._isAssert ) {
		++ fc._nestedCalls;
	}
	return;
	M_EPILOG
}

void OCompiler::repack_named_parameters( executing_parser::range_t range_ ) {
	M_PROLOG
	HExpression* expr( current_expression().raw() );
	expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::repack_named_parameters, range_.start() ) );
	return;
	M_EPILOG
}

void OCompiler::unpack_variadic_parameters( executing_parser::range_t range_ ) {
	M_PROLOG
	HExpression* expr( current_expression().raw() );
	expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::unpack_variadic_parameters, range_.start() ) );
	return;
	M_EPILOG
}

void OCompiler::note_function_argument( int index_, executing_parser::range_t ) {
	M_PROLOG
	current_scope_context()._argumentIndexes.top().push_back( index_ );
	return;
	M_EPILOG
}

void OCompiler::index_unbound( int index_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( index_ <= IMPLICIT_UNBOUND_INDEX ) {
		throw HHuginn::HHuginnRuntimeException( "Invalid explicit unbound index value: "_ys.append( index_ ), _fileId, range_.start() );
	}
	current_scope_context()._argumentIndexes.top().back() = index_;
	return;
	M_EPILOG
}

void OCompiler::close_function_call( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	if ( fc._isAssert ) {
		-- fc._nestedCalls;
		OScopeContext& sc( *fc._scopeStack.top() );
		if ( ( fc._nestedCalls == 0 ) && ( sc._assertExpressionEnd == 0 ) ) {
			sc._assertExpressionEnd = range_.start();
		}
	}
	return;
	M_EPILOG
}

bool OCompiler::commit_unbound( HExpression::OExecutionStep::action_t const& action_, executing_parser::range_t range_ ) {
	M_PROLOG
	if ( action_ != &HExpression::make_partial ) {
		return ( false );
	}
	OScopeContext& sc( current_scope_context() );
	OScopeContext::argument_indexes_t const& argumentIndexesFound( sc._argumentIndexes.top() );
	if ( count( argumentIndexesFound.begin(), argumentIndexesFound.end(), BOUND_INDEX + 0 ) == argumentIndexesFound.get_size() ) {
		return ( false );
	}
	OScopeContext::argument_indexes_t argumentIndexes( argumentIndexesFound );
	sort( argumentIndexes.begin(), argumentIndexes.end() );
	int previous( BOUND_INDEX );
	bool hasImplicit( false );
	bool hasExplicit( false );
	for( int current : argumentIndexes ) {
		hasImplicit = hasImplicit || ( current == IMPLICIT_UNBOUND_INDEX );
		hasExplicit = hasExplicit || ( current > IMPLICIT_UNBOUND_INDEX );
		if ( ( ( current - previous ) > 1 ) && ( ( previous != BOUND_INDEX ) || ( current != ( IMPLICIT_UNBOUND_INDEX + 1 ) ) ) ) {
			throw HHuginn::HHuginnRuntimeException( "A hole in explicit unbound indexing.", _fileId, range_.start() );
		}
		previous = current;
	}
	M_ASSERT( hasImplicit || hasExplicit );
	if ( hasImplicit && hasExplicit ) {
		throw HHuginn::HHuginnRuntimeException( "Implicit and explicit unbound indexes cannot be mixed.", _fileId, range_.start() );
	}
	HPartial::unbound_indexes_t unboundIndexes;
	int internalIndex( 0 );
	int externalIndex( 1 );
	for ( int index : argumentIndexesFound ) {
		if ( index == IMPLICIT_UNBOUND_INDEX ) {
			index = externalIndex;
			++ externalIndex;
		}
		if ( index > IMPLICIT_UNBOUND_INDEX ) {
			unboundIndexes.emplace_back( internalIndex, index - 1 );
		}
		++ internalIndex;
	}
	sc._argumentIndexes.pop();
	OScopeContext::argument_indexes_t uniqCounter( unboundIndexes.get_size(), IMPLICIT_UNBOUND_INDEX + 0 );
	for ( HPartial::HIndexMap const& indexMap : unboundIndexes ) {
		uniqCounter[indexMap.external_index()] = IMPLICIT_UNBOUND_INDEX + 1;
	}
	int argCount( static_cast<int>( count( uniqCounter.begin(), uniqCounter.end(), IMPLICIT_UNBOUND_INDEX + 1 ) ) );
	HExpression* expr( current_expression().raw() );
	expr->add_execution_step( HExpression::OExecutionStep( expr, action_, range_.start(), unboundIndexes, argCount ) );
	return ( true );
	M_EPILOG
}

void OCompiler::dispatch_plus( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::PLUS ) || ( o == OPERATOR::MINUS ) );
	defer_action( o == OPERATOR::PLUS ? &HExpression::plus : &HExpression::minus, range_ );
	current_expression()->commit_oper( o, _fileId, p );
	fc._operations.pop();
	HClass const* c1( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* c2( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	if ( ! are_congruous( c1, c2 ) ) {
		operands_type_mismatch( op_to_str( o ), c2, c1, _fileId, range_.start() );
	}
	if ( ! ( is_summable( c1 ) && is_summable( c2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::HString( o == OPERATOR::PLUS ? _errMsgHHuginn_[ERR_CODE::OP_NOT_SUM] : _errMsgHHuginn_[ERR_CODE::OP_NOT_SUB] )
				.append( a_type_name( c2 ) )
				.append( ", " )
				.append( a_type_name( c1 ) ),
			_fileId,
			p
		);
	}
	fc._valueTypes.push( congruent( c1, c2 ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_mul( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::MULTIPLY ) || ( o == OPERATOR::DIVIDE ) || ( o == OPERATOR::MODULO ) );
	defer_action( o == OPERATOR::MULTIPLY ? &HExpression::mul : ( o == OPERATOR::DIVIDE ? &HExpression::div : &HExpression::mod ), range_ );
	current_expression()->commit_oper( o, _fileId, p );
	fc._operations.pop();
	HClass const* c1( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* c2( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	if ( ! are_congruous( c1, c2 ) ) {
		operands_type_mismatch( op_to_str( o ), c2, c1, _fileId, range_.start() );
	}
	bool setCongruous( ( o == OPERATOR::MULTIPLY ) && is_set_congruent( c1 ) && is_set_congruent( c2 ) );
	if ( ! ( ( is_numeric_congruent( c1 ) && is_numeric_congruent( c2 ) ) || setCongruous ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::HString( o == OPERATOR::MULTIPLY ? _errMsgHHuginn_[ERR_CODE::OP_NOT_MUL] : _errMsgHHuginn_[ERR_CODE::OP_NOT_DIV] )
				.append( a_type_name( c2 ) )
				.append( ", " )
				.append( a_type_name( c1 ) ),
			_fileId,
			p
		);
	}
	fc._valueTypes.push( congruent( c1, c2 ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_power( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	bool hasPower( false );
	while ( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::POWER ) ) {
		M_ASSERT( fc._valueTypes.get_size() >= 2 );
		int p( fc._operations.top()._position );
		fc._operations.pop();
		HClass const* c1( fc._valueTypes.top()._class );
		fc._valueTypes.pop();
		HClass const* c2( fc._valueTypes.top()._class );
		fc._valueTypes.pop();
		if ( ! are_congruous( c1, c2 ) ) {
			operands_type_mismatch( op_to_str( OPERATOR::POWER ), c2, c1, _fileId, p );
		}
		if ( ! ( is_numeric_congruent( c1 ) && is_numeric_congruent( c2 ) ) ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_EXP] )
					.append( a_type_name( c2 ) )
					.append( ", " )
					.append( a_type_name( c1 ) ),
				_fileId,
				p
			);
		}
		fc._valueTypes.push( congruent( c1, c2 ) );
		hasPower = true;
	}
	if ( hasPower ) {
		defer_action( &HExpression::power, range_ );
		current_expression()->commit_oper( OPERATOR::POWER_TERM, _fileId, range_.start() );
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_factorial( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._valueTypes.is_empty() );
	defer_action( &HExpression::factorial, range_ );
	if ( ! is_numeric_congruent( fc._valueTypes.top()._class ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_NUM] )
				.append( a_type_name( fc._valueTypes.top()._class ) ),
			_fileId,
			range_.start()
		);
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_negate( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	M_ASSERT( ! fc._operations.is_empty() );
	OPositionedOperator const& po(  fc._operations.top() );
	int p( po._position  );
	M_ASSERT( po._operator == OPERATOR::NEGATE );
	M_ASSERT( ! fc._valueTypes.is_empty() );
	defer_action( &HExpression::negate, range_ );
	current_expression()->commit_oper( OPERATOR::NEGATE, _fileId, p );
	fc._operations.pop();
	if ( ! is_numeric_congruent( fc._valueTypes.top()._class ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_NUM] )
				.append( a_type_name( fc._valueTypes.top()._class ) ),
			_fileId,
			p
		);
	}
	return;
	M_EPILOG
}

void OCompiler::dispatch_compare( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT(
		( o == OPERATOR::LESS )
		|| ( o == OPERATOR::GREATER )
		|| ( o == OPERATOR::LESS_OR_EQUAL )
		|| ( o == OPERATOR::GREATER_OR_EQUAL )
		|| ( o == OPERATOR::IS_ELEMENT_OF )
		|| ( o == OPERATOR::IS_NOT_ELEMENT_OF )
	);
	char const* os( op_to_str( o ) );
	switch ( o ) {
		case ( OPERATOR::LESS ):             { defer_action( &HExpression::less, range_ );             } break;
		case ( OPERATOR::GREATER ):          { defer_action( &HExpression::greater, range_ );          } break;
		case ( OPERATOR::LESS_OR_EQUAL ):    { defer_action( &HExpression::less_or_equal, range_ );    } break;
		case ( OPERATOR::GREATER_OR_EQUAL ): { defer_action( &HExpression::greater_or_equal, range_ ); } break;
		case ( OPERATOR::IS_ELEMENT_OF ):    { defer_action( &HExpression::is_element_of, range_ );    } break;
		case ( OPERATOR::IS_NOT_ELEMENT_OF ):{ defer_action( &HExpression::is_not_element_of, range_ ); } break;
		default: {
			M_ASSERT( ! "bad code path"[0] );
		}
	}
	current_expression()->commit_oper( o, _fileId, p );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HClass const* c1( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* c2( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	if ( ( o != OPERATOR::IS_ELEMENT_OF ) && ( o != OPERATOR::IS_NOT_ELEMENT_OF ) ) {
		if ( ! are_congruous( c1, c2 ) ) {
			operands_type_mismatch( os, c2, c1, _fileId, p );
		}
		if ( ! ( is_comparable_congruent( c1 ) && is_comparable_congruent( c2 ) ) ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_CMP] )
					.append( a_type_name( c2 ) )
					.append( ", " )
					.append( a_type_name( c1 ) ),
				_fileId,
				p
			);
		}
	} else if ( ! is_collection_like( c1 ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_COLL] )
				.append( a_type_name( c1 ) ),
			_fileId,
			p
		);
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::BOOLEAN ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_boolean( HExpression::OExecutionStep::action_t const& action_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	int p( po._position );
	if ( !! action_ ) {
		M_ASSERT( action_ == &HExpression::boolean_xor );
		defer_action( action_, range_ );
		current_expression()->commit_oper( po._operator, _fileId, p );
	}
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HClass const* c1( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* c2( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	if ( ! ( is_boolean_congruent( c1 ) && is_boolean_congruent( c2 ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL] )
				.append( a_type_name( c2 ) )
				.append( ", " )
				.append( a_type_name( c1 ) ),
			_fileId,
			p
		);
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::BOOLEAN ) );
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
	HClass const* c1( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* c2( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* t0( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	if ( ! is_boolean_congruent( t0 ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( a_type_name( t0 ) ),
			_fileId,
			p
		);
	}
	fc._valueTypes.push( c1 == c2 ? c1 : type_to_class( HHuginn::TYPE::UNKNOWN ) );
	return;
	M_EPILOG
}

void OCompiler::dispatch_equals( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OPositionedOperator po( fc._operations.top() );
	OPERATOR o( po._operator );
	int p( po._position );
	M_ASSERT( ( o == OPERATOR::EQUALS ) || ( o == OPERATOR::NOT_EQUALS ) );
	defer_action( o == OPERATOR::EQUALS ? &HExpression::equals : &HExpression::not_equals, range_ );
	current_expression()->commit_oper( o, _fileId, p );
	fc._operations.pop();
	M_ASSERT( fc._valueTypes.get_size() >= 2 );
	HClass const* c1( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	HClass const* c2( fc._valueTypes.top()._class );
	fc._valueTypes.pop();
	if (
		! are_congruous( c1, c2 )
		&& ( compiled_type_id( c1 ) != HHuginn::TYPE::NONE )
		&& ( compiled_type_id( c2 ) != HHuginn::TYPE::NONE )
	) {
		operands_type_mismatch( op_to_str( o ), c2, c1, _fileId, p );
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::BOOLEAN ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_real( double long value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HExpression* expr( current_expression().raw() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_direct, range_.start(), _runtime->object_factory()->create_real( value_ ), HHuginn::TYPE::REAL ) );
	} else {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_real, range_.start(), value_ ) );
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::REAL ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_integer( int long long value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HExpression* expr( current_expression().raw() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_direct, range_.start(), _runtime->object_factory()->create_integer( value_ ), HHuginn::TYPE::INTEGER ) );
	} else {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_integer, range_.start(), value_ ) );
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::INTEGER ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_string( yaal::hcore::HString const& value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HExpression* expr( current_expression().raw() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_direct, range_.start(), _runtime->object_factory()->create_string( value_ ), HHuginn::TYPE::STRING ) );
	} else {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_string, range_.start(), value_ ) );
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::STRING ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_number( yaal::hcore::HNumber const& value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HExpression* expr( current_expression().raw() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_direct, range_.start(), _runtime->object_factory()->create_number( value_ ), HHuginn::TYPE::NUMBER ) );
	} else {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_number, range_.start(), value_ ) );
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::NUMBER ) );
	return;
	M_EPILOG
}

void OCompiler::defer_store_character( code_point_t value_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HExpression* expr( current_expression().raw() );
	if ( fc._operations.is_empty() || ( find( begin( _copyConstContext_ ), end( _copyConstContext_ ), fc._operations.top()._operator ) == end( _copyConstContext_ ) ) ) {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_direct, range_.start(), _runtime->object_factory()->create_character( value_ ), HHuginn::TYPE::CHARACTER ) );
	} else {
		expr->add_execution_step( HExpression::OExecutionStep( expr, &HExpression::store_character, range_.start(), value_ ) );
	}
	fc._valueTypes.push( type_to_class( HHuginn::TYPE::CHARACTER ) );
	return;
	M_EPILOG
}

void OCompiler::commit_boolean( OPERATOR operator_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	int expressionCount( static_cast<int>( fc.expressions_stack().top().get_size() ) );
	if ( expressionCount > 1 ) {
		HHuginn::value_t And( _runtime->object_factory()->create<HBooleanEvaluator>( _runtime, fc.expressions_stack().top(), operator_ ) );
		fc.expressions_stack().pop();
		M_ASSERT( ! fc._valueTypes.is_empty() && ( compiled_type_id( fc._valueTypes.top()._class ) == HHuginn::TYPE::BOOLEAN ) );
		fc._valueTypes.pop();
		fc._shortCircuit -= ( expressionCount - 1 );
		defer_store_direct( And, range_ );
		HExpression& expression( *current_expression() );
		expression.oper( operator_, range_.start() );
		defer_action( operator_ == OPERATOR::BOOLEAN_AND ? &HExpression::boolean_and : &HExpression::boolean_or, range_ );
		expression.commit_oper( operator_, _fileId, range_.start() );
	} else {
		HHuginn::expression_t e( fc._scopeStack.top()->expression() );
		fc.expressions_stack().pop();
		merge( fc._scopeStack.top()->expression(), e );
	}
	return;
	M_EPILOG
}

void OCompiler::commit_ternary( executing_parser::range_t range_ ) {
	OFunctionContext& fc( f() );
	HFunction::expressions_t const& exprs( fc.expressions_stack().top() );
	if ( exprs.get_size() > 1 ) {
		M_ASSERT( exprs.get_size() == 3 );
		HHuginn::value_t ternary( _runtime->object_factory()->create<HTernaryEvaluator>( exprs[0], exprs[1], exprs[2] ) );
		fc.expressions_stack().pop();
		M_ASSERT( ! fc._valueTypes.is_empty() );
		fc._valueTypes.pop();
		fc._shortCircuit -= 2;
		defer_store_direct( ternary, range_ );
		HExpression& expression( *current_expression() );
		expression.oper( OPERATOR::TERNARY, range_.start() );
		defer_action( &HExpression::ternary, range_ );
		expression.commit_oper( OPERATOR::TERNARY, _fileId, range_.start() );
	} else {
		HHuginn::expression_t e( fc._scopeStack.top()->expression() );
		fc.expressions_stack().pop();
		merge( fc._scopeStack.top()->expression(), e );
	}
}

void OCompiler::set_lambda_name( executing_parser::range_t range_ ) {
	M_PROLOG
	OCompiler::OFunctionContext& fc( f() );
	HHuginn::HCoordinate coord( _runtime->huginn()->get_coordinate( _fileId, range_.start() ) );
	using yaal::hcore::to_string;
	HHuginn::identifier_id_t id( _runtime->identifier_id( to_string( "@" ).append( coord.line() ).append( ":" ).append( coord.column() ) ) );
	_functionContexts.emplace( make_resource<OFunctionContext>( this, id, ++ _statementIdGenerator, _fileId, true ) );
	if ( ! fc._captures.is_empty() ) {
		_capturesLog.insert( make_pair( f()._functionIdentifier, yaal::move( fc._captures ) ) );
	}
	create_scope( range_ );
	return;
	M_EPILOG
}

void OCompiler::create_lambda( executing_parser::range_t range_ ) {
	M_PROLOG
	function_info_t fi( create_function_low( range_ ) );
	OCompiler::OFunctionContext& fc( f() );
	HHuginn::identifier_id_t enclosingFunctionId( fc._functionIdentifier );
	lambda_definition_sites_t::const_iterator it( _lambdaDefinitionSites.find( enclosingFunctionId ) );
	if ( it != _lambdaDefinitionSites.end() ) {
		enclosingFunctionId = it->second;
	}
	_lambdaDefinitionSites.insert( make_pair( fi.first, enclosingFunctionId ) );
	HHuginn::value_t fRef( _runtime->object_factory()->create_function_reference( fi.first, fi.second, "Lambda: "_ys.append( _runtime->identifier_name( fi.first ) ) ) );
	defer_store_direct( fRef, range_ );
	if ( _capturesLog.find( fi.first ) != _capturesLog.end() ) {
		defer_action( &HExpression::create_closure, range_ );
		current_expression()->commit_oper( OPERATOR::FUNCTION_CALL, _fileId, range_.start() );
		f()._valueTypes.top()._class = type_to_class( HHuginn::TYPE::BOUND_METHOD );
	}
	return;
	M_EPILOG
}

void OCompiler::add_parameter( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t parameterIdentifier( _runtime->identifier_id( name_ ) );
	captures_log_t::const_iterator cli( _capturesLog.find( fc._functionIdentifier ) );
	if ( ( cli != _capturesLog.end() ) && ( find( cli->second.begin(), cli->second.end(), parameterIdentifier ) != cli->second.end() ) ) {
		throw HHuginn::HHuginnRuntimeException( "Symbol `"_ys.append( name_ ).append( "` is a already used as a capture." ), _fileId, range_.start() );
	}
	if ( find( fc._parameters.begin(), fc._parameters.end(), parameterIdentifier ) != fc._parameters.end() ) {
		throw HHuginn::HHuginnRuntimeException( "Parameter `"_ys.append( name_ ).append( "` was already defined." ), _fileId, range_.start() );
	}
	_usedIdentifiers[parameterIdentifier].write( range_.start(), HHuginn::SYMBOL_KIND::VARIABLE );
	_executionStepsBacklog.emplace_back(
		OExecutionStep::OPERATION::DEFINE,
		HHuginn::expression_t(),
		fc._scopeStack.top(),
		!! _classContext && ! fc._isLambda ? _classContext->_classIdentifier : IDENTIFIER::INVALID,
		-1,
		parameterIdentifier,
		range_.start()
	);
	fc._parameters.push_back( parameterIdentifier );
	return;
	M_EPILOG
}

void OCompiler::mark_variadic( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._isVariadic = true;
	return;
	M_EPILOG
}

void OCompiler::mark_named_parameter_capture( void ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc._capturesNamedParameters = true;
	return;
	M_EPILOG
}

void OCompiler::mark_expression_position( executing_parser::range_t range_ ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	current_expression()->set_range( _fileId, range_ );
	return;
	M_EPILOG
}

void OCompiler::add_capture( yaal::hcore::HString const& name_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	HHuginn::identifier_id_t captureIdentifier( _runtime->identifier_id( name_ ) );
	if ( find( fc._captures.begin(), fc._captures.end(), captureIdentifier ) != fc._captures.end() ) {
		throw HHuginn::HHuginnRuntimeException( "Capture `"_ys.append( name_ ).append( "` was already defined." ), _fileId, range_.start() );
	}
	if ( fc._captures.is_empty() ) {
		current_expression()->oper( OPERATOR::FUNCTION_CALL, range_.start() );
	}
	fc._operations.emplace( OPERATOR::FUNCTION_CALL, range_.start() );
	fc._captures.push_back( captureIdentifier );
	start_subexpression( range_ );
	return;
	M_EPILOG
}

void OCompiler::commit_capture( executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	OCompiler::expressions_stack_t& exprStack( fc.expressions_stack() );
	HHuginn::expression_t captureExpression( exprStack.top().back() );
	exprStack.pop();
	HHuginn::expression_t& expression( exprStack.top().back() );
	M_ASSERT( ! fc._operations.is_empty() && ( fc._operations.top()._operator == OPERATOR::FUNCTION_CALL ) );
	fc._operations.pop();
	if ( captureExpression->is_empty() ) {
		defer_get_reference( _runtime->identifier_name( fc._captures.back() ), range_ );
	} else {
		merge( expression, captureExpression );
	}
	fc._valueTypes.pop();
	expression->oper( OPERATOR::FUNCTION_ARGUMENT, range_.start() );
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

void OCompiler::start_subexpression( executing_parser::range_t range_ ) {
	M_PROLOG
	f().expressions_stack().emplace( 1, new_expression( _fileId, range_ ) );
	return;
	M_EPILOG
}

void OCompiler::add_subexpression( OPERATOR op_, executing_parser::range_t range_ ) {
	M_PROLOG
	OFunctionContext& fc( f() );
	fc.expressions_stack().top().emplace_back( new_expression( _fileId, range_ ) );
	fc._operations.emplace( op_, range_.start() );
	++ fc._shortCircuit;
	return;
	M_EPILOG
}

void OCompiler::commit_expression( executing_parser::range_t ) {
	M_PROLOG
	M_ASSERT( ! f()._scopeStack.is_empty() );
	current_scope()->add_statement( current_expression() );
	reset_expression();
	return;
	M_EPILOG
}

}

}

}

