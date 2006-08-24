/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HINFO_H
#define __YAAL_HCORE_HINFO_H

#include "hstring.h"
#include "htime.h"
#include "hexception.h" /* types definitions */

namespace yaal
{

namespace hcore
{

class HInfo
	{
protected:
	/*{*/
	type_t		f_eType;
	int				f_iInt;
	int long	f_lLongInt;
	double		f_dDouble;
	void *		f_pvPointer;
	HString		f_oString;
	HTime			f_oTime;
	/*}*/
public:
	/*{*/
	HInfo ( void );
	HInfo ( int const & );
	HInfo ( int long const & );
	HInfo ( double const & );
	HInfo ( char const * const );
	HInfo ( HString const & );
	HInfo ( HTime const & );
	HInfo ( void * const );
	HInfo ( HInfo const & );
	virtual ~HInfo ( void );
	char & operator [ ] ( int const );
	HInfo & operator = ( HInfo const & );
	HInfo & operator ( ) ( HInfo const & );
	template < typename tType >
	tType const get ( void ) const;
	operator bool const ( void ) const;
	/*}*/
protected:
	/*{*/
	void purge ( void );
	/*}*/
	};

}

}

#endif /* not __YAAL_HCORE_HINFO_H */
