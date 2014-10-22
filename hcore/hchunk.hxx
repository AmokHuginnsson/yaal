/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hchunk.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hchunk.hxx
 * \brief Declaration of HChunk class.
 */

#ifndef YAAL_HCORE_HCHUNK_HXX_INCLUDED
#define YAAL_HCORE_HCHUNK_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"

namespace yaal {

namespace hcore {

/*! \brief Simplest raw memory provisioner.
 */
class HChunk {
public:
	typedef HChunk this_type;
private:
	int long _size;
	void* _data;
public:
	struct STRATEGY {
		typedef enum {
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
	HChunk( HChunk&& );
	HChunk& operator = ( HChunk&& );
	~HChunk( void );
	template<typename tType>
	tType const* get() const
		{ return ( static_cast<tType const*>( _data ) ); }
	template<typename tType>
	tType* get()
		{ return ( static_cast<tType*>( _data ) ); }
	void const* raw( void ) const
		{ return ( static_cast<char const*>( _data ) ); }
	void* raw( void )
		{ return ( static_cast<char*>( _data ) ); }
	void free( void );
	void* realloc( int long, STRATEGY::enum_t = STRATEGY::GEOMETRIC );
	void swap( HChunk& );
	int long get_size( void ) const
		{ return ( _size ); }
	int long size( void ) const
		{ return ( _size ); }
	template<typename T>
	int long count_of( void ) const
		{ return ( _size / static_cast<int>( sizeof ( T ) ) ); }
private:
	HChunk( HChunk const& );
	HChunk& operator = ( HChunk const& );
};

template<typename T>
int long chunk_size( int long count_ ) {
	return ( static_cast<int>( sizeof ( T ) ) * count_ );
}

inline void swap( yaal::hcore::HChunk& a, yaal::hcore::HChunk& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HCHUNK_HXX_INCLUDED */

