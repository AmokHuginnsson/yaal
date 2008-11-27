/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hchunk.h - this file is integral part of `yaal' project.

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
/*! \file hcore/hchunk.h
 * \brief Declaration of HChunk class.
 */

#ifndef __YAAL_HCORE_HCHUNK_H
#define __YAAL_HCORE_HCHUNK_H

#include "hcore/hpointer.h"

namespace yaal
{

namespace hcore
{

/*! \brief Simplest raw memory provisioner.
 */
class HChunk
	{
	void* f_pvData;
public:
	typedef yaal::hcore::HPointer<HChunk> ptr_t;
	HChunk( void* );
	~HChunk( void );
	template<typename tType>
	tType* get() const
		{ return ( static_cast<tType*>( f_pvData ) ); }
	void set( void* );
	void clear( void );
private:
	HChunk( HChunk const& );
	HChunk& operator = ( HChunk const& );
	};

}

}

#endif /* not __YAAL_HCORE_HCHUNK_H */

