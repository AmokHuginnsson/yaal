/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "filter.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HFilter::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Filter",
			"The `Filter` class represents a *lazy* filtering transformation of one *iterable* into another. Resulting *iterable* produces only values matching supplied predicate.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
	M_EPILOG
}

int long HFilter::do_size( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Getting size of `Filter` is an invalid operation.", thread_->file_id(), position_ );
}

HHuginn::value_t HFilter::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	return ( thread_->object_factory().create<HFilter>( HIterable::get_class(), _source->clone( thread_, const_cast<HHuginn::value_t*>( &_source ), position_ ), _callable ) );
}

class HFilterIterator : public HIteratorInterface {
protected:
	huginn::HIterable::iterator_t _impl;
	HHuginn::value_t _value;
	HHuginn::value_t& _callable;
public:
	HFilterIterator( huginn::HIterable::iterator_t&& iterator_, HHuginn::value_t& callable_, HThread* thread_, int position_ )
		: _impl( yaal::move( iterator_ ) )
		, _value()
		, _callable( callable_ ) {
		scan( thread_, position_ );
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( thread_->can_continue() && _impl->is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl->next( thread_, position_ );
		scan( thread_, position_ );
	}
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _value );
	}
	void scan( HThread* thread_, int position_ ) {
		while ( thread_->can_continue() && _impl->is_valid( thread_, position_ ) ) {
			_value = _impl->value( thread_, position_ );
			if ( ! thread_->can_continue() || test( thread_, position_ ) ) {
				break;
			}
			_impl->next( thread_, position_ );
		}
	}
	bool test( HThread* thread_, int position_ ) {
		HHuginn::value_t v( _callable->operator_call( thread_, _callable, HArguments( thread_, _value ), position_ ) );
		if ( ! thread_->can_continue() ) {
			return ( false );
		}
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( "Filter functor returned wrong type, expected `boolean` got: `" ).append( v->get_class()->name() ).append( "`." ),
				thread_->file_id(),
				position_
			);
		}
		return ( static_cast<HBoolean*>( v.raw() )->value() );
	}
private:
	HFilterIterator( HFilterIterator const& ) = delete;
	HFilterIterator& operator = ( HFilterIterator const& ) = delete;
};

HFilter::iterator_t HFilter::do_iterator( HThread* thread_, int position_ ) {
	iterator_t impl( hcore::make_pointer<HFilterIterator>( static_cast<huginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _callable, thread_, position_ ) );
	return impl;
}

}

}

}

