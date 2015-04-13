/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hthreadpool.hxx - this file is integral part of `yaal' project.

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

/*! \file tools/hthreadpool.hxx
 * \brief Declaration of HThreadPool class.
 */

#ifndef YAAL_TOOLS_HTHREADPOOL_HXX_INCLUDED
#define YAAL_TOOLS_HTHREADPOOL_HXX_INCLUDED 1

#include "tools/hworkflow.hxx"
#include "hcore/hsingleton.hxx"

namespace yaal {

namespace tools {

class HThreadPool : public yaal::tools::HWorkFlow, public yaal::hcore::HSingleton<HThreadPool> {
public:
	typedef HThreadPool this_type;
	typedef HWorkFlow base_type;
	static int life_time( int ) {
		return ( 100 );
	}
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

