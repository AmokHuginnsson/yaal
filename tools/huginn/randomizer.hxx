/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_RANDOMIZER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RANDOMIZER_HXX_INCLUDED 1

#include "hcore/hrandomizer.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRandomizer : public HHuginn::HValue {
private:
	yaal::hcore::HRandomizer _generator;
public:
	HRandomizer( HHuginn::HClass const*, yaal::u64_t );
	HRandomizer( HHuginn::HClass const*, yaal::hcore::HRandomizer const& );
	static HHuginn::value_t seed( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t next( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t next_real( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HHuginn::HClass const* );
	static HHuginn::value_t create_instance( HHuginn::HClass const*, huginn::HThread*, HHuginn::values_t&, int );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RANDOMIZER_HXX_INCLUDED */

