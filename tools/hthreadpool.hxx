/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/hthreadpool.hxx
 * \brief Declaration of HThreadPool class.
 */

#ifndef YAAL_TOOLS_HTHREADPOOL_HXX_INCLUDED
#define YAAL_TOOLS_HTHREADPOOL_HXX_INCLUDED 1

#include "tools/hworkflow.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

class M_YAAL_TOOLS_PUBLIC_API HThreadPool : public yaal::tools::HWorkFlow, public yaal::hcore::HSingleton<HThreadPool> {
public:
	typedef HThreadPool this_type;
	typedef HWorkFlow base_type;
	static int life_time( int ) {
		return ( 100 );
	}
	void stop( void );
private:
	HThreadPool( void );
	virtual ~HThreadPool( void );
	HThreadPool( HThreadPool const& ) = delete;
	HThreadPool& operator = ( HThreadPool const& ) = delete;
	friend class yaal::hcore::HSingleton<HThreadPool>;
	friend class yaal::hcore::HDestructor<HThreadPool>;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HTHREADPOOL_HXX_INCLUDED */

