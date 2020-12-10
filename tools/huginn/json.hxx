/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_JSON_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_JSON_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

void json_serialize( huginn::HThread*, yaal::tools::HHuginn::value_t const&, yaal::hcore::HStreamInterface&, bool, int );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_JSON_HXX_INCLUDED */

