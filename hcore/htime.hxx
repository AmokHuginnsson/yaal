/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htime.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HTIME_HXX_INCLUDED
#define YAAL_HCORE_HTIME_HXX_INCLUDED 1

#include <ctime>

#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _defaultTimeFormat_;

/*! \brief Date and time handling class.
 */
class HTime {
	typedef HTime this_type;
	HString	_format;
	mutable HChunk _cache;
	time_t	_value;
	tm			_broken;
public:
	HTime( void );
	HTime( HTime const& );
	HTime( char const* const );
	HTime( time_t const& );
	HTime( int const, int const, int const, int const = 0, int const = 0,
			int const = 0 );
	virtual ~HTime ( void );
	void swap( HTime& );
	void set_now( void );
	void set( time_t const& );
	void format( char const* const = _defaultTimeFormat_ );
	void set_time( int const = 0, int const = 0, int const = 0 );
	void set_date( int const, int const = 1, int const = 1 );
	void set_datetime( int const, int const = 1, int const = 1,
						 int const = 0, int const = 0, int const = 0 );
	int get_year( void ) const;
	int get_month( void ) const;
	int get_day( void ) const;
	int get_hour( void ) const;
	int get_minute( void ) const;
	int get_second( void ) const;
	HTime& operator = ( HTime const& );
	HTime operator - ( HTime const& ) const;
	HTime& operator -= ( HTime const& );
	bool operator == ( time_t const& ) const;
	bool operator != ( time_t const& ) const;
	bool operator <= ( time_t const& ) const;
	bool operator >= ( time_t const& ) const;
	bool operator < ( time_t const& ) const;
	bool operator > ( time_t const& ) const;
	HString string( void ) const;
	time_t raw( void ) const;
};

typedef HExceptionT<HTime> HTimeException;

}

inline void swap( yaal::hcore::HTime& a, yaal::hcore::HTime& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HTIME_HXX_INCLUDED */

