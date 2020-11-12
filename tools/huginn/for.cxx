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
	HHuginn::statement_id_t id_,
	int fileId_,
	executing_parser::range_t range_
) : HVirtualScope( id_, fileId_, range_ )
	, _control()
	, _source()
	, _loop() {
	return;
}

void HFor::init(
	HHuginn::expressions_t&& control_,
	HHuginn::expression_t const& source_,
	HHuginn::scope_t const& loop_
) {
	_control = yaal::move( control_ );
	_source = source_;
	_loop = loop_;
	return;
}

inline HHuginn::value_t ensure_virtual_collection(
	huginn::HThread* thread_,
	HHuginn::value_t&& value_,
	int position_
) {
	M_PROLOG
	HHuginn::value_t v( yaal::move( value_ ) );
	do {
		if ( dynamic_cast<huginn::HIterable const*>( v.raw() ) ) {
			break;
		}
		if (
			dynamic_cast<HObject*>( v.raw() )
			&& ( v->field_index( IDENTIFIER::INTERFACE::ITERATOR ) >= 0 )
		) {
			v = thread_->object_factory().create<HIterableAdaptor>( thread_->object_factory().iterable_adaptor_class(), v );
			break;
		}
		throw HHuginn::HHuginnRuntimeException( "`For` source is not an iterable.", thread_->file_id(), position_ );
	} while ( false );
	return ( v );
	M_EPILOG
}

void HFor::do_execute_internal( HThread* thread_ ) const {
	M_PROLOG
	HHuginn::value_t source( _source->evaluate( thread_ ) );
	int sourcePosition( _source->position() );
	if ( thread_->can_continue() ) {
		source = ensure_virtual_collection( thread_, yaal::move( source ), sourcePosition );
		huginn::HIterable* coll( static_cast<huginn::HIterable*>( source.raw() ) );
		huginn::HIterable::iterator_t it( coll->iterator( thread_, sourcePosition ) );
		while ( thread_->can_continue() && it->is_valid( thread_, sourcePosition ) ) {
			if ( thread_->can_continue() ) {
				run_loop( thread_, it->value( thread_, sourcePosition ) );
			}
			if ( thread_->can_continue() ) {
				it->next( thread_, sourcePosition );
			}
		}
	}
	thread_->state_unbreak();
	return;
	M_EPILOG
}

void HFor::run_loop( HThread* thread_, HHuginn::value_t&& value_ ) const {
	M_PROLOG
	int cs( static_cast<int>( _control.get_size() ) );
	if ( cs == 1 ) {
		_control.front()->commit( thread_, yaal::move( value_ ) );
	} else {
		if ( value_->type_id() != HHuginn::TYPE::TUPLE ) {
			throw HHuginn::HHuginnRuntimeException( "`For` source did not return a `tuple` object.", file_id(), _source->position() );
		}
		huginn::HTuple::values_t& srcData( static_cast<huginn::HTuple*>( value_.raw() )->value() );
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
			control->commit( thread_, yaal::move( srcData[i ++] ) );
		}
	}
	if ( thread_->can_continue() ) {
		_loop->execute_internal( thread_ );
	}
	thread_->state_transition( HThread::STATE::CONTINUE, HThread::STATE::NORMAL );
	return;
	M_EPILOG
}

}

}

}

