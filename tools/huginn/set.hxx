/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/set.hxx
 * \brief Declaration of huginn::HSet class.
 */

#ifndef YAAL_TOOLS_HUGINN_SET_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SET_HXX_INCLUDED 1

#include "tools/huginn/iterable.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSet : public HInvalidatingIterable {
public:
	typedef HSet this_type;
	typedef HIterable base_type;
	typedef yaal::hcore::HHashSet<HHuginn::value_t, HValueHashHelper&, HValueHashHelper&> values_prototype_t;
	typedef yaal::hcore::HPool<values_prototype_t::node_size::value> pool_t;
	typedef allocator::shared_pool<values_prototype_t::node_type> allocator_t;
	typedef yaal::hcore::HHashSet<HHuginn::value_t, HValueHashHelper&, HValueHashHelper&, allocator_t> values_t;
private:
	mutable HValueHashHelper _helper;
	values_t _data;
public:
	HSet( HClass const*, allocator_t const& );
	void insert( huginn::HThread*, HHuginn::value_t const&, int );
	bool has_key( huginn::HThread*, HHuginn::value_t const&, int ) const;
	void erase( huginn::HThread*, HHuginn::value_t const&, int );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
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
	HSet( HSet const& ) = delete;
	HSet& operator = ( HSet const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual void do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual bool do_operator_contains( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual hash_value_t do_operator_hash( HThread*, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

namespace set {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t reversed_view( huginn::HThread*, HHuginn::value_t const& );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SET_HXX_INCLUDED */

