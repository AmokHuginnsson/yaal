/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/model.hxx
 * \brief Declaration of cxx_to_huginn() and huginn_to_cxx() functions.
 */

#ifndef YAAL_TOOLS_HUGINN_MODEL_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_MODEL_HXX_INCLUDED 1

#include "tools/model.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

yaal::tools::model::HValue huginn_to_cxx( HThread*, HHuginn::value_t const&, char const*, int );
HHuginn::value_t cxx_to_huginn( HThread*, yaal::tools::model::HValue const&, int );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_MODEL_HXX_INCLUDED */

