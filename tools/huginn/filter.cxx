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

HHuginn::class_t HFilter::get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
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
	return ( c );
	M_EPILOG
}

int long HFilter::do_size( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Getting size of `Filter' is an invalid operation.", thread_->current_frame()->file_id(), position_ );
}

HHuginn::value_t HFilter::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	return ( thread_->object_factory().create<HFilter>( HIterable::get_class(), _source->clone( thread_, const_cast<HHuginn::value_t*>( &_source ), position_ ), _function, _method ) );
}

class HFilterIterator : public HIteratorInterface {
protected:
	HHuginn::HIterable::iterator_t _impl;
	HHuginn::value_t _value;
public:
	HFilterIterator( HHuginn::HIterable::iterator_t&& iterator_ )
		: _impl( yaal::move( iterator_ ) )
		, _value() {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( _impl->is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl->next( thread_, position_ );
		scan( thread_, position_ );
	}
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _value );
	}
	void scan( HThread* thread_, int position_ ) {
		while ( _impl->is_valid( thread_, position_ ) ) {
			_value = _impl->value( thread_, position_ );
			if ( do_test( thread_, position_ ) ) {
				break;
			}
			_impl->next( thread_, position_ );
		}
	}
	virtual bool do_test( HThread*, int ) = 0;
private:
	HFilterIterator( HFilterIterator const& ) = delete;
	HFilterIterator& operator = ( HFilterIterator const& ) = delete;
};

class HFunctionFilterIterator : public HFilterIterator {
	HHuginn::function_t const& _function;
public:
	HFunctionFilterIterator( HHuginn::HIterable::iterator_t&& iterator_, HHuginn::function_t const& function_, HThread* thread_, int position_ )
		: HFilterIterator( yaal::move( iterator_ ) )
		, _function( function_ ) {
		scan( thread_, position_ );
		return;
	}
protected:
	virtual bool do_test( HThread* thread_, int position_ ) override {
		HHuginn::value_t v( _function( thread_, nullptr, HArguments( thread_, _value ), position_ ) );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( "Filter function returned wrong type, expected `boolean' got: `" ).append( v->get_class()->name() ).append( "'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		return ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() );
	}
};

class HUnboundMethodFilterIterator : public HFilterIterator {
	HHuginn::HClass::HUnboundMethod& _method;
public:
	HUnboundMethodFilterIterator( HHuginn::HIterable::iterator_t&& iterator_, HHuginn::HClass::HUnboundMethod& method_, HThread* thread_, int position_ )
		: HFilterIterator( yaal::move( iterator_ ) )
		, _method( method_ ) {
		scan( thread_, position_ );
		return;
	}
protected:
	virtual bool do_test( HThread* thread_, int position_ ) override {
		HHuginn::value_t v( _method.call( thread_, HArguments( thread_, _impl->value( thread_, position_ ) ), position_ ) );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( "Filter functor returned wrong type, expected `boolean' got: `" ).append( v->get_class()->name() ).append( "'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		return ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() );
	}
};

class HBoundMethodFilterIterator : public HFilterIterator {
	HHuginn::HClass::HBoundMethod& _method;
public:
	HBoundMethodFilterIterator( HHuginn::HIterable::iterator_t&& iterator_, HHuginn::HClass::HBoundMethod& method_, HThread* thread_, int position_ )
		: HFilterIterator( yaal::move( iterator_ ) )
		, _method( method_ ) {
		scan( thread_, position_ );
		return;
	}
protected:
	virtual bool do_test( HThread* thread_, int position_ ) override {
		HHuginn::value_t v( _method.call( thread_, HArguments( thread_, _impl->value( thread_, position_ ) ), position_ ) );
		if ( v->type_id() != HHuginn::TYPE::BOOLEAN ) {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( "Filter functor returned wrong type, expected `boolean' got: `" ).append( v->get_class()->name() ).append( "'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		return ( static_cast<HHuginn::HBoolean*>( v.raw() )->value() );
	}
};

HFilter::iterator_t HFilter::do_iterator( HThread* thread_, int position_ ) {
	iterator_t impl;
	if ( !! _function ) {
		impl = hcore::make_pointer<HFunctionFilterIterator>( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), _function, thread_, position_ );
	} else if ( _method->type_id() == HHuginn::TYPE::UNBOUND_METHOD ) {
		impl = hcore::make_pointer<HUnboundMethodFilterIterator>( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), *static_cast<HHuginn::HClass::HUnboundMethod*>( _method.raw() ), thread_, position_ );
	} else {
		impl = hcore::make_pointer<HBoundMethodFilterIterator>( static_cast<HHuginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ), *static_cast<HHuginn::HClass::HBoundMethod*>( _method.raw() ), thread_, position_ );
	}
	return ( impl );
}

}

}

}

