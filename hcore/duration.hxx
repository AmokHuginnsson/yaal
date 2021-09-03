/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/duration.hxx
 * \brief Declaration of time duration related type and functions.
 */

#ifndef YAAL_HCORE_DURATION_HXX_INCLUDED
#define YAAL_HCORE_DURATION_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/htaggedpod.hxx"
#include "hcore/hstring.hxx"

namespace yaal {

namespace hcore {

namespace time {

/*! \brief Resolution units for time measurement.
 */
enum class UNIT {
	NANOSECOND,
	MICROSECOND,
	MILLISECOND,
	SECOND,
	MINUTE,
	HOUR,
	DAY,
	WEEK
};

enum class UNIT_FORM {
	FULL,
	ABBREVIATED
};

extern M_YAAL_HCORE_PUBLIC_API UNIT_FORM _durationUnitForm_;

typedef HTaggedPOD<yaal::i64_t, struct Duration> duration_t;

duration_t duration( yaal::i64_t, UNIT );
template <UNIT const u>
yaal::i64_t in_units( duration_t );
yaal::hcore::HString duration_to_string( duration_t, UNIT, UNIT_FORM = _durationUnitForm_ );
UNIT scale( duration_t, UNIT = UNIT::WEEK );

}

}

}

#endif /* #ifndef YAAL_HCORE_DURATION_HXX_INCLUDED */

