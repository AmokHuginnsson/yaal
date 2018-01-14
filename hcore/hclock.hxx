/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hclock.hxx
 * \brief Declaration of HClock class.
 */

#ifndef YAAL_HCORE_HCLOCK_HXX_INCLUDED
#define YAAL_HCORE_HCLOCK_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/duration.hxx"

namespace yaal {

namespace hcore {

/*! \brief High resolution time measurement tool.
 */
class HClock final {
public:
	typedef HClock this_type;
	/*! \brief Specify what kind of time to measure.
	 */
	enum class TYPE {
		REAL,
		CPU
	};
private:
	i64_t _moment[2];
	TYPE _type;
public:
	/*! \brief Create clock of given type.
	 *
	 * \param type_ - type of clock to create.
	 */
	HClock( TYPE type_ = TYPE::REAL );
	/*! \brief Get time that elapsed from last reset.
	 *
	 * \param unit_ - get time given in specific units.
	 * \return Amount of time that elapsed in given unit.
	 */
	i64_t get_time_elapsed( yaal::hcore::time::UNIT unit_ = yaal::hcore::time::UNIT::SECOND ) const;

	/*! \brief Reset clock.
	 */
	void reset( void );
};

}

}

#endif /* #ifndef YAAL_HCORE_HCLOCK_HXX_INCLUDED */

