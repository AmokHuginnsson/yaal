/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/enumeral.hxx
 * \brief Declaration of huginn::HEnumeral class.
 */

#ifndef YAAL_TOOLS_HUGINN_ENUMERAL_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ENUMERAL_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HEnumeral : public HValue {
public:
	typedef HEnumeral this_type;
	typedef HValue base_type;
	typedef int value_type;
private:
	HHuginn::identifier_id_t _identifier;
	value_type _value;
public:
	HEnumeral( HClass const*, HHuginn::identifier_id_t, value_type );
	HHuginn::identifier_id_t const& identifier( void ) const {
		return ( _identifier );
	}
	value_type value( void ) const {
		return ( _value );
	}
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ENUMERAL_HXX_INCLUDED */

