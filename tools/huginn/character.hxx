/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/character.hxx
 * \brief Declaration of huginn::HCharacter class.
 */

#ifndef YAAL_TOOLS_HUGINN_CHARACTER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_CHARACTER_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HCharacter : public HValue {
public:
	typedef HCharacter this_type;
	typedef HValue base_type;
	typedef yaal::code_point_t value_type;
private:
	value_type _value;
public:
	HCharacter( HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	void set( value_type );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_CHARACTER_HXX_INCLUDED */

