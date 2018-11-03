/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "range.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HRange::get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Range",
			"The `Range` class represents *lazy* *iterable* range of integer values.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "contains", runtime_->create_method( &HRange::contains ), "( *num* ) - test if a *num* is part of this `Range`" }
	};
	c->redefine( nullptr, fd );
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HRange::contains( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Range.contains", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HRange& range( *static_cast<HRange*>( object_->raw() ) );
	HHuginn::HInteger::value_type elem( get_integer( values_[0] ) );
	HHuginn::HInteger::value_type from( range._from );
	HHuginn::HInteger::value_type stop( range._stop );
	HHuginn::HInteger::value_type step( range._step );
	if ( step < 0 ) {
		using yaal::swap;
		swap( from, stop );
		step = math::abs( step );
	}
	bool doesContain(
		( ( range._step > 0 ) ? ( elem >= from ) : ( elem > from ) )
		&& ( ( range._step > 0 ) ? ( elem < stop ) : ( elem <= stop ) )
		&& ( ( ( elem - ( range._from % step ) ) % step ) == 0 )
	);
	return ( doesContain ? thread_->runtime().true_value() : thread_->runtime().false_value() );
	M_EPILOG
}

HHuginn::value_t HRange::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	return ( thread_->object_factory().create<HRange>( HIterable::get_class(), _from, _stop, _step ) );
}

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

