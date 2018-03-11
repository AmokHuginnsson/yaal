/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_ENUMERATION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ENUMERATION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "runtime.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace enumeration {

class HDesctiption {
private:
	yaal::hcore::HString _name;
	yaal::hcore::HString _doc;
public:
	HDesctiption( yaal::hcore::HString const& name_, yaal::hcore::HString const& doc_ )
		: _name( name_ )
		, _doc( doc_ ) {
	}
	yaal::hcore::HString const& name( void ) const {
		return ( _name );
	}
	yaal::hcore::HString const& doc( void ) const {
		return ( _doc );
	}
};

typedef yaal::hcore::HArray<HDesctiption> descriptions_t;

HHuginn::class_t create_class(
	HRuntime*,
	yaal::hcore::HString const&,
	descriptions_t const&,
	yaal::hcore::HString const&,
	HHuginn::VISIBILITY,
	HHuginn::HClass const* = nullptr
);

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ENUMERATION_HXX_INCLUDED */

