/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/introspect.hxx
 * \brief C++ process introspection routines.
 */

#ifndef YAAL_HCORE_INTROSPECT_HXX_INCLUDED
#define YAAL_HCORE_INTROSPECT_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace hcore {

/*! \brief Low-level run-time information regarding program execution.
 */
struct execution_info {
	typedef HList<HString> strings_t;
	typedef HPointer<strings_t> strings_ptr_t;
	/*! \brief Get current call stack information.
	 *
	 * \param frames - number of frames to get.
	 * \return Current call stack as list of strings.
	 */
	static strings_ptr_t get_call_stack( int frames );
};

/*! \brief Print current call stack on the console.
 *
 * \param frames - numbers of frames to print.
 */
void dump_call_stack( HStreamInterface&, int frames );

}

}

#endif /* #ifndef YAAL_HCORE_INTROSPECT_HXX_INCLUDED */

