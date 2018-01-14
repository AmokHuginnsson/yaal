/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace list {

HHuginn::value_t sort( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t reversed_view( huginn::HThread*, HHuginn::value_t const& );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_LIST_HXX_INCLUDED */

