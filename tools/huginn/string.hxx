/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/string.hxx
 * \brief Declaration of huginn::HString class.
 */

#ifndef YAAL_TOOLS_HUGINN_STRING_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_STRING_HXX_INCLUDED 1

#include "tools/huginn/iterable.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HString : public HIterable {
public:
	typedef HString this_type;
	typedef HIterable base_type;
	typedef yaal::hcore::HString value_type;
private:
	value_type _value;
public:
	HString( HClass const*, value_type const& );
	value_type const& value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;;
};

namespace string {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t reversed_view( huginn::HThread*, HHuginn::value_t const& );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_STRING_HXX_INCLUDED */

