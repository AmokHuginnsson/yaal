/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/taggedvalue.hxx
 * \brief Declaration of huginn::HTaggedValue class.
 */

#ifndef YAAL_TOOLS_HUGINN_TAGGEDVALUE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_TAGGEDVALUE_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

/*! \brief Huginn Value tagger.
 *
 * Sole purpose of this class is to help with support for named parameters and variadic functions.
 */
class HTaggedValue : public HValue {
public:
	typedef HTaggedValue this_type;
	typedef HValue base_type;
private:
	value_t _value;
public:
	HTaggedValue( value_t const&, HClass const* );
	value_t const& value( void ) const {
		return ( _value );
	}
	value_t& value( void ) {
		return ( _value );
	}
private:
	HTaggedValue( HTaggedValue const& ) = delete;
	HTaggedValue& operator = ( HTaggedValue const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_TAGGEDVALUE_HXX_INCLUDED */

