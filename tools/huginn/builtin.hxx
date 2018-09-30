/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_BUILTIN_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_BUILTIN_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/frame.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/operator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace builtin {

#undef assert
HHuginn::value_t assert( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t string( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t integer( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t real( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t boolean( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t character( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t number( HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t tuple( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t list( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t deque( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t dict( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t order( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t lookup( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t set( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t blob( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t size( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t type( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t copy( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t observe( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t use( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t square_root( char const*, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t n_ary_summation( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t print( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t input( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_BUILTIN_HXX_INCLUDED */

