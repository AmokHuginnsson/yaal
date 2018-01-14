/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_OMENUITEM_HXX_INCLUDED
#define YAAL_HCONSOLE_OMENUITEM_HXX_INCLUDED 1

#include "hcore/hboundcall.hxx"
#include "hcore/hstring.hxx"
#include "hcore/htime.hxx"

namespace yaal {

namespace hconsole {

class HTUIProcess;

/*! \brief TUI menu node.
 */
struct OMenuItem {
public: /* All is public for simpler usage. */
	typedef yaal::hcore::HBoundCall<> call_t;
	call_t _call;
	yaal::hcore::HString _label;
/* Methods */
	OMenuItem( void );
	OMenuItem( OMenuItem const& );
	void call( void ) const;
	yaal::hcore::HString const& get_string( void ) const;
	int long get_integer( void ) const M_DEBUG_CODE( __attribute__((__noreturn__)) );
	double get_real( void ) const M_DEBUG_CODE( __attribute__((__noreturn__)) );
	yaal::hcore::HTime const& get_time( void ) const M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_OMENUITEM_HXX_INCLUDED */

