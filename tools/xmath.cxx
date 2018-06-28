/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "xmath.hxx"

namespace yaal {

namespace tools {

namespace xmath {

namespace {
aggregate_type_t const NONE = aggregate_type_t::new_flag();
}

aggregate_type_t const AGGREGATE_TYPE::COUNT = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::MINIMUM = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::MAXIMUM = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::SUM = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::ARITHMETIC_MEAN = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::SAMPLE_VARIANCE = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::POPULATION_VARIANCE = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::BASIC = AGGREGATE_TYPE::COUNT
	| AGGREGATE_TYPE::MINIMUM
	| AGGREGATE_TYPE::MAXIMUM
	| AGGREGATE_TYPE::SUM
	| AGGREGATE_TYPE::ARITHMETIC_MEAN
	| AGGREGATE_TYPE::SAMPLE_VARIANCE
	| AGGREGATE_TYPE::POPULATION_VARIANCE
	| AGGREGATE_TYPE::SAMPLE_STANDARD_DEVIATION
	| AGGREGATE_TYPE::POPULATION_STANDARD_DEVIATION;
aggregate_type_t const AGGREGATE_TYPE::MEDIAN = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::MEAN_ABSOLUTE_DEVIATION = aggregate_type_t::new_flag();
aggregate_type_t const AGGREGATE_TYPE::INTERQUARTILE_RANGE = aggregate_type_t::new_flag();

int long long AGGREGATE_TYPE::required_data_points( aggregate_type_t const& aggregateType_ ) {
	int long long rdp( 0 );
	if ( aggregateType_ & SAMPLE_STANDARD_DEVIATION ) {
		rdp = max( rdp, 1LL );
	}
	if ( aggregateType_ & SAMPLE_VARIANCE ) {
		rdp = max( rdp, 1LL );
	}
	return ( rdp );
}

}

}

}

