/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/integer.hxx
 * \brief Declaration of huginn::HInteger class.
 */

#ifndef YAAL_TOOLS_HUGINN_INTEGER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_INTEGER_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HInteger : public HValue {
public:
	typedef HInteger this_type;
	typedef HValue base_type;
	typedef int long long value_type;
private:
	value_type _value;
public:
	HInteger( HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_INTEGER_HXX_INCLUDED */

