/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/boolean.hxx
 * \brief Declaration of huginn::HBoolean class.
 */

#ifndef YAAL_TOOLS_HUGINN_BOOLEAN_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_BOOLEAN_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HBoolean : public HValue {
public:
	typedef HBoolean this_type;
	typedef HValue base_type;
	typedef bool value_type;
private:
	value_type _value;
public:
	HBoolean( HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual hash_value_t do_operator_hash( HThread*, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_BOOLEAN_HXX_INCLUDED */

