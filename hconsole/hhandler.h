/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.h - this file is integral part of `stdhapi' project.

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

#ifndef __HHANDLER_H
#define __HHANDLER_H

#line 31 "hhandler.h"

template < class tType >
class HHandler
	{
protected:
	/*{*/
	int		f_iCode;		/* event code for that causes handler to run */
	tType	f_tHandler;	/* caller itself */
	/*}*/
public:
	/*{*/
	HHandler ( void );
	HHandler ( int, tType );
	HHandler ( const HHandler &, int = 0 );
	virtual ~HHandler ( void );
	HHandler & operator = ( const HHandler & );
	operator const int ( void );
	operator const tType ( void );
	operator bool ( void );
	/*}*/
	};

#include "./hcore/hexception.h"

template < class tType >
HHandler < tType >::HHandler ( void )
	{
	M_PROLOG
	f_iCode = 0;
	f_tHandler = NULL;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::HHandler ( const HHandler & a_roHandler, int )
	{
	M_PROLOG
	( * this ) = a_roHandler;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::HHandler ( int a_iCode, tType a_tHandler )
	{
	M_PROLOG
	f_iCode = a_iCode;
	f_tHandler= a_tHandler;
	return;
	M_EPILOG
	}

template < class tType >
HHandler < tType >::~HHandler ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

template < class tType >
HHandler<tType> & HHandler <tType>::operator = ( const HHandler & a_roHandler )
	{
	M_PROLOG
	f_iCode = a_roHandler.f_iCode;
	f_tHandler = a_roHandler.f_tHandler;
	return ( * this );
	M_EPILOG
	}

template < class tType >
HHandler < tType >::operator const int ( void )
	{
	M_PROLOG
	return ( f_iCode );
	M_EPILOG
	}

template < class tType >
HHandler < tType >::operator const tType ( void )
	{
	M_PROLOG
	return ( f_tHandler );
	M_EPILOG
	}

template < class tType >
HHandler < tType >::operator bool ( void )
	{
	M_PROLOG
	return ( f_iCode && f_tHandler );
	M_EPILOG
	}

#endif /* __HHANDLER_H */
