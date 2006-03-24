/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpointer.h - this file is integral part of `stdhapi' project.

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

#ifndef __HPOINTER_H
#define __HPOINTER_H

#line 31 "hpointer.h"

#define D_CVSID_HPOINTER_H "$CVSHeader$"

#include "./hcore/hexception.h"

namespace stdhapi
{

namespace hcore
{

/* Due to extream vitality of this class,
 * none of the methods are guarded. */

template < typename tType, bool array = false >
class HPointer
	{
	class HShared
		{
		tType * f_ptPointer;
		int f_iReferenceCounter;
		bool f_bArray;
		explicit HShared ( tType * const );
		virtual ~HShared ( void ) throw ( );
		bool release ( void ) throw ( );
		HShared ( HShared const & );
		HShared & operator = ( HShared const & );
		friend class HPointer;
		};
protected:
	/*{*/
	HShared * f_poShared;
	/*}*/
public:
	/*{*/
	explicit HPointer ( tType * const );
	virtual ~HPointer ( void );
	HPointer ( HPointer const & );
	HPointer & operator = ( HPointer const & );
	tType & operator * ( void ) const;
	tType * operator-> ( void ) const;
	tType & operator [ ] ( int ) const;
	/*}*/
	};

template < typename tType, bool array >
HPointer < tType, array >::HShared::HShared ( tType * const a_ptPointer )
	: f_ptPointer ( a_ptPointer ), f_iReferenceCounter ( 1 ), f_bArray ( array )
	{
	M_ASSERT ( f_ptPointer );
	return;
	}

template < typename tType, bool array >
HPointer < tType, array >::HShared::~HShared ( void ) throw ( )
	{
	M_ASSERT ( f_iReferenceCounter == 0 );
	return;
	}

template < typename tType, bool array >
bool HPointer < tType, array >::HShared::release ( void ) throw ( )
	{
	M_ASSERT ( f_iReferenceCounter > 0 );
	f_iReferenceCounter --;
	if ( ! f_iReferenceCounter )
		{
		if ( array )
			delete [ ] f_ptPointer;
		else
			delete f_ptPointer;
		f_ptPointer = NULL;
		}
	return ( ! f_iReferenceCounter );
	}

template < typename tType, bool array >
HPointer < tType, array >::HPointer ( tType * const a_ptPointer )
	: f_poShared ( new HShared ( a_ptPointer ) )
	{
	return;
	}

template < typename tType, bool array >
HPointer < tType, array >::~HPointer ( void )
	{
	if ( f_poShared->release ( ) )
		delete f_poShared;
	return;
	}

template < typename tType, bool array >
HPointer < tType, array >::HPointer ( HPointer const & a_roPointer )
	: f_poShared ( NULL )
	{
	operator = ( a_roPointer );
	return;
	}

template < typename tType, bool array >
HPointer < tType, array > & HPointer < tType, array >::operator = ( HPointer const & a_roPointer )
	{
	if ( ( & a_roPointer != this ) && ( a_roPointer.f_poShared != f_poShared ) )
		{
		a_roPointer.f_poShared->f_iReferenceCounter ++;
		if ( f_poShared )
			{
			M_ASSERT ( f_poShared->f_ptPointer != a_roPointer.f_poShared->f_ptPointer );
			if ( f_poShared->release ( ) )
				delete f_poShared;
			}
		f_poShared = a_roPointer.f_poShared;
		}
	return ( * this );
	}

template < typename tType, bool array >
tType & HPointer < tType, array >::operator * ( void ) const
	{
	return ( * f_poShared->f_ptPointer );
	}

template < typename tType, bool array >
tType * HPointer < tType, array >::operator-> ( void ) const
	{
	return ( f_poShared->f_ptPointer );
	}

template < typename tType, bool array >
tType & HPointer < tType, array >::operator [ ] ( int a_iIndex ) const
	{
	M_ASSERT ( f_poShared->f_bArray );
	M_ASSERT ( a_iIndex >= 0 );
	return ( f_poShared->f_ptPointer [ a_iIndex ] );
	}

}

}

#endif /* not __HPOINTER_H */

