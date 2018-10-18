/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_INSTRUCTION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_INSTRUCTION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/frame.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/operator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace instruction {

HHuginn::value_t subscript( HThread*, HFrame::ACCESS, HHuginn::value_t&, HHuginn::value_t const&, int );
HHuginn::value_t range( HThread*, HHuginn::value_t&, HHuginn::value_t const&, HHuginn::value_t const&, HHuginn::value_t const&, int );
void add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void sub( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void mul( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void div( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void mod( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
void pow( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
HHuginn::value_t abs( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t neg( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t factorial( HThread*, HHuginn::value_t const&, int );
bool equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool checked_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
bool is_element_of( HThread*, OPERATOR, HHuginn::value_t const&, HHuginn::value_t const&, int );
int long hash( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t boolean_xor( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
HHuginn::value_t boolean_not( HThread*, HHuginn::value_t const&, int );
yaal::hcore::HString string_representation( HThread*, HHuginn::value_t const&, HCycleTracker&, int );

HHuginn::value_t string( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t integer( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t real( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t boolean( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t character( HThread*, HHuginn::value_t const&, int );
HHuginn::value_t number( HThread*, HHuginn::value_t const&, int );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_INSTRUCTION_HXX_INCLUDED */

