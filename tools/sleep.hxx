/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/sleep.hxx
 * \brief Declaration of sleep functions.
 */

#include "hcore/duration.hxx"

#ifndef YAAL_TOOLS_SLEEP_HXX_INCLUDED
#define YAAL_TOOLS_SLEEP_HXX_INCLUDED 1

namespace yaal {

namespace tools {

/*! \brief Suspend execution of current thread for given duration.
 *
 * \param duration - try sleep for that long period.
 * \param ignoreInterrupts - continue sleeping even if interrupted.
 * \return True iff sleep_for slept for full duration (was not interrupted).
 */
bool sleep_for( yaal::hcore::time::duration_t duration, bool ignoreInterrupts = false );

}

}

#endif /* #ifndef YAAL_TOOLS_SLEEP_HXX_INCLUDED */

