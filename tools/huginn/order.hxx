/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/order.hxx
 * \brief Declaration of huginn::HOrder class.
 */

#ifndef YAAL_TOOLS_HUGINN_ORDER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ORDER_HXX_INCLUDED 1

#include "tools/huginn/iterable.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HOrder : public HInvalidatingIterable {
public:
	typedef HOrder this_type;
	typedef HInvalidatingIterable base_type;
	typedef yaal::hcore::HSet<HHuginn::value_t, HValueCompareHelper&> values_prototype_t;
	typedef yaal::hcore::HPool<values_prototype_t::node_size::value> pool_t;
	typedef allocator::shared_pool<values_prototype_t::node_type> allocator_t;
	typedef yaal::hcore::HSet<HHuginn::value_t, HValueCompareHelper&, allocator_t> values_t;
private:
	mutable HValueCompareHelper _helper;
	values_t _data;
	HClass const* _keyType;
public:
	HOrder( HClass const*, allocator_t const& );
	void insert( huginn::HThread*, HHuginn::value_t const&, int );
	bool has_key( huginn::HThread*, HHuginn::value_t const&, int ) const;
	void erase( huginn::HThread*, HHuginn::value_t const&, int );
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
	HOrder( HOrder const& ) = delete;
	HOrder& operator = ( HOrder const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_contains( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

namespace order {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t reversed_view( huginn::HThread*, HHuginn::value_t const& );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ORDER_HXX_INCLUDED */

