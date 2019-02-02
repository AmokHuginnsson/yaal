/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "for.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"
#include "iterator.hxx"
#include "keyword.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HFor::HFor(
	HStatement::statement_id_t id_,
	HHuginn::expressions_t&& control_,
	HHuginn::expression_t const& source_,
	HHuginn::scope_t const& loop_,
	int fileId_,
	executing_parser::range_t range_
) : HStatement( id_, fileId_, range_ )
	, _control( yaal::move( control_ ) )
	, _source( source_ )
	, _loop( loop_ ) {
	_loop->make_inline();
	return;
}

void HFor::do_execute( HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame( this );
	HFrame* f( thread_->current_frame() );
	_source->execute( thread_ );
	int sourcePosition( _source->position() );
	if ( f->can_continue() ) {
		HHuginn::value_t source( f->result() );
		HHuginn::HIterable* coll( dynamic_cast<HHuginn::HIterable*>( source.raw() ) );
		HHuginn::HObject* obj( nullptr );
		if ( coll ) {
			HHuginn::HIterable::iterator_t it( coll->iterator( thread_, sourcePosition ) );
			while ( f->can_continue() && it->is_valid( thread_, sourcePosition ) ) {
				run_loop( thread_, f, it->value( thread_, sourcePosition ) );
				it->next( thread_, sourcePosition );
			}
		} else if ( ( obj = dynamic_cast<HHuginn::HObject*>( source.raw() ) ) ) {
			HHuginn::value_t itVal( obj->call_method( thread_, source, IDENTIFIER::INTERFACE::ITERATOR, HArguments( thread_ ), sourcePosition ) );
			HHuginn::HObject* it( dynamic_cast<HHuginn::HObject*>( itVal.raw() ) );
			if ( ! it ) {
				throw HHuginn::HHuginnRuntimeException( "`For' source returned invalid iterator object.", file_id(), sourcePosition );
			}
			HHuginn::value_t isValidField( it->get_method( thread_, itVal, IDENTIFIER::INTERFACE::IS_VALID, sourcePosition ) );
			HHuginn::HClass::HBoundMethod* isValidMethod( static_cast<HHuginn::HClass::HBoundMethod*>( isValidField.raw() ) );
			HHuginn::value_t valueField( it->get_method( thread_, itVal, IDENTIFIER::INTERFACE::VALUE, sourcePosition ) );
			HHuginn::HClass::HBoundMethod* valueMethod( static_cast<HHuginn::HClass::HBoundMethod*>( valueField.raw() ) );
			HHuginn::value_t nextField( it->get_method( thread_, itVal, IDENTIFIER::INTERFACE::NEXT, sourcePosition ) );
			HHuginn::HClass::HBoundMethod* nextMethod( static_cast<HHuginn::HClass::HBoundMethod*>( nextField.raw() ) );
			while ( f->can_continue() ) {
				HHuginn::value_t isValid( isValidMethod->call( thread_, HArguments( thread_ ), sourcePosition ) );
				if ( isValid->type_id() != HHuginn::TYPE::BOOLEAN ) {
					throw HHuginn::HHuginnRuntimeException( "`For' source iterator is_valid returned non-boolean value.", file_id(), sourcePosition );
				}
				if ( ! ( f->can_continue() && static_cast<HHuginn::HBoolean*>( isValid.raw() )->value() ) ) {
					break;
				}
				HHuginn::value_t value( valueMethod->call( thread_, HArguments( thread_ ), sourcePosition ) );
				if ( ! f->can_continue() ) {
					break;
				}
				run_loop( thread_, f, yaal::move( value ) );
				nextMethod->call( thread_, HArguments( thread_ ), sourcePosition );
			}
		} else {
			throw HHuginn::HHuginnRuntimeException( "`For' source is not an iterable.", file_id(), sourcePosition );
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

void HFor::run_loop( HThread* thread_, HFrame* frame_, HHuginn::value_t&& value_ ) const {
	M_PROLOG
	int cs( static_cast<int>( _control.get_size() ) );
	if ( cs == 1 ) {
		_control.front()->execute( thread_ );
		frame_->commit_variable( yaal::move( value_ ), _control.front()->position() );
	} else {
		if ( value_->type_id() != HHuginn::TYPE::TUPLE ) {
			throw HHuginn::HHuginnRuntimeException( "`For` source did not return a `tuple` object.", file_id(), _source->position() );
		}
		HHuginn::HTuple::values_t& srcData( static_cast<HHuginn::HTuple*>( value_.raw() )->value() );
		int ds( static_cast<int>( srcData.get_size() ) );
		if ( ds != cs ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( ds > cs ? "Too many" : "Not enough" )
					.append( " values to unpack, expected: " ).append( cs )
					.append( ", got: " ).append( ds ).append( "." ),
				file_id(),
				_source->position()
			);
		}
		int i( 0 );
		for ( HHuginn::expression_t const& control : _control ) {
			control->execute( thread_ );
			frame_->commit_variable( yaal::move( srcData[i ++] ), control->position() );
		}
	}
	if ( frame_->can_continue() ) {
		_loop->execute( thread_ );
	}
	frame_->continue_execution();
	return;
	M_EPILOG
}

}

}

}

