/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hclock.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hclock.hxx
 * \brief Declaration of HClock class.
 */

#ifndef YAAL_HCORE_HCLOCK_HXX_INCLUDED
#define YAAL_HCORE_HCLOCK_HXX_INCLUDED 1

namespace yaal
{

namespace hcore
{

/*! \brief High resolution time mesurement tool.
 */
class HClock
	{
public:
	/*! \brief Resolution units for time mesurement.
	 */
	struct UNIT
		{
		typedef enum
			{
			SECOND = 0,
			NANOSECOND = 1,
			MICROSECOND = 2,
			MILISECOND = 3
			} unit_t;
		};
private:
	mutable int long _moment[2];
public:
	HClock( void );
	int long get_time_elapsed( UNIT::unit_t const& = UNIT::SECOND, bool = false ) const;
	};

}

}

#endif /* #ifndef YAAL_HCORE_HCLOCK_HXX_INCLUDED */

