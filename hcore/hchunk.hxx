/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hchunk.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hchunk.hxx
 * \brief Declaration of HChunk class.
 */

#ifndef YAAL_HCORE_HCHUNK_HXX_INCLUDED
#define YAAL_HCORE_HCHUNK_HXX_INCLUDED

#include "hcore/hpointer.hxx"

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
	tType const* get() const
		{ return ( static_cast<tType*>( f_pvData ) ); }
	template<typename tType>
	tType* get()
		{ return ( static_cast<tType*>( f_pvData ) ); }
	void set( void* );
	void clear( void );
private:
	HChunk( HChunk const& );
	HChunk& operator = ( HChunk const& );
	};

}

}

#endif /* not YAAL_HCORE_HCHUNK_HXX_INCLUDED */

