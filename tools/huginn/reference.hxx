/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_REFERENCE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_REFERENCE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSubscriptReference : public HHuginn::HReference {
public:
	typedef HSubscriptReference this_type;
	typedef HHuginn::HReference base_type;
private:
	HHuginn::value_t _base;
	HHuginn::value_t _key;
public:
	HSubscriptReference( HHuginn::HClass const*, HHuginn::value_t const&, HHuginn::value_t const& );
private:
	virtual HHuginn::value_t do_get( huginn::HThread*, int ) const override;
	virtual void do_set( huginn::HThread*, HHuginn::value_t&&, int ) override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_REFERENCE_HXX_INCLUDED */

