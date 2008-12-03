/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmemory.h - this file is integral part of hanalyser.h project.

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

#ifndef YAAL_TOOLS_HMEMORY_HXX_INCLUDED
#define YAAL_TOOLS_HMEMORY_HXX_INCLUDED

#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Stream based access to raw memory block.
 *
 * This stream based memory accessor wrapper does not own the memory block.
 */
class HMemory : public yaal::hcore::HStreamInterface
	{
	void* f_pvBlock;
	int long f_lSize;
public:
	/*! \brief Create new memory accessor.
	 *
	 * \param ptr - pointer to memory block to be wrapped.
	 * \param size - size of memoru block in octets.
	 */
	HMemory( void* ptr, int long const& size );
	/*! \brief Copy constructor.
	 *
	 * \param src - original HMemory object to be copied.
	 */
	HMemory( HMemory const& src );
	/*! \brief Assign operator.
	 *
	 * \param src - original HMemory object to be copied.
	 * \return Self.
	 */
	HMemory& operator = ( HMemory const& src );
	};

}

}

#endif /* not YAAL_TOOLS_HMEMORY_HXX_INCLUDED */

