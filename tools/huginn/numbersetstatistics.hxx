/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  numbersetstatistics.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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
	static HHuginn::class_t get_class( HRuntime* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_NUMBERSETSTATISTICS_HXX_INCLUDED */

