/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "expression.hxx"
#include "runtime.hxx"
#include "frame.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "booleanevaluator.hxx"
#include "helper.hxx"
#include "keyword.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HExpression::OExecutionStep::OExecutionStep( void )
	: _expression( nullptr )
	, _action( nullptr )
	, _position( INVALID_POSITION )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, int index_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( index_ )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, HHuginn::identifier_id_t identifierId_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( identifierId_ )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, HStatement::statement_id_t statementId_, int index_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _statementId( statementId_ )
	, _index( index_ )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HFrame::ACCESS access_, HStatement::statement_id_t statementId_, int index_, HHuginn::identifier_id_t identifierId_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( access_ )
	, _statementId( statementId_ )
	, _index( index_ )
	, _identifierId( identifierId_ )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HHuginn::value_t const* valueRef_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef( valueRef_ )
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, HHuginn::value_t const& value_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value( value_ )
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, double long doubleLong_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( doubleLong_ )
	, _integer( 0 )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, int long long integer_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( integer_ )
	, _string()
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, yaal::hcore::HString const& string_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string( string_ )
	, _character( 0 ) {
}

HExpression::OExecutionStep::OExecutionStep( HExpression* expression_, action_t action_, int position_, code_point_t character_ )
	: _expression( expression_ )
	, _action( action_ )
	, _position( position_ )
	, _access( HFrame::ACCESS::VALUE )
	, _statementId( INVALID_STATEMENT_IDENTIFIER )
	, _index( -1 )
	, _identifierId( IDENTIFIER::INVALID )
	, _value()
	, _valueRef()
	, _real( 0.0L )
	, _integer( 0 )
	, _string()
	, _character( character_ ) {
}

HExpression::HExpression( int fileId_, executing_parser::range_t range_ )
	: HStatement( INVALID_STATEMENT_IDENTIFIER, fileId_, range_ )
	, _executionSteps()
	, _instructions()
	, _operations() {
	return;
}

int HExpression::execution_step_count( void ) const {
	M_PROLOG
	return ( static_cast<int>( _executionSteps.get_size() ) );
	M_EPILOG
}

int HExpression::add_execution_step( OExecutionStep const& executionStep_ ) {
	M_PROLOG
	_executionSteps.push_back( executionStep_ );
	return ( static_cast<int>( _executionSteps.get_size() - 1 ) );
	M_EPILOG
}

void HExpression::replace_execution_step( int index_, OExecutionStep const& executionStep_ ) {
	M_PROLOG
	_executionSteps[index_] = executionStep_;
	return;
	M_EPILOG
}

void HExpression::pop_execution_step( void ) {
	M_PROLOG
	_executionSteps.pop_back();
	return;
	M_EPILOG
}

HExpression::OExecutionStep& HExpression::execution_step( int idx_ ) {
	M_PROLOG
	return ( _executionSteps[idx_] );
	M_EPILOG
}

void HExpression::merge( HExpression& expression_ ) {
	M_PROLOG
	for ( OExecutionStep& es : expression_._executionSteps ) {
		es._expression = this;
	}
	move( expression_._executionSteps.begin(), expression_._executionSteps.end(), back_insert_iterator( _executionSteps ) );
	move( expression_._instructions.begin(), expression_._instructions.end(), back_insert_iterator( _instructions ) );
	expression_._executionSteps.clear();
	expression_._instructions.clear();
	operations_t operationsStore;
	while ( ! expression_._operations.is_empty() ) {
		operationsStore.push( expression_._operations.top() );
		expression_._operations.pop();
	}
	while ( ! operationsStore.is_empty() ) {
		_operations.push( operationsStore.top() );
		operationsStore.pop();
	}
	return;
	M_EPILOG
}

bool HExpression::is_empty( void ) const {
	M_PROLOG
	return ( _executionSteps.is_empty() );
	M_EPILOG
}

void HExpression::oper( OPERATOR operator_, int position_ ) {
	M_PROLOG
	_operations.emplace( operator_, position_ );
	return;
	M_EPILOG
}

void HExpression::commit_oper( OPERATOR operator_ ) {
	M_PROLOG
	M_ASSERT( ! _operations.is_empty() );
	switch ( operator_ ) {
		case ( OPERATOR::EQUALS ):
		case ( OPERATOR::NOT_EQUALS ):
		case ( OPERATOR::LESS ):
		case ( OPERATOR::GREATER ):
		case ( OPERATOR::LESS_OR_EQUAL ):
		case ( OPERATOR::GREATER_OR_EQUAL ):
		case ( OPERATOR::IS_ELEMENT_OF ):
		case ( OPERATOR::IS_NOT_ELEMENT_OF ):
		case ( OPERATOR::PLUS ):
		case ( OPERATOR::MINUS ):
		case ( OPERATOR::MULTIPLY ):
		case ( OPERATOR::DIVIDE ):
		case ( OPERATOR::MODULO ):
		case ( OPERATOR::NEGATE ):
		case ( OPERATOR::BOOLEAN_AND ):
		case ( OPERATOR::BOOLEAN_OR ):
		case ( OPERATOR::BOOLEAN_XOR ):
		case ( OPERATOR::BOOLEAN_NOT ):
		case ( OPERATOR::TERNARY ):
		case ( OPERATOR::MEMBER_ACCESS ):
		case ( OPERATOR::MODULUS ):
		case ( OPERATOR::PARENTHESIS ): {
			M_ASSERT( _operations.top()._operator == operator_ );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		case ( OPERATOR::POWER_TERM ): {
			M_DEBUG_CODE( bool hasPowerOperator( false ); );
			while ( ! _operations.is_empty() && ( _operations.top()._operator == OPERATOR::POWER ) ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
				M_DEBUG_CODE( hasPowerOperator = true; );
			}
			M_ASSERT( hasPowerOperator );
			_instructions.push_back( OPositionedOperator( operator_, 0 ) );
		} break;
		case ( OPERATOR::ASSIGN_TERM ): {
			M_DEBUG_CODE( bool hasAssignOperator( false ); );
			while ( ! _operations.is_empty() && ( _operations.top()._operator >= OPERATOR::ASSIGN ) && ( _operations.top()._operator <= OPERATOR::POWER_ASSIGN ) ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
				M_DEBUG_CODE( hasAssignOperator = true; );
			}
			M_ASSERT( hasAssignOperator );
			_instructions.push_back( OPositionedOperator( operator_, 0 ) );
		} break;
		case ( OPERATOR::FUNCTION_CALL ): {
			while ( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
			}
			M_ASSERT( _operations.top()._operator == operator_ );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		case ( OPERATOR::SUBSCRIPT ): {
			M_ASSERT( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT );
			_instructions.push_back( _operations.top() );
			_operations.pop();
			M_ASSERT( _operations.top()._operator == operator_ );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		case ( OPERATOR::RANGE ): {
			while ( ( _operations.top()._operator == OPERATOR::FUNCTION_ARGUMENT ) || ( _operations.top()._operator == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
				_instructions.push_back( _operations.top() );
				_operations.pop();
			}
			M_ASSERT( _operations.top()._operator == OPERATOR::SUBSCRIPT );
			_instructions.push_back( _operations.top() );
			_operations.pop();
		} break;
		default: {
			M_ASSERT( ! "Invalid code path."[0] );
		}
	}
	return;
	M_EPILOG
}

void HExpression::close_parenthesis( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( ! frame_->values().is_empty() );
	OPERATOR o( _instructions[frame_->ip()]._operator );
	M_ASSERT( ( o == OPERATOR::MODULUS ) || ( o == OPERATOR::PARENTHESIS ) );
	++ frame_->ip();
	if ( o == OPERATOR::MODULUS ) {
		HHuginn::value_t v( yaal::move( frame_->values().top() ) );
		frame_->values().pop();
		frame_->values().push( instruction::abs( frame_->thread(), v, executionStep_._position ) );
	}
	return;
	M_EPILOG
}

void HExpression::get_field_direct( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_field( executionStep_._access, executionStep_._index ) );
	return;
	M_EPILOG
}

void HExpression::get_variable_value( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_variable_value( executionStep_._statementId, executionStep_._index ) );
	return;
	M_EPILOG
}

void HExpression::get_variable_reference( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_variable_reference( executionStep_._statementId, executionStep_._index ) );
	return;
	M_EPILOG
}

void HExpression::make_variable( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->make_variable( executionStep_._statementId, executionStep_._index ) );
	return;
	M_EPILOG
}

void HExpression::get_this( OExecutionStep const&, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_this() );
	return;
	M_EPILOG
}

void HExpression::get_super( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().emplace( frame_->get_super( executionStep_._position ) );
	return;
	M_EPILOG
}

void HExpression::get_field( OExecutionStep const& executionStep_,huginn::HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MEMBER_ACCESS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t v( yaal::move( values.top() ) );
	values.pop();
	HThread* t( frame_->thread() );
	if ( executionStep_._access != HFrame::ACCESS::BOUND_CALL ) {
		values.push( instruction::member( t, executionStep_._access, v, executionStep_._identifierId, p ) );
	} else {
		HHuginn::value_t f( instruction::member( t, executionStep_._access, v, executionStep_._identifierId, p ) );
		if ( f->type_id() == HHuginn::TYPE::METHOD ) {
			values.push( yaal::move( v ) );
		}
		values.push( yaal::move( f ) );
	}
	return;
	M_EPILOG
}

void HExpression::set_variable( OExecutionStep const& es_, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	HFrame::values_t& values( frame_->values() );
	HThread* t( frame_->thread() );
	while ( ip < static_cast<int>( _instructions.get_size() ) ) {
		instructions_t::value_type& operation( _instructions[ip] );
		if ( ( operation._operator < OPERATOR::ASSIGN ) || ( operation._operator > OPERATOR::POWER_ASSIGN ) ) {
			break;
		}
		int p( operation._position );
		++ ip;
		HHuginn::value_t src( yaal::move( values.top() ) );
		values.pop();
		if ( operation._operator == OPERATOR::ASSIGN_PACK ) {
			if ( src->type_id() != HHuginn::TYPE::TUPLE ) {
				throw HHuginn::HHuginnRuntimeException( "Assigner is not a `tuple` object.", file_id(), p );
			}
			HHuginn::HTuple::values_t& srcData( static_cast<HHuginn::HTuple*>( src.raw() )->value() );
			int ds( static_cast<int>( srcData.get_size() ) );
			if ( ds != es_._index ) {
				throw HHuginn::HHuginnRuntimeException(
					hcore::to_string( ds > es_._index ? "Too many" : "Not enough" )
						.append( " values to unpack, expected: " ).append( es_._index )
						.append( ", got: " ).append( ds ).append( "." ),
					file_id(),
					p
				);
			}
			HHuginn::HTuple::values_t data( es_._index );
			for ( int i( ds - 1 ); i >= 0; -- i ) {
				HHuginn::value_t dst( yaal::move( values.top() ) );
				values.pop();
				M_ASSERT( dst->type_id() == HHuginn::TYPE::REFERENCE );
				HHuginn::HReference& ref( *static_cast<HHuginn::HReference*>( dst.raw() ) );
				data[i] = srcData[i];
				ref.set( t, yaal::move( srcData[i] ), p );
			}
			values.push( t->object_factory().create_tuple( yaal::move( data ) ) );
		} else {
			HHuginn::value_t dst( yaal::move( values.top() ) );
			values.pop();
			M_ASSERT( dst->type_id() == HHuginn::TYPE::REFERENCE );
			HHuginn::HReference& ref( *static_cast<HHuginn::HReference*>( dst.raw() ) );
			if ( operation._operator == OPERATOR::ASSIGN ) {
				ref.set( t, yaal::move( src ), p );
			} else {
				HHuginn::value_t v( ref.get( t, p ) );
				HHuginn::HClass const* refClass( v->get_class() );
				HHuginn::HClass const* srcClass( src->get_class() );
				if ( refClass != srcClass ) {
					operands_type_mismatch( op_to_str( operation._operator ), refClass, srcClass, file_id(), p );
				}
				switch ( operation._operator ) {
					case ( OPERATOR::PLUS_ASSIGN ):     { instruction::add( t, v, src, p ); } break;
					case ( OPERATOR::MINUS_ASSIGN ):    { instruction::sub( t, v, src, p ); } break;
					case ( OPERATOR::MULTIPLY_ASSIGN ): { instruction::mul( t, v, src, p ); } break;
					case ( OPERATOR::DIVIDE_ASSIGN ):   { instruction::div( t, v, src, p ); } break;
					case ( OPERATOR::MODULO_ASSIGN ):   { instruction::mod( t, v, src, p ); } break;
					case ( OPERATOR::POWER_ASSIGN ):    { instruction::pow( t, v, src, p ); } break;
					default: {
						M_ASSERT( ! "bad code path"[0] );
					} break;
				}
				ref.set( t, yaal::move( v ), p );
			}
			values.push( ref.get( t, p ) );
		}
	}
	M_ASSERT( ( ip < static_cast<int>( _instructions.get_size() ) ) && (  _instructions[ip]._operator == OPERATOR::ASSIGN_TERM ) );
	++ ip;
	return;
	M_EPILOG
}

void HExpression::grab_args( HFrame* frame_, HArguments& args_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( ! frame_->values().is_empty() );
	HHuginn::values_t& values( args_ );
	while ( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT ) {
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		HHuginn::value_t& v( frame_->values().top() );
		if ( v->type_id() != HHuginn::TYPE::VARIADIC_PARAMETERS ) {
			values.push_back( yaal::move( v ) );
		} else {
			HHuginn::value_t& vp( static_cast<HHuginn::HTaggedValue*>( v.raw() )->value() );
			HHuginn::values_t& p( static_cast<HHuginn::HTuple*>( vp.raw() )->value() );
			values.insert( values.end(), p.rbegin(), p.rend() );
		}
		frame_->values().pop();
	}
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_CALL );
	reverse( values.begin(), values.end() );
	return;
	M_EPILOG
}

void HExpression::repack_named_parameters( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t v( yaal::move( values.top() ) );
	values.pop();
	if ( v->type_id() != HHuginn::TYPE::LOOKUP ) {
		throw HHuginn::HHuginnRuntimeException(
			"Packed parameter is "_ys.append( a_type_name( v->get_class() ) ).append( " instead of a `lookup`." ),
			file_id(),
			executionStep_._position
		);
	}
	HHuginn::value_t* pv( ! values.is_empty() ? &values.top() : nullptr );
	if ( pv && ( (*pv)->type_id() == HHuginn::TYPE::NAMED_PARAMETERS ) ) {
		HHuginn::HTaggedValue& tv( *static_cast<HHuginn::HTaggedValue*>( pv->raw() ) );
		HHuginn::HLookup& l( *static_cast<HHuginn::HLookup*>( tv.value().raw() ) );
		l.update( frame_->thread(), v, executionStep_._position );
		int& ip( frame_->ip() );
		M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT );
		++ ip;
	} else {
		v = frame_->thread()->object_factory().create_tagged_value( v, frame_->thread()->object_factory().named_parameters_class() );
		values.push( yaal::move( v ) );
	}
	return;
	M_EPILOG
}

void HExpression::unpack_variadic_parameters( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t& vr( values.top() );
	if ( vr->type_id() == HHuginn::TYPE::TUPLE ) {
		vr = frame_->thread()->object_factory().create_tagged_value( vr, frame_->thread()->object_factory().variadic_parameters_class() );
	} else {
		throw HHuginn::HHuginnRuntimeException(
			"Unpacked parameter is "_ys.append( a_type_name( vr->get_class() ) ).append( " instead of a `tuple`." ),
			file_id(),
			executionStep_._position
		);
	}
	return;
	M_EPILOG
}

void HExpression::function_call( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	HArguments args( frame_ );
	grab_args( frame_, args );
	M_ASSERT( ! frame_->values().is_empty() );
	int& ip( frame_->ip() );
	int p( _instructions[ip]._position );
	++ ip;
	HFrame::values_t& values( frame_->values() );
	HHuginn::HClass const* c( values.top()->get_class() );
	HHuginn::type_id_t t( c->type_id() );
	HHuginn::value_t f( yaal::move( values.top() ) );
	values.pop();
	HThread* thread( frame_->thread() );
	if ( thread->call_stack_size() >= thread->runtime().max_call_stack_size() ) {
		throw HHuginn::HHuginnRuntimeException( "Call stack size limit exceeded: "_ys.append( thread->call_stack_size() + 1 ), file_id(), executionStep_._position );
	}
	frame_->set_position( p );
	if ( t == HHuginn::TYPE::FUNCTION_REFERENCE ) {
		values.push( static_cast<HHuginn::HFunctionReference*>( f.raw() )->function()( thread, nullptr, args, p ) );
	} else if ( t == HHuginn::TYPE::BOUND_METHOD ) {
		HHuginn::HClass::HBoundMethod* m( static_cast<HHuginn::HClass::HBoundMethod*>( f.raw() ) );
		values.push( m->call( thread, args, p ) );
	} else if ( t == HHuginn::TYPE::UNBOUND_METHOD ) {
		HHuginn::HClass::HUnboundMethod* m( static_cast<HHuginn::HClass::HUnboundMethod*>( f.raw() ) );
		values.push( m->call( thread, args, p ) );
	} else if ( t == HHuginn::TYPE::METHOD ) {
		HHuginn::HClass::HMethod* m( static_cast<HHuginn::HClass::HMethod*>( f.raw() ) );
		HHuginn::value_t v( yaal::move( values.top() ) );
		values.pop();
		values.push( m->function()( thread, &v, args, p ) );
	} else {
		HHuginn::HObject* o( nullptr );
		if ( ( o = dynamic_cast<HHuginn::HObject*>( f.raw() ) ) ) {
			values.push( o->call_method( thread, f, IDENTIFIER::INTERFACE::CALL, args, executionStep_._position ) );
		} else {
			int idx( c->field_index( IDENTIFIER::INTERFACE::CALL ) );
			if ( idx >= 0 ) {
				HHuginn::HClass::HMethod const& m( *static_cast<HHuginn::HClass::HMethod const*>( c->field( idx ).raw() ) );
				values.push( m.function()( thread, &f, args, executionStep_._position ) );
			} else {
				throw HHuginn::HHuginnRuntimeException(
					"Reference `"_ys.append( c->name() ).append( "` is not a callable." ),
					file_id(),
					executionStep_._position
				);
			}
		}
	}
	return;
	M_EPILOG
}

void HExpression::create_closure( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	HFrame::values_t& values( frame_->values() );
	HHuginn::value_t f( yaal::move( values.top() ) );
	values.pop();
	HArguments args( frame_ );
	grab_args( frame_, args );
	++ frame_->ip();
	M_ASSERT( f->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE );
	HObjectFactory& of( frame_->thread()->object_factory() );
	HHuginn::value_t closure( of.create_object( of.none_class(), args ) );
	values.push( of.create_bound_method( static_cast<HHuginn::HFunctionReference*>( f.raw() )->function(), closure ) );
	return;
	M_EPILOG
}

namespace {

struct ValuePosition {
	HHuginn::value_t _value;
	int _position;
	ValuePosition( HHuginn::value_t const& val_, int pos_ )
		: _value( val_ ), _position( pos_ ) {
	}
};
typedef yaal::hcore::HArray<ValuePosition> positioned_values_t;

template <typename assoc_t>
inline HHuginn::value_t fill_assoc( HThread* thread_, HHuginn::value_t&& assoc_, positioned_values_t const& values_ ) {
	assoc_t* dict( static_cast<assoc_t*>( assoc_.raw() ) );
	for ( int i( 0 ), S( static_cast<int>( values_.get_size() ) ); i < S; i += 2 ) {
		dict->insert( thread_, values_[i]._value, values_[i + 1]._value, values_[i]._position );
	}
	return ( yaal::move( assoc_ ) );
}

}

void HExpression::make_assoc( OPERATOR op_, HFrame* frame_, int ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	positioned_values_t values;
	while ( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT ) {
		values.emplace_back( yaal::move( frame_->values().top() ), _instructions[ip]._position );
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		frame_->values().pop();
	}
	M_ASSERT( ( values.get_size() % 2 ) == 0 );
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_CALL );
	++ ip;
	reverse( values.begin(), values.end() );
	HThread* t( frame_->thread() );
	HHuginn::value_t v(
		op_ == OPERATOR::MAKE_DICT
		? fill_assoc<HHuginn::HDict>( t, t->object_factory().create_dict(), values )
		: fill_assoc<HHuginn::HLookup>( t, t->object_factory().create_lookup(), values )
	);
	frame_->values().push( v );
	return;
	M_EPILOG
}

void HExpression::plus( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::PLUS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::PLUS ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	instruction::add( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::minus( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MINUS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::MINUS ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	instruction::sub( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::mul( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MULTIPLY );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::MULTIPLY ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	instruction::mul( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::div( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::DIVIDE );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::DIVIDE ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	instruction::div( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::mod( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::MODULO );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t& v1( frame_->values().top() );
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::MODULO ), c1, c2, file_id(), p );
	}
	if ( ! v1.unique() ) {
		v1 = v1->clone( frame_->thread(), &v1, p );
	}
	instruction::mod( frame_->thread(), v1, v2, p );
	return;
	M_EPILOG
}

void HExpression::factorial( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( ! frame_->values().is_empty() );
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	frame_->values().push( instruction::factorial( frame_->thread(), v, executionStep_._position ) );
	return;
	M_EPILOG
}

void HExpression::negate( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::NEGATE );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	M_ASSERT( ! frame_->values().is_empty() );
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	frame_->values().push( instruction::neg( frame_->thread(), v, p ) );
	return;
	M_EPILOG
}

void HExpression::power( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	while ( ( ip < static_cast<int>( _instructions.get_size() ) ) && ( _instructions[ip]._operator == OPERATOR::POWER ) ) {
		int p( _instructions[ip]._position );
		++ ip;
		HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
		frame_->values().pop();
		HHuginn::value_t& v1( frame_->values().top() );
		HHuginn::HClass const* c1( v1->get_class() );
		HHuginn::HClass const* c2( v2->get_class() );
		if ( c1 != c2 ) {
			operands_type_mismatch( op_to_str( OPERATOR::POWER ), c1, c2, file_id(), p );
		}
		if ( ! v1.unique() ) {
			v1 = v1->clone( frame_->thread(), &v1, p );
		}
		instruction::pow( frame_->thread(), v1, v2, p );
	}
	M_ASSERT( ( ip < static_cast<int>( _instructions.get_size() ) ) && (  _instructions[ip]._operator == OPERATOR::POWER_TERM ) );
	++ ip;
	return;
	M_EPILOG
}

void HExpression::subscript( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[ip]._operator == OPERATOR::FUNCTION_ARGUMENT );
	++ ip;
	HHuginn::value_t index( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[ip]._operator == OPERATOR::SUBSCRIPT );
	int p( _instructions[ip]._position );
	++ ip;
	HHuginn::value_t base( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	frame_->values().push( instruction::subscript( frame_->thread(), executionStep_._access, base, index, p ) );
	return;
	M_EPILOG
}

void HExpression::range( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	int& ip( frame_->ip() );
	M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
	HHuginn::value_t from;
	HHuginn::value_t to;
	HHuginn::value_t step;
	HHuginn::value_t* v( &step );
	OPERATOR op( _instructions[ip]._operator );
	M_ASSERT( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) );
	int rangeOpCount( 0 );
	while ( ( op == OPERATOR::FUNCTION_ARGUMENT ) || ( op == OPERATOR::SUBSCRIPT_ARGUMENT ) ) {
		if ( op == OPERATOR::FUNCTION_ARGUMENT ) {
			*v = yaal::move( frame_->values().top() );
			frame_->values().pop();
		} else {
			if ( rangeOpCount == 0 ) {
				v = &to;
			} else {
				v = &from;
			}
			++ rangeOpCount;
		}
		++ ip;
		M_ASSERT( ip < static_cast<int>( _instructions.get_size() ) );
		op = _instructions[ip]._operator;
	}
	M_ASSERT( rangeOpCount <= 2 );
	M_ASSERT( op == OPERATOR::SUBSCRIPT );
	int p( _instructions[ip]._position );
	++ ip;
	bool select( ( !! from ) || ( !! to ) || ( !! step ) );
	HHuginn::value_t base( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	if ( ! select && ( rangeOpCount > 0 ) ) {
		HHuginn::type_id_t t( base->type_id() );
		if ( ( t != HHuginn::TYPE::TUPLE ) && ( t != HHuginn::TYPE::LIST ) && ( t != HHuginn::TYPE::DEQUE ) && ( t != HHuginn::TYPE::STRING ) ) {
			throw HHuginn::HHuginnRuntimeException( "Range operator not supported on `"_ys.append( base->get_class()->name() ).append( "`." ), file_id(), p );
		}
		frame_->values().push( yaal::move( base ) );
	} else {
		if ( rangeOpCount == 1 ) {
			from = yaal::move( to );
			to = yaal::move( step );
		}
		frame_->values().push( instruction::range( frame_->thread(), base, from, to, step, p ) );
	}
	return;
	M_EPILOG
}

void HExpression::equals( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::EQUALS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( ( c1 != c2 ) && ( c1->type_id() != HHuginn::TYPE::NONE ) && ( c2->type_id() != HHuginn::TYPE::NONE ) ) {
		operands_type_mismatch( op_to_str( OPERATOR::EQUALS ), c1, c2, file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( instruction::equals( frame_->thread(), v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::not_equals( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::NOT_EQUALS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( ( c1 != c2 ) && ( c1->type_id() != HHuginn::TYPE::NONE ) && ( c2->type_id() != HHuginn::TYPE::NONE ) ) {
		operands_type_mismatch( op_to_str( OPERATOR::NOT_EQUALS ), c1, c2, file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( ! instruction::equals( frame_->thread(), v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::less( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::LESS );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::LESS ), c1, c2, file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( instruction::less( frame_->thread(), v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::greater( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::GREATER );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::GREATER ), c1, c2, file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( instruction::greater( frame_->thread(), v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::less_or_equal( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::LESS_OR_EQUAL );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::LESS_OR_EQUAL ), c1, c2, file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( instruction::less_or_equal( frame_->thread(), v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::greater_or_equal( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::GREATER_OR_EQUAL );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c1( v1->get_class() );
	HHuginn::HClass const* c2( v2->get_class() );
	if ( c1 != c2 ) {
		operands_type_mismatch( op_to_str( OPERATOR::GREATER_OR_EQUAL ), c1, c2, file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( instruction::greater_or_equal( frame_->thread(), v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::is_element_of( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::IS_ELEMENT_OF );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c2( v2->get_class() );
	if ( ( c2->type_id() <= type_id( HHuginn::TYPE::UNKNOWN ) ) && ! is_collection_like( c2 ) ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_COLL] ).append( a_type_name( c2 ) ), file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( instruction::is_element_of( frame_->thread(), OPERATOR::IS_ELEMENT_OF, v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::is_not_element_of( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::IS_NOT_ELEMENT_OF );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::HClass const* c2( v2->get_class() );
	if ( ( c2->type_id() <= type_id( HHuginn::TYPE::UNKNOWN ) ) && ! is_collection_like( c2 ) ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_COLL] ).append( a_type_name( c2 ) ), file_id(), p );
	}
	frame_->values().push( frame_->thread()->runtime().boolean_value( ! instruction::is_element_of( frame_->thread(), OPERATOR::IS_NOT_ELEMENT_OF, v1, v2, p ) ) );
	return;
	M_EPILOG
}

void HExpression::boolean_and( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_AND );
	++ frame_->ip();
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HBooleanEvaluator*>( v.raw() ) );
	frame_->values().push( frame_->thread()->runtime().boolean_value( static_cast<HBooleanEvaluator*>( v.raw() )->execute( frame_->thread() ) ) );
	return;
	M_EPILOG
}

void HExpression::boolean_or( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_OR );
	++ frame_->ip();
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HBooleanEvaluator*>( v.raw() ) );
	frame_->values().push( frame_->thread()->runtime().boolean_value( static_cast<HBooleanEvaluator*>( v.raw() )->execute( frame_->thread() ) ) );
	return;
	M_EPILOG
}

void HExpression::boolean_xor( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_XOR );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t v2( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	HHuginn::value_t v1( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	if ( ( v1->type_id() != HHuginn::TYPE::BOOLEAN ) || ( v2->type_id() != HHuginn::TYPE::BOOLEAN ) ) {
		throw HHuginn::HHuginnRuntimeException(
			hcore::to_string( _errMsgHHuginn_[ERR_CODE::OPS_NOT_BOOL] )
				.append( a_type_name( v1->get_class() ) )
				.append( ", " )
				.append( a_type_name( v2->get_class() ) ),
			file_id(),
			p
		);
	}
	frame_->values().push( instruction::boolean_xor( frame_->thread(), v1, v2, p ) );
	return;
	M_EPILOG
}

void HExpression::boolean_not( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::BOOLEAN_NOT );
	int p( _instructions[frame_->ip()]._position );
	++ frame_->ip();
	HHuginn::value_t& v( frame_->values().top() );
	if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
		throw HHuginn::HHuginnRuntimeException( hcore::to_string( _errMsgHHuginn_[ERR_CODE::OP_NOT_BOOL] ).append( v->get_class()->name() ), file_id(), p );
	}
	v = instruction::boolean_not( frame_->thread(), v, p );
	return;
	M_EPILOG
}

void HExpression::ternary( OExecutionStep const&, HFrame* frame_ ) {
	M_PROLOG
	M_ASSERT( frame_->ip() < static_cast<int>( _instructions.get_size() ) );
	M_ASSERT( _instructions[frame_->ip()]._operator == OPERATOR::TERNARY );
	++ frame_->ip();
	HHuginn::value_t v( yaal::move( frame_->values().top() ) );
	frame_->values().pop();
	M_ASSERT( dynamic_cast<HHuginn::HTernaryEvaluator*>( v.raw() ) );
	frame_->values().push( static_cast<HHuginn::HTernaryEvaluator*>( v.raw() )->execute( frame_->thread() ) );
	return;
	M_EPILOG
}

void HExpression::store_direct( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( executionStep_._value );
	return;
	M_EPILOG
}

void HExpression::store_external_reference( OExecutionStep const& executionStep_, huginn::HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( *executionStep_._valueRef );
	return;
	M_EPILOG
}

void HExpression::store_real( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_real( executionStep_._real ) );
	return;
	M_EPILOG
}

void HExpression::store_integer( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_integer( executionStep_._integer ) );
	return;
	M_EPILOG
}

void HExpression::store_string( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->runtime().object_factory()->create_string( executionStep_._string ) );
	return;
	M_EPILOG
}

void HExpression::store_number( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_number( executionStep_._string ) );
	return;
	M_EPILOG
}

void HExpression::store_character( OExecutionStep const& executionStep_, HFrame* frame_ ) {
	M_PROLOG
	frame_->values().push( frame_->thread()->object_factory().create_character( executionStep_._character ) );
	return;
	M_EPILOG
}

void HExpression::do_execute( huginn::HThread* thread_ ) const {
	try {
		M_ASSERT( _operations.is_empty() );
		HFrame* f( thread_->current_frame() );
		f->start_expression();
		try {
			for ( OExecutionStep const& e : _executionSteps ) {
				( e._expression->*e._action )( e, f );
				if ( ! thread_->can_continue() ) {
					break;
				}
			}
		} catch ( instruction::Interrupt const& ) {
		}
		HFrame::STATE s( f->state() );
		if ( s < HFrame::STATE::EXCEPTION ) {
			f->set_result( yaal::move( f->values().top() ) );
			M_ASSERT( f->ip() == static_cast<int>( _instructions.get_size() ) );
			f->values().pop();
		} else {
			f->values().clear();
		}
		f->end_expression();
		return;
	} catch ( HHuginn::HHuginnRuntimeException const& e ) {
		if ( e.position() <= 0 ) {
			throw HHuginn::HHuginnRuntimeException( e.message(), file_id(), position() );
		} else {
			throw;
		}
	M_EPILOG
}

}

}

}

