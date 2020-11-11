/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "function.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"
#include "objectfactory.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HFunction::HFunction(
	HHuginn::identifier_id_t name_,
	parameter_names_t const& parameterNames_,
	HHuginn::scope_t const& scope_,
	expressions_t const& defaultValues_,
	bool isVariadic_,
	bool capturesNamedParameters_
) : HStatement( scope_->id(), scope_->file_id(), scope_->range() )
	, _name( name_ )
	, _parameterNames( parameterNames_ )
	, _defaultParametersStart( static_cast<int>( parameterNames_.get_size() - defaultValues_.get_size() ) )
	, _defaultValues( defaultValues_ )
	, _scope( scope_ )
	, _parameterCount( static_cast<int>( _parameterNames.get_size() ) )
	, _isVariadic( isVariadic_ )
	, _capturesNamedParameters( capturesNamedParameters_ ) {
	if ( _isVariadic ) {
		-- _defaultParametersStart;
		-- _parameterCount;
	}
	if ( _capturesNamedParameters ) {
		-- _defaultParametersStart;
		-- _parameterCount;
	}
	_scope->make_inline();
	return;
}

int HFunction::upcast( HHuginn::value_t* object_ ) const {
	M_PROLOG
	int upCast( 0 );
	HObject* object( object_ ? static_cast<HObject*>( object_->raw() ) : nullptr );
	if ( object_ && ( object->field_index( _name ) >= 0 ) ) {
		HClass const* c( object->get_class() );
		while ( c ) {
			int idx( c->field_index( _name ) );
			if ( ( idx >= 0 ) && ( c->function( idx ).id() == this ) ) {
				break;
			}
			c = c->super();
			++ upCast;
		}
		if ( ! c ) {
			upCast = 0;
		}
	}
	return ( upCast );
	M_EPILOG
}

HHuginn::value_t HFunction::execute(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) const {
	M_PROLOG
	thread_->create_function_frame( this, object_, upcast( object_ ) );
	HHuginn::value_t res( execute_impl( thread_, values_, position_ ) );
	thread_->pop_frame();
	return ( res );
	M_EPILOG
}

HHuginn::value_t HFunction::execute_destructor(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) const {
	M_PROLOG
	HThread::STATE state( thread_->state() );
	thread_->state_set( HThread::STATE::NORMAL );
	HHuginn::value_t res( execute( thread_, object_, values_, position_ ) );
	if ( thread_->state() == HThread::STATE::EXCEPTION ) {
		thread_->flush_uncaught_exception( " from destructor" );
	}
	thread_->state_set( state );
	return ( res );
	M_EPILOG
}

HHuginn::value_t HFunction::execute_incremental_main(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t&,
	int
) const {
	M_PROLOG
	thread_->create_incremental_function_frame( this, object_, upcast( object_ ) );
	HHuginn::value_t res( execute_incremental_main_impl( thread_ ) );
	thread_->pop_incremental_frame();
	return ( res );
	M_EPILOG
}

HHuginn::value_t HFunction::execute_impl(
	huginn::HThread* thread_,
	HHuginn::values_t& values_,
	int position_
) const {
	M_PROLOG
	HFrame* f( thread_->current_frame() );
	HHuginn::values_t& variables( f->variable_values() );
	variables.swap( values_ );
	HHuginn::value_t namedParameters;
	int positionalArgumentsCount( static_cast<int>( variables.get_size() ) );
	bool hasKeywordArguments( ! variables.is_empty() && ( variables.back()->type_id() == HHuginn::TYPE::NAMED_PARAMETERS ) );
	if ( hasKeywordArguments ) {
		namedParameters = yaal::move( static_cast<HTaggedValue*>( variables.back().raw() )->value() );
		variables.pop_back();
		-- positionalArgumentsCount;
	}
	if ( positionalArgumentsCount < _parameterCount ) {
		variables.resize( _parameterCount );
	}
	if ( hasKeywordArguments ) {
		huginn::HLookup::values_t& namedParametersData( static_cast<huginn::HLookup*>( namedParameters.raw() )->value() );
		HRuntime& r( thread_->runtime() );
		for ( huginn::HLookup::values_t::iterator it( namedParametersData.begin() ); it != namedParametersData.end(); ) {
			huginn::HLookup::values_t::iterator del( it );
			++ it;
			if ( del->first->type_id() != HHuginn::TYPE::STRING ) {
				throw HHuginn::HHuginnRuntimeException(
					"In call to `"_ys
						.append( thread_->runtime().identifier_name( _name ) )
						.append( "()`, a non-string name in named parameter." ),
					f->parent() ? f->parent()->file_id() : f->file_id(),
					position_
				);
			}
			hcore::HString const& keywordName( get_string( del->first ) );
			HHuginn::identifier_id_t keywordIdentifier( r.identifier_id( keywordName ) );
			parameter_names_t::const_iterator positionalPosition( find( _parameterNames.begin(), _parameterNames.end(), keywordIdentifier ) );
			if ( positionalPosition != _parameterNames.end() ) {
				int positionalIndex( static_cast<int>( positionalPosition - _parameterNames.begin() ) );
				if ( !! variables[positionalIndex] ) {
					throw HHuginn::HHuginnRuntimeException(
						"In call to `"_ys
							.append( thread_->runtime().identifier_name( _name ) )
							.append( "()`, positional argument `" )
							.append( keywordName )
							.append( "` was already set." ),
						f->parent() ? f->parent()->file_id() : f->file_id(),
						position_
					);
				}
				variables[positionalIndex] = del->second;
				++ positionalArgumentsCount;
				namedParametersData.erase( del );
			}
		}
	}
	for ( int i( _defaultParametersStart ); i < _parameterCount; ++ i ) {
		int defaultValueIndex( i - _defaultParametersStart );
		if ( ! variables[i] ) {
			variables[i] = _defaultValues[defaultValueIndex]->evaluate( thread_ );
			if ( ! thread_->can_continue() ) {
				break;
			}
			++ positionalArgumentsCount;
		}
	}
	if ( _isVariadic ) {
		HHuginn::value_t v( thread_->object_factory().create_tuple() );
		huginn::HTuple::values_t& variadic( static_cast<huginn::HTuple*>( v.raw() )->value() );
		variadic.reserve( max( positionalArgumentsCount - _parameterCount, 0 ) );
		for ( int i( _parameterCount ); i < positionalArgumentsCount; ++ i ) {
			variadic.push_back( variables[i] );
		}
		if ( positionalArgumentsCount > _parameterCount ) {
			variables.resize( _parameterCount );
			positionalArgumentsCount = _parameterCount;
		}
		variables.push_back( v );
	}
	if ( _capturesNamedParameters ) {
		if ( ! namedParameters ) {
			namedParameters = thread_->object_factory().create_lookup();
		}
		variables.push_back( namedParameters );
	} else if ( !! namedParameters && ! static_cast<huginn::HLookup*>( namedParameters.raw() )->value().is_empty() ) {
		throw HHuginn::HHuginnRuntimeException(
			"Call to `"_ys
				.append( thread_->runtime().identifier_name( _name ) )
				.append( "()` got unexpected keyword arguments." ),
			f->parent() ? f->parent()->file_id() : f->file_id(),
			position_
		);
	}
	if ( positionalArgumentsCount < _parameterCount ) {
		parameter_names_t missing;
		for ( int i( 0 ); i < _defaultParametersStart; ++ i ) {
			if ( ! variables[i] ) {
				missing.push_back( _parameterNames[i] );
			}
		}
		hcore::HString missingList;
		bool first( true );
		HRuntime& r( thread_->runtime() );
		for ( HHuginn::identifier_id_t m : missing ) {
			if ( ! first ) {
				missingList.append( m == missing.back() ? " and " : ", " );
			}
			missingList.append( '`' ).append( r.identifier_name( m ) ).append( '`' );
			first = false;
		}
		hcore::HString msg( "In call to `" );
		msg.append( r.identifier_name( _name ) )
			.append( "()`, missing required positional argument" )
			.append( missing.get_size() > 1 ? "s: " : ": " )
			.append( missingList ).append( "." );
		throw HHuginn::HHuginnRuntimeException( msg, f->parent() ? f->parent()->file_id() : f->file_id(), position_ );
	} else if ( positionalArgumentsCount > _parameterCount ) {
		if ( _defaultParametersStart != _parameterCount ) {
			throw HHuginn::HHuginnRuntimeException(
				"Function `"_ys
					.append( thread_->runtime().identifier_name( _name ) )
					.append( "()` accepts from " )
					.append( _defaultParametersStart )
					.append( " to " )
					.append( _parameterCount )
					.append( " positional arguments, but " )
					.append( positionalArgumentsCount )
					.append( " were given." ),
				f->parent() ? f->parent()->file_id() : f->file_id(),
				position_
			);
		} else {
			throw HHuginn::HHuginnRuntimeException(
				"Function `"_ys
					.append( thread_->runtime().identifier_name( _name ) )
					.append( "()` accepts exactly " )
					.append( _parameterCount )
					.append( " positional argument" )
					.append( _parameterCount == 1 ? "" : "s" )
					.append(", but " )
					.append( positionalArgumentsCount )
					.append( " were given." ),
				f->parent() ? f->parent()->file_id() : f->file_id(),
				position_
			);
		}
	}
	if ( thread_->can_continue() ) {
		_scope->execute( thread_ );
	}
	thread_->state_transition( HThread::STATE::RETURN, HThread::STATE::NORMAL );
	M_ASSERT(
		( thread_->state() != HThread::STATE::BREAK )
		&& ( thread_->state() != HThread::STATE::CONTINUE )
		&& ( thread_->state() != HThread::STATE::RETURN )
	);
	return ( f->result() );
	M_EPILOG
}

void HFunction::note_parameters( HThread* thread_ ) const {
	M_PROLOG
	HFrame* f( thread_->current_frame() );
	for ( HHuginn::identifier_id_t identifier : _parameterNames ) {
		f->note_variable( identifier );
	}
	return;
	M_EPILOG
}

}

}

}

