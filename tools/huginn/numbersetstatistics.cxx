/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

template<typename coll_t>
HNumberSetStatistics::number_set_stats_t make_data( HThread* thread_, HHuginn::type_id_t const& vt_, HHuginn::value_t const& col_, int position_ ) {
	M_PROLOG
	typename coll_t::values_t const& src( static_cast<coll_t const*>( col_.raw() )->value() );
	HNumberSetStatistics::number_set_stats_t nss;
	if ( src.is_empty() ) {
		throw HHuginn::HHuginnRuntimeException( "Cannot aggregate statistics over empty set.", thread_->current_frame()->file_id(), position_ );
	}
	if ( vt_ == HHuginn::TYPE::REAL ) {
		nss = HNumberSetStatistics::number_set_stats_t(
			make_resource<HNumberSetStatistics::number_set_stats_real_t>(
				value_unboxing_iterator<double long>( src.begin() ),
				value_unboxing_iterator<double long>( src.end() ),
				AGGREGATE_TYPE::BASIC | AGGREGATE_TYPE::MEDIAN | AGGREGATE_TYPE::INTERQUARTILE_RANGE | AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION
			)
		);
	} else {
		M_ASSERT( vt_ == HHuginn::TYPE::NUMBER );
		nss = HNumberSetStatistics::number_set_stats_t(
			make_resource<HNumberSetStatistics::number_set_stats_number_t>(
				value_unboxing_iterator<yaal::hcore::HNumber>( src.begin() ),
				value_unboxing_iterator<yaal::hcore::HNumber>( src.end() ),
				AGGREGATE_TYPE::BASIC | AGGREGATE_TYPE::MEDIAN | AGGREGATE_TYPE::INTERQUARTILE_RANGE | AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION
			)
		);
	}
	return ( nss );
	M_EPILOG
}

HNumberSetStatistics::HNumberSetStatistics( huginn::HThread* thread_, HHuginn::HClass const* class_, HHuginn::values_t& values_, int position_ )
	: HValue( class_ )
	, _stats() {
	char const name[] = "NumberSetStatistics.constructor";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HHuginn::type_id_t t( verify_arg_type( name, values_, 0, { HHuginn::TYPE::TUPLE, HHuginn::TYPE::LIST, HHuginn::TYPE::DEQUE, HHuginn::TYPE::SET, HHuginn::TYPE::ORDER }, ARITY::UNARY, thread_, position_ ) );
	HHuginn::type_id_t vt( verify_arg_collection_value_type( name, values_, 0, ARITY::UNARY, { HHuginn::TYPE::REAL, HHuginn::TYPE::NUMBER }, UNIFORMITY::REQUIRED, thread_, position_ ) );
	if ( t == HHuginn::TYPE::TUPLE ) {
		_stats = make_data<HHuginn::HTuple>( thread_, vt, values_[0], position_ );
	} else if ( t == HHuginn::TYPE::LIST ) {
		_stats = make_data<HHuginn::HList>( thread_, vt, values_[0], position_ );
	} else if ( t == HHuginn::TYPE::DEQUE ) {
		_stats = make_data<HHuginn::HDeque>( thread_, vt, values_[0], position_ );
	} else if ( t == HHuginn::TYPE::SET ) {
		_stats = make_data<HHuginn::HSet>( thread_, vt, values_[0], position_ );
	} else if ( t == HHuginn::TYPE::ORDER ) {
		_stats = make_data<HHuginn::HOrder>( thread_, vt, values_[0], position_ );
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

HHuginn::value_t HNumberSetStatistics::stat( char const* name_, xmath::aggregate_type_t aggregateType_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HNumberSetStatistics::derivative_stat( char const* name_, DERIVATIVE_STAT derivativeStats_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, thread_, position_ );
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

HHuginn::value_t HNumberSetStatistics::count( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
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

HHuginn::value_t HNumberSetStatistics::create_instance( HHuginn::HClass const* class_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	return ( thread_->object_factory().create<HNumberSetStatistics>( thread_, class_, values_, position_ ) );
	M_EPILOG
}

HHuginn::class_t HNumberSetStatistics::get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"NumberSetStatistics",
			"The `NumberSetStatistics` is a class representing results of gathering numerical statistics over some uniformly typed number set.",
			HHuginn::ACCESS::PUBLIC,
			HHuginn::HClass::TYPE::BUILTIN,
			origin_,
			&HNumberSetStatistics::create_instance
		)
	);
	HHuginn::field_definitions_t fd{
		{ "count",                         runtime_->create_method( &HNumberSetStatistics::count ), "a number of elements in given numeric set" },
		{ "minimum",                       runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.minimum",                       AGGREGATE_TYPE::MINIMUM ),                       "a minimum numeric value in set" },
		{ "maximum",                       runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.maximum",                       AGGREGATE_TYPE::MAXIMUM ),                       "a maximum numeric value in set" },
		{ "sum",                           runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.sum",                           AGGREGATE_TYPE::SUM ),                           "a sum of all values in set" },
		{ "arithmetic_mean",               runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.arithmetic_mean",               AGGREGATE_TYPE::ARITHMETIC_MEAN ),               "an arithmetic mean of all numbers in set" },
		{ "median",                        runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.median",                        AGGREGATE_TYPE::MEDIAN ),                        "a median value in set" },
		{ "interquartile_range",           runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.interquartile_range",           AGGREGATE_TYPE::INTERQUARTILE_RANGE ),           "an interquartile range of given set of values" },
		{ "sample_variance",               runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.sample_variance",               AGGREGATE_TYPE::SAMPLE_VARIANCE ),               "a sample_variance of the set" },
		{ "population_variance",           runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.population_variance",           AGGREGATE_TYPE::POPULATION_VARIANCE ),           "a population sample_variance of the numeric set" },
		{ "sample_standard_deviation",     runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.sample_standard_deviation",     AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION ),     "a standard deviation of the numeric set" },
		{ "population_standard_deviation", runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.population_standard_deviation", AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION ), "a population standard deviation of the numeric set" },
		{ "mean_absolute_deviation",       runtime_->create_method( &HNumberSetStatistics::stat, "NumberSetStatistics.mean_absolute_deviation",       AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION ),       "a mean absolute deviation of the numeric set" },
		{ "range",                         runtime_->create_method( &HNumberSetStatistics::derivative_stat, "NumberSetStatistics.range",              DERIVATIVE_STAT::RANGE ),                        "a range of values in the numeric set" },
		{ "mid_range",                     runtime_->create_method( &HNumberSetStatistics::derivative_stat, "NumberSetStatistics.mid_range",          DERIVATIVE_STAT::MID_RANGE ),                    "a mid range value of the numbers in the given set" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HNumberSetStatistics::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on NumberSetStatistics.", thread_->current_frame()->file_id(), position_ );
}

}

}

}
