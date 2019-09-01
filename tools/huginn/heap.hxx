/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/order.hxx
 * \brief Declaration of huginn::HHeap class.
 */

#ifndef YAAL_TOOLS_HUGINN_HEAP_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_HEAP_HXX_INCLUDED 1

#include "hcore/hheap.hxx"
#include "tools/huginn/iterable.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HHeap : public HInvalidatingIterable {
public:
	typedef HHeap this_type;
	typedef HInvalidatingIterable base_type;
	typedef yaal::hcore::HArray<HHuginn::value_t> sequence_t;
	typedef yaal::hcore::HHeap<HHuginn::value_t, sequence_t, HValueCompareHelper&> values_t;
private:
	mutable HValueCompareHelper _helper;
	values_t _data;
	HClass const* _keyType;
public:
	HHeap( HClass const* );
	void push( huginn::HThread*, HHuginn::value_t const&, int );
	HHuginn::value_t pop( huginn::HThread*, int );
	HHuginn::value_t top( huginn::HThread*, int );
	HClass const* key_type( void ) const;
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
	void verify_key_type( huginn::HThread*, HClass const*, int ) const;
	void update_key_type( huginn::HThread*, HClass const*, int );
	void clear( void );
	void anchor( huginn::HThread* thread_, int position_ ) const {
		_helper.anchor( thread_, position_ );
	}
	void detach( void ) const {
		_helper.detach();
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	HHeap( HHeap const& ) = delete;
	HHeap& operator = ( HHeap const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual void do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual hash_value_t do_operator_hash( HThread*, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

namespace heap {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_HEAP_HXX_INCLUDED */

