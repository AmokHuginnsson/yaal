/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/list.hxx
 * \brief Declaration of huginn::HList class.
 */

#ifndef YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED 1

#include "tools/huginn/iterable.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HList : public HInvalidatingIterable {
public:
	typedef HList this_type;
	typedef HInvalidatingIterable base_type;
	typedef yaal::hcore::HArray<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HList( HClass const*, values_t&& );
	void push_back( value_t const& );
	void pop_back( void );
	void clear( void );
	int long find( huginn::HThread*, int, HHuginn::value_t const&, int long = 0, int long = -1 ) const;
	value_t get( int long long ) const;
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_contains( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual void do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual HHuginn::value_t do_operator_subscript( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual void do_operator_subscript_assign( HThread*, HHuginn::value_t&, HHuginn::value_t const&, HHuginn::value_t&&, int ) override;
	virtual HHuginn::value_t do_operator_range( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual hash_value_t do_operator_hash( HThread*, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

namespace list {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t reversed_view( huginn::HThread*, HHuginn::value_t const& );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED */

