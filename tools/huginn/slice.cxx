/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "slice.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSliceIterator : public HIteratorInterface {
	int long long _i;
	HSlice const* _slice;
	huginn::HIterable::iterator_t _impl;
public:
	HSliceIterator( int long long from_, HSlice const* slice_, huginn::HIterable::iterator_t const& impl_ )
		: _i( from_ )
		, _slice( slice_ )
		, _impl( impl_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _impl->value( thread_, position_ ) );
	}
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return (
			thread_->can_continue()
			&& ( _i < _slice->stop() )
			&& _impl->is_valid( thread_, position_ )
		);
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		for ( int long long i( 0 ), count( _slice->step() ); ( i < count ) && do_is_valid( thread_, position_ ); ++ i, ++ _i ) {
			_impl->next( thread_, position_ );
		}
	}
private:
	HSliceIterator( HSliceIterator const& ) = delete;
	HSliceIterator& operator = ( HSliceIterator const& ) = delete;
};

HSlice::iterator_t HSlice::do_iterator( HThread* thread_, int position_ ) {
	M_PROLOG
	huginn::HIterable::iterator_t impl( static_cast<huginn::HIterable*>( _source.raw() )->iterator( thread_, position_ ) );
	for ( int long long i( 0 ); ( i < _from ) && impl->is_valid( thread_, position_ ); ++ i ) {
		impl->next( thread_, position_ );
	}
	return ( hcore::make_pointer<HSliceIterator>( _from, this, impl ) );
	M_EPILOG
}

HSlice::HSlice(
	huginn::HClass const* class_, HHuginn::value_t const& source_,
	int long long from_, int long long stop_, int long long step_
) : HIterable( class_ )
	, _source( source_ )
	, _from( from_ )
	, _stop( stop_ )
	, _step( step_ ) {
}

int long HSlice::do_size( huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	int long long s( static_cast<huginn::HIterable const*>( _source.raw() )->size( thread_, position_ ) );
	return ( safe_int::cast<int long>( ( min( s, _stop ) + _step - ( _from + 1 ) ) / _step ) );
	M_EPILOG
}

HHuginn::value_t HSlice::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	HHuginn::value_t s( _source->clone( thread_, const_cast<HHuginn::value_t*>( &_source ), position_ ) );
	return ( thread_->object_factory().create<HSlice>( HIterable::get_class(), s, _from, _stop, _step ) );
	M_EPILOG
}

HHuginn::class_t HSlice::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Slice",
			"The `Slice` class represents *lazy* *iterable* *Range*d selection of elements from another *iterable*.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
	M_EPILOG
}

}

}

}

