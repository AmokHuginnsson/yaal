/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	streamtools.hxx - this file is integral part of yaal project.

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

/*! \file tools/streamtools.hxx
 * \brief Various yaal::hcore::HStreamInterface utils.
 *
 * HBinaryFormatter - `bin' mainpulator for yaal::hcore::HStreamInterface.
 */

#ifndef YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED
#define YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

/*! \brief New manipulator for yaal::hcore::HStreamInterface, `bin'.
 *
 * Output PODs as in binary number format.
 */
class HBinaryFormatter
	{
	yaal::hcore::HStreamInterface* _stream;
public:
	HBinaryFormatter( yaal::hcore::HStreamInterface* );
	template<typename T>
	HBinaryFormatter& operator << ( T const& v )
		{
		return ( binary( &v, sizeof ( T ) ) );
		}
	yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface::manipulator_t const& );
	HBinaryFormatter& binary( void const*, int );
	};

class HBinaryFormatterSeed
	{
public:
	HBinaryFormatter create( yaal::hcore::HStreamInterface* ) const;
	} extern bin;

HBinaryFormatter operator << ( yaal::hcore::HStreamInterface&, HBinaryFormatterSeed const& );

}

}

#endif /* #ifndef YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED */

