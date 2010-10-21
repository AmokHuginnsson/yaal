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
#define YAAL_HCORE_HCHUNK_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Simplest raw memory provisioner.
 */
class HChunk
	{
	int long _size;
	void* _data;
public:
	struct STRATEGY
		{
		typedef enum
			{
			GEOMETRIC,
			EXACT
			} enum_t;
		};
	typedef yaal::hcore::HPointer<HChunk> ptr_t;
	/*! \brief Create empty memory holder.
	 */
	HChunk( void );
	/*! \brief Create memory chunk guard and allocate memory for it.
	 *
	 * \param size - requested memory chunk size.
	 */
	explicit HChunk( int long size, STRATEGY::enum_t = STRATEGY::EXACT );
	~HChunk( void );
	template<typename tType>
	tType const* get() const
		{ return ( static_cast<tType const*>( _data ) ); }
	template<typename tType>
	tType* get()
		{ return ( static_cast<tType*>( _data ) ); }
	char const* raw( void ) const
		{ return ( static_cast<char const*>( _data ) ); }
	char* raw( void )
		{ return ( static_cast<char*>( _data ) ); }
	void free( void );
	void* realloc( int long, STRATEGY::enum_t = STRATEGY::GEOMETRIC );
	void swap( HChunk& );
	int long get_size( void ) const
		{ return ( _size ); }
	int long size( void ) const
		{ return ( _size ); }
private:
	HChunk( HChunk const& );
	HChunk& operator = ( HChunk const& );
	};

template<typename T>
int long chunk_size( int long count_ )
	{
	return ( sizeof ( T ) * count_ );
	}

}

inline void swap( yaal::hcore::HChunk& a, yaal::hcore::HChunk& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HCHUNK_HXX_INCLUDED */

