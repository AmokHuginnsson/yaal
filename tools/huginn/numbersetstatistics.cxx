/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  numbersetstatistics.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "numbersetstatistics.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::xmath;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HNumberSetStatistics::HNumberSetStatistics( HHuginn::HClass const* class_, HHuginn::values_t const& values_, int position_ )
	: HValue( class_ )
	, _stats() {
	char const name[] = "NumberSetStatistics.constructor";
	verify_arg_count( name, values_, 1, 1, position_ );
	HHuginn::type_id_t t( values_[0]->type_id() );
	HHuginn::type_id_t vt( verify_arg_collection_value_type( name, values_, 0, true, { HHuginn::TYPE::REAL, HHuginn::TYPE::NUMBER }, true, position_ ) );
	if ( t == HHuginn::TYPE::LIST ) {
		HHuginn::HList::values_t const& src( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
		if ( vt == HHuginn::TYPE::REAL ) {
			_stats = number_set_stats_t( make_resource<number_set_stats_real_t>( value_unboxing_iterator<double long>( src.begin() ), value_unboxing_iterator<double long>( src.end() ), AGGREGATE_TYPE::BASIC | AGGREGATE_TYPE::MEDIAN ) );
		} else {
			M_ASSERT( vt == HHuginn::TYPE::NUMBER );
			_stats = number_set_stats_t( make_resource<number_set_stats_number_t>( value_unboxing_iterator<yaal::hcore::HNumber>( src.begin() ), value_unboxing_iterator<yaal::hcore::HNumber>( src.end() ), AGGREGATE_TYPE::BASIC | AGGREGATE_TYPE::MEDIAN ) );
		}
	}
	return;
}

template<typename stats_t>
typename stats_t::value_type stats_impl( stats_t const& stats_, aggregate_type_t aggregateType_ ) {
	typename stats_t::value_type res( 0 );
	if ( aggregateType_ == AGGREGATE_TYPE::MINIMUM ) {
		res = stats_.minimum();
	} else if ( aggregateType_ == AGGREGATE_TYPE::MAXIMUM ) {
		res = stats_.maximum();
	} else if ( aggregateType_ == AGGREGATE_TYPE::SUM ) {
		res = stats_.sum();
	} else if ( aggregateType_ == AGGREGATE_TYPE::AVERAGE ) {
		res = stats_.average();
	} else if ( aggregateType_ == AGGREGATE_TYPE::MEDIAN ) {
		res = stats_.median();
	} else if ( aggregateType_ == AGGREGATE_TYPE::VARIANCE ) {
		res = stats_.variance();
	} else if ( aggregateType_ == AGGREGATE_TYPE::POPULATION_VARIANCE ) {
		res = stats_.population_variance();
	} else if ( aggregateType_ == AGGREGATE_TYPE::STANDARD_DEVIATION ) {
		res = stats_.standard_deviation();
	} else if ( aggregateType_ == AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION ) {
		res = stats_.population_standard_deviation();
	}
	return ( res );
}

HHuginn::value_t HNumberSetStatistics::stat( char const* name_, xmath::aggregate_type_t aggregateType_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "NumberSetStatistics."_ys.append( name_ ), values_, 0, 0, position_ );
	HHuginn::value_t v;
	HNumberSetStatistics* o( static_cast<HNumberSetStatistics*>( object_->raw() ) );
	if ( o->_stats.type() == 0 ) {
		number_set_stats_real_t& nss( *( o->_stats.get<number_set_stats_real_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_real( stats_impl( nss, aggregateType_ ) );
	} else {
		number_set_stats_number_t& nss( *( o->_stats.get<number_set_stats_number_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_number( stats_impl( nss, aggregateType_ ) );
	}
	return ( v );
	M_EPILOG
}

HHuginn::class_t HNumberSetStatistics::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"NumberSetStatistics",
			nullptr,
			HHuginn::field_definitions_t{
				{ "minimum",                       make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "minimum",                       AGGREGATE_TYPE::MINIMUM, _1, _2, _3, _4 ) ), "minimum numeric value in set" },
				{ "maximum",                       make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "maximum",                       AGGREGATE_TYPE::MAXIMUM, _1, _2, _3, _4 ) ), "maximum numeric value in set" },
				{ "sum",                           make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "sum",                           AGGREGATE_TYPE::SUM, _1, _2, _3, _4 ) ), "sum of all values in set" },
				{ "average",                       make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "average",                       AGGREGATE_TYPE::AVERAGE, _1, _2, _3, _4 ) ), "average value of all numbers in set" },
				{ "median",                        make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "median",                        AGGREGATE_TYPE::MEDIAN, _1, _2, _3, _4 ) ), "median value in set" },
				{ "variance",                      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "variance",                      AGGREGATE_TYPE::VARIANCE, _1, _2, _3, _4 ) ), "variance of the set" },
				{ "population_variance",           make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "population_variance",           AGGREGATE_TYPE::POPULATION_VARIANCE, _1, _2, _3, _4 ) ), "population variance of the numeric set" },
				{ "standard_deviation",            make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "standard_deviation",            AGGREGATE_TYPE::STANDARD_DEVIATION, _1, _2, _3, _4 ) ), "standard deviation of the numeric set" },
				{ "population_standard_deviation", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "population_standard_deviation", AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION, _1, _2, _3, _4 ) ), "population standard deviation of the numeric set" }
			},
			"The `NumberSetStatistics` is a class representing results of gathering numerical statistics over some uniformly typed number set."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HNumberSetStatistics::do_clone( huginn::HRuntime* ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on NumberSetStatistics.", 0 );
}

}

}

}
