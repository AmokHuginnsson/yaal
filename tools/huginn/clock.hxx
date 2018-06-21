/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_CLOCK_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_CLOCK_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hclock.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HClock : public HHuginn::HValue {
	yaal::hcore::HClock _clock;
public:
	HClock( HHuginn::HClass* );
	static HHuginn::value_t milliseconds( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t reset( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HHuginn::HClass const* );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_CLOCK_HXX_INCLUDED */

