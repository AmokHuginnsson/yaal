/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRange : public HHuginn::HIterable {
	int long long _from;
	int long long _stop;
	int long long _step;
public:
	HRange( HHuginn::HClass const* class_, int long long from_, int long long stop_, int long long step_ )
		: HIterable( class_ )
		, _from( from_ )
		, _stop( stop_ )
		, _step( step_ ) {
	}
	int long long step( void ) const {
		return ( _step );
	}
	int long long stop( void ) const {
		return ( _stop );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"Range",
				"The `Range` class represents *lazy* *iterable* range of integer values.",
				HHuginn::ACCESS::PRIVATE
			)
		);
		c->set_origin( origin_ );
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread*, int ) const override {
		return ( safe_int::cast<int long>( ( _stop + _step - ( _from + 1 ) ) / _step ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HRange>( HIterable::get_class(), _from, _stop, _step ) );
	}
};

class HRangeIterator : public HIteratorInterface {
	int long long _i;
	HRange const* _range;
	HObjectFactory* _objectFactory;
public:
	HRangeIterator( int long long from_, HRange const* range_ )
		: _i( from_ )
		, _range( range_ )
		, _objectFactory( range_->HIterable::get_class()->runtime()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _objectFactory->create_integer( _i ) );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _range->step() > 0 ? _i < _range->stop() : _i > _range->stop() );
	}
	virtual void do_next( HThread*, int ) override {
		_i += _range->step();
	}
private:
	HRangeIterator( HRangeIterator const& ) = delete;
	HRangeIterator& operator = ( HRangeIterator const& ) = delete;
};

HRange::iterator_t HRange::do_iterator( HThread*, int ) {
	return ( hcore::make_pointer<HRangeIterator>( _from, this ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED */

