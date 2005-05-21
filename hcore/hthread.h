/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hthread.h - this file is integral part of `stdhapi' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __HCORE_HTHREAD_H
#define __HCORE_HTHREAD_H

#include <pthread.h>

namespace stdhapi
{

namespace hcore
{

class HThread
	{
protected:
	/*{*/
	bool						f_bAlive;
	pthread_attr_t	f_sAttributes;
	pthread_t				f_xThread;
	/*}*/
public:
	/*{*/
	HThread ( void );
	virtual ~HThread ( void );
	int spawn ( void );
	int finish ( void );
	void listen ( void ) const;
	/*}*/
private:
	virtual int run ( void ) = 0;
	static void * SPAWN ( void * );
	HThread ( const HThread & );
	HThread & operator = ( const HThread & );
	};

}

}

#endif /* not __HCORE_HTHREAD_H */
