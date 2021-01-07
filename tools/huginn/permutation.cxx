/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "permutation.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

HHuginn::class_t HPermutation::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Permutation",
			"The `Permutation` class represents a *lazy* view of a list of all permutations of a given data set.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HPermutation::HPermutation( huginn::HClass const* class_, HHuginn::values_t&& dataSet_, HHuginn::TYPE type_ )
	: HIterable( class_ )
	, _dataSet( dataSet_ )
	, _type( type_ ) {
}

HHuginn::value_t HPermutation::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	HHuginn::values_t dataSet;
	for ( HHuginn::values_t::value_type const& v : _dataSet ) {
		dataSet.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
	}
	return (
		thread_->object_factory().create<HPermutation>(
			HIterable::get_class(),
			yaal::move( dataSet ),
			_type
		)
	);
	M_EPILOG
}

class HPermutationIterator : public HIteratorInterface {
protected:
	HHuginn::values_t _dataSet;
	HHuginn::TYPE _type;
	bool _valid;
public:
	HPermutationIterator( HHuginn::values_t const& dataSet_, HHuginn::TYPE type_ )
		: _dataSet( dataSet_ )
		, _type( type_ )
		, _valid( true ) {
		return;
	}
protected:
	virtual bool do_is_valid( HThread* thread_, int ) override {
		return ( thread_->can_continue() && _valid );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		HValueCompareHelper less( &instruction::checked_less );
		less.anchor( thread_, position_ );
		_valid = next_permutation( _dataSet.begin(), _dataSet.end(), cref( less ) );
		return;
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int ) override {
		HHuginn::value_t v;
		switch ( _type ) {
			case ( HHuginn::TYPE::TUPLE ): {
				HHuginn::values_t dataSet( _dataSet );
				v = thread_->object_factory().create_tuple( yaal::move( dataSet ) );
			} break;
			case ( HHuginn::TYPE::LIST ): {
				HHuginn::values_t dataSet( _dataSet );
				v = thread_->object_factory().create_list( yaal::move( dataSet ) );
			} break;
			case ( HHuginn::TYPE::DEQUE ): {
				HDeque::values_t dataSet;
				for ( HHuginn::value_t const& e : _dataSet ) {
					dataSet.push_back( e );
				}
				v = thread_->object_factory().create_deque( yaal::move( dataSet ) );
			} break;
			default: {
				M_ASSERT( !"bad code path"[0] );
			} break;
		}
		return ( v );
	}
private:
	HPermutationIterator( HPermutationIterator const& ) = delete;
	HPermutationIterator& operator = ( HPermutationIterator const& ) = delete;
private:
};

HPermutation::iterator_t HPermutation::do_iterator( HThread* thread_, int position_ ) {
	HValueCompareHelper less( &instruction::checked_less );
	less.anchor( thread_, position_ );
	sort( _dataSet.begin(), _dataSet.end(), cref( less ) );
	return ( yaal::hcore::make_pointer<HPermutationIterator>( _dataSet, _type ) );
}

}

}

}

