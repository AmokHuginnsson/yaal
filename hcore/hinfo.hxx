/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HINFO_HXX_INCLUDED
#define YAAL_HCORE_HINFO_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/htime.hxx"
#include "hcore/hexception.hxx" /* types definitions */
#include "hcore/hstrongenum.hxx"
#include "hcore/reflection.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Type agnostic data container.
 */
class HInfo
	{
	type_t		f_eType;
	int				f_iInt;
	int long	f_lIntLong;
	double		f_dDouble;
	void*		f_pvPointer;
	HString		f_oString;
	HTime			f_oTime;
public:
	HInfo( void );
	HInfo( int const& );
	HInfo( int long const& );
	HInfo( double const& );
	HInfo( char const* const );
	HInfo( HString const& );
	HInfo( HTime const& );
	HInfo( void* const );
	HInfo( HInfo const& );
	virtual ~HInfo( void );
	char operator [] ( int const );
	HInfo& operator = ( HInfo const& );
	HInfo& operator() ( HInfo const& );
	template<typename tType>
	tType get( void ) const;
	bool operator ! ( void ) const;
private:
	void purge( void );
	};

}

}

#endif /* not YAAL_HCORE_HINFO_HXX_INCLUDED */
