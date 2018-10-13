/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_NUMBERSETSTATISTICS_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_NUMBERSETSTATISTICS_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/xmath.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HNumberSetStatistics : public HHuginn::HValue {
public:
	enum class DERIVATIVE_STAT {
		RANGE,
		MID_RANGE
	};
	typedef yaal::tools::xmath::HNumberSetStats<double long> number_set_stats_real_t;
	typedef yaal::hcore::HResource<number_set_stats_real_t> number_set_stats_real_ptr_t;
	typedef yaal::tools::xmath::HNumberSetStats<yaal::hcore::HNumber> number_set_stats_number_t;
	typedef yaal::hcore::HResource<number_set_stats_number_t> number_set_stats_number_ptr_t;
	typedef yaal::hcore::HVariant<number_set_stats_real_ptr_t, number_set_stats_number_ptr_t> number_set_stats_t;
private:
	number_set_stats_t _stats;
public:
	HNumberSetStatistics( huginn::HThread*, HHuginn::HClass const*, HHuginn::values_t&, int );
	static HHuginn::value_t stat( char const*, xmath::aggregate_type_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t derivative_stat( char const*, DERIVATIVE_STAT, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t count( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t histogram( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HHuginn::HClass const* );
	static HHuginn::value_t create_instance( HHuginn::HClass const*, huginn::HThread*, HHuginn::values_t&, int );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_NUMBERSETSTATISTICS_HXX_INCLUDED */

