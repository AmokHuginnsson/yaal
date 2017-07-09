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

HNumberSetStatistics::HNumberSetStatistics( huginn::HThread* thread_, HHuginn::HClass const* class_, HHuginn::values_t const& values_, int position_ )
	: HValue( class_ )
	, _stats() {
	char const name[] = "NumberSetStatistics.constructor";
	verify_signature( name, values_, { HHuginn::TYPE::LIST }, thread_, position_ );
	HHuginn::type_id_t t( values_[0]->type_id() );
	HHuginn::type_id_t vt( verify_arg_collection_value_type( name, values_, 0, ARITY::UNARY, { HHuginn::TYPE::REAL, HHuginn::TYPE::NUMBER }, UNIFORMITY::REQUIRED, thread_, position_ ) );
	if ( t == HHuginn::TYPE::LIST ) {
		HHuginn::HList::values_t const& src( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
		if ( src.is_empty() ) {
			throw HHuginn::HHuginnRuntimeException( "Cannot aggregate statistics over empty set.", thread_->current_frame()->file_id(), position_ );
		}
		if ( vt == HHuginn::TYPE::REAL ) {
			_stats = number_set_stats_t( make_resource<number_set_stats_real_t>( value_unboxing_iterator<double long>( src.begin() ), value_unboxing_iterator<double long>( src.end() ), AGGREGATE_TYPE::BASIC | AGGREGATE_TYPE::MEDIAN | AGGREGATE_TYPE::INTERQUARTILE_RANGE | AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION ) );
		} else {
			M_ASSERT( vt == HHuginn::TYPE::NUMBER );
			_stats = number_set_stats_t( make_resource<number_set_stats_number_t>( value_unboxing_iterator<yaal::hcore::HNumber>( src.begin() ), value_unboxing_iterator<yaal::hcore::HNumber>( src.end() ), AGGREGATE_TYPE::BASIC | AGGREGATE_TYPE::MEDIAN | AGGREGATE_TYPE::INTERQUARTILE_RANGE | AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION ) );
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
	} else if ( aggregateType_ == AGGREGATE_TYPE::ARITHMETIC_MEAN ) {
		res = stats_.arithmetic_mean();
	} else if ( aggregateType_ == AGGREGATE_TYPE::MEDIAN ) {
		res = stats_.median();
	} else if ( aggregateType_ == AGGREGATE_TYPE::INTERQUARTILE_RANGE ) {
		res = stats_.interquartile_range();
	} else if ( aggregateType_ == AGGREGATE_TYPE::SAMPLE_VARIANCE ) {
		res = stats_.sample_variance();
	} else if ( aggregateType_ == AGGREGATE_TYPE::POPULATION_VARIANCE ) {
		res = stats_.population_variance();
	} else if ( aggregateType_ == AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION ) {
		res = stats_.sample_standard_deviation();
	} else if ( aggregateType_ == AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION ) {
		res = stats_.population_standard_deviation();
	} else if ( aggregateType_ == AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION ) {
		res = stats_.mean_absolute_deviation();
	}
	return ( res );
}

template<typename stats_t>
typename stats_t::value_type derivative_stats_impl( stats_t const& stats_, HNumberSetStatistics::DERIVATIVE_STAT derivativeStats_ ) {
	typename stats_t::value_type res( 0 );
	if ( derivativeStats_ == HNumberSetStatistics::DERIVATIVE_STAT::RANGE ) {
		res = stats_.range();
	} else if ( derivativeStats_ == HNumberSetStatistics::DERIVATIVE_STAT::MID_RANGE ) {
		res = stats_.mid_range();
	}
	return ( res );
}

HHuginn::value_t HNumberSetStatistics::stat( char const* name_, xmath::aggregate_type_t aggregateType_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "NumberSetStatistics."_ys.append( name_ ), values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HNumberSetStatistics::derivative_stat( char const* name_, DERIVATIVE_STAT derivativeStats_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "NumberSetStatistics."_ys.append( name_ ), values_, 0, 0, thread_, position_ );
	HHuginn::value_t v;
	HNumberSetStatistics* o( static_cast<HNumberSetStatistics*>( object_->raw() ) );
	if ( o->_stats.type() == 0 ) {
		number_set_stats_real_t& nss( *( o->_stats.get<number_set_stats_real_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_real( derivative_stats_impl( nss, derivativeStats_ ) );
	} else {
		number_set_stats_number_t& nss( *( o->_stats.get<number_set_stats_number_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_number( derivative_stats_impl( nss, derivativeStats_ ) );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HNumberSetStatistics::count( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "NumberSetStatistics.count", values_, 0, 0, thread_, position_ );
	HHuginn::value_t v;
	HNumberSetStatistics* o( static_cast<HNumberSetStatistics*>( object_->raw() ) );
	if ( o->_stats.type() == 0 ) {
		number_set_stats_real_t& nss( *( o->_stats.get<number_set_stats_real_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_integer( nss.count() );
	} else {
		number_set_stats_number_t& nss( *( o->_stats.get<number_set_stats_number_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_integer( nss.count() );
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
				{ "count",                         runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::count, _1, _2, _3, _4 ) ), "a number of elements in given numeric set" },
				{ "minimum",                       runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "minimum",                       AGGREGATE_TYPE::MINIMUM, _1, _2, _3, _4 ) ),                       "a minimum numeric value in set" },
				{ "maximum",                       runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "maximum",                       AGGREGATE_TYPE::MAXIMUM, _1, _2, _3, _4 ) ),                       "a maximum numeric value in set" },
				{ "sum",                           runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "sum",                           AGGREGATE_TYPE::SUM, _1, _2, _3, _4 ) ),                           "a sum of all values in set" },
				{ "arithmetic_mean",               runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "arithmetic_mean",               AGGREGATE_TYPE::ARITHMETIC_MEAN, _1, _2, _3, _4 ) ),               "an arithmetic mean of all numbers in set" },
				{ "median",                        runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "median",                        AGGREGATE_TYPE::MEDIAN, _1, _2, _3, _4 ) ),                        "a median value in set" },
				{ "interquartile_range",           runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "interquartile_range",           AGGREGATE_TYPE::INTERQUARTILE_RANGE, _1, _2, _3, _4 ) ),           "an interquartile range of given set of values" },
				{ "sample_variance",               runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "sample_variance",               AGGREGATE_TYPE::SAMPLE_VARIANCE, _1, _2, _3, _4 ) ),               "a sample_variance of the set" },
				{ "population_variance",           runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "population_variance",           AGGREGATE_TYPE::POPULATION_VARIANCE, _1, _2, _3, _4 ) ),           "a population sample_variance of the numeric set" },
				{ "sample_standard_deviation",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "sample_standard_deviation",     AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION, _1, _2, _3, _4 ) ),     "a standard deviation of the numeric set" },
				{ "population_standard_deviation", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "population_standard_deviation", AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION, _1, _2, _3, _4 ) ), "a population standard deviation of the numeric set" },
				{ "mean_absolute_deviation",       runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::stat, "mean_absolute_deviation",       AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION, _1, _2, _3, _4 ) ),       "a mean absolute deviation of the numeric set" },
				{ "range",     runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::derivative_stat, "range",     DERIVATIVE_STAT::RANGE,     _1, _2, _3, _4 ) ), "a range of values in the numeric set" },
				{ "mid_range", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HNumberSetStatistics::derivative_stat, "mid_range", DERIVATIVE_STAT::MID_RANGE, _1, _2, _3, _4 ) ), "a mid range value of the numbers in the given set" }
			},
			"The `NumberSetStatistics` is a class representing results of gathering numerical statistics over some uniformly typed number set."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HNumberSetStatistics::do_clone( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on NumberSetStatistics.", thread_->current_frame()->file_id(), position_ );
}

}

}

}
