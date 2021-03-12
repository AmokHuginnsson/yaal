/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "product.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HProduct::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Product",
			"The `Product` class represents a *lazy* view of a Cartesian product of a set of *iterable*s.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
	M_EPILOG
}

HHuginn::value_t HProduct::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	axes_t axes;
	for ( axes_t::value_type const& a : _axes ) {
		axes_t::value_type axis;
		for ( HHuginn::value_t const& v : a ) {
			axis.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
		}
		axes.push_back( yaal::move( axis ) );
	}
	return (
		thread_->object_factory().create<HProduct>(
			HIterable::get_class(),
			yaal::move( axes )
		)
	);
	M_EPILOG
}

class HProductIterator : public HIteratorInterface {
	typedef yaal::hcore::HArray<int> indexes_t;
protected:
	HProduct::axes_t& _axes;
	indexes_t _indexes;
public:
	HProductIterator( HProduct::axes_t& source_ )
		: _axes( source_ )
		, _indexes( _axes.get_size(), 0 ) {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int ) override {
		HHuginn::values_t const& a( _axes.back() );
		return ( thread_->can_continue() && ( _indexes.back() < a.get_size() ) );
	}
	virtual void do_next( HThread*, int ) override {
		int axesCount( static_cast<int>( _indexes.get_size() ) );
		int a( axesCount - 1 );
		while ( a >= 0 ) {
			++ _indexes[a];
			if ( _indexes[a] < _axes[a].get_size() ) {
				for ( int i( a + 1 ); i < axesCount; ++ i ) {
					_indexes[i] = 0;
				}
				break;
			}
			-- a;
		}
		return;
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int ) override {
		HHuginn::values_t v;
		for ( int i( 0 ), count( static_cast<int>( _indexes.get_size() ) ); i < count; ++ i ) {
			v.push_back( _axes[i][_indexes[i]] );
		}
		return ( thread_->object_factory().create_tuple( yaal::move( v ) ) );
	}
private:
	HProductIterator( HProductIterator const& ) = delete;
	HProductIterator& operator = ( HProductIterator const& ) = delete;
private:
};

HProduct::iterator_t HProduct::do_iterator( HThread*, int ) {
	return ( yaal::hcore::make_pointer<HProductIterator>( _axes ) );
}

}

}

}

