/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/real.hxx
 * \brief Declaration of huginn::HReal class.
 */

#ifndef YAAL_TOOLS_HUGINN_REAL_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_REAL_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HReal : public HValue {
public:
	typedef HReal this_type;
	typedef HValue base_type;
	typedef double long value_type;
private:
	value_type _value;
public:
	HReal( HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual void do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_multiply( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_divide( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_modulo( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_power( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual HHuginn::value_t do_operator_modulus( HThread*, HHuginn::value_t const&, int ) const override;
	virtual HHuginn::value_t do_operator_negate( HThread*, HHuginn::value_t const&, int ) const override;
	virtual hash_value_t do_operator_hash( HThread*, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_REAL_HXX_INCLUDED */

