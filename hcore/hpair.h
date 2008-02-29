/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpair.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HPAIR_H
#define __YAAL_HCORE_HPAIR_H

#line 31

#define D_VCSID_YAAL_HCORE_HPAIR_H "$Id$"

namespace yaal
{

namespace hcore
{

template<typename tType, typename ttType>
class HPair
	{
	tType f_tKey;
	ttType f_tValue;
public:
	typename trait::strip_reference<tType>::type& first;
	typename trait::strip_reference<ttType>::type& second;
	HPair( void ) : f_tKey(), f_tValue(), first( f_tKey ), second( f_tValue ) {}
	HPair( tType key, ttType value ) : f_tKey( key ), f_tValue( value ), first( f_tKey ), second( f_tValue ) {}
	HPair( HPair const& pair ) : f_tKey( pair.f_tKey ), f_tValue( pair.f_tValue ), first( f_tKey ), second( f_tValue ) {}
	HPair& operator = ( HPair const& pair )
		{
		if ( &pair != this )
			{
			f_tKey = pair.f_tKey;
			f_tValue = pair.f_tValue;
			}
		return ( *this );
		}
	bool operator == ( HPair const& pair ) const
		{	return ( ( f_tKey == pair.f_tKey ) && ( f_tValue == pair.f_tValue ) );	}
	bool operator != ( HPair const& pair ) const
		{	return ( ! operator == ( pair ) );	}
	bool operator < ( HPair const& pair ) const
		{
		return ( ( f_tKey < pair.f_tKey )
				|| ( ! ( pair.f_tKey < f_tKey ) && ( f_tValue < pair.f_tValue ) ) );
		}
	};

}

}

#endif /* not __YAAL_HCORE_HPAIR_H */

