/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HINFO_HXX_INCLUDED
#define YAAL_HCORE_HINFO_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/htime.hxx"
#include "hcore/hexception.hxx" /* types definitions */
#include "hcore/hstrongenum.hxx"
#include "hcore/reflection.hxx"

namespace yaal {

namespace hcore {

/*! \brief Type agnostic data container.
 */
class HInfo {
	type_id_t		_type;
	int				_int;
	int long	_intLong;
	double		_double;
	void*		_pointer;
	HString		_string;
	HTime			_time;
public:
	HInfo( void );
	HInfo( int );
	HInfo( int long );
	HInfo( double );
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

#endif /* #ifndef YAAL_HCORE_HINFO_HXX_INCLUDED */

