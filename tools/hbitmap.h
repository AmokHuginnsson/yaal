/*
---        `yaal' (c) 1978 by Marcin 'Amok' Konarski             ---

	hbitmap.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_TOOLS_HBITMAP_H
#define __YAAL_TOOLS_HBITMAP_H

namespace yaal
{

namespace tools
{

/*! \brief Abstract for bit pool bit operations.
 */
class HBitmap
	{
	typedef HBitmap self_t;
public:
	class HBit;
	template<typename const_qual_t>
	class HIterator;
	typedef HIterator<bool> const_iterator;
	typedef HIterator<HBit> iterator;
private:
	int long f_lAllocatedBytes;
	int long f_lSize;
	void* f_pvData;
public:
	HBitmap( void );
	~HBitmap( void );
	HBitmap( int long const& );
	HBitmap( HBitmap const& );
	HBitmap& operator = ( HBitmap const& );
	bool operator == ( HBitmap const& ) const;
	bool operator != ( HBitmap const& ) const;
	HBitmap& operator |= ( HBitmap const& );
	HBitmap& operator &= ( HBitmap const& );
	HBitmap& operator ^= ( HBitmap const& );
	HBitmap& operator <<= ( int long const& );
	HBitmap& operator >>= ( int long const& );
	HBitmap operator | ( HBitmap const& ) const;
	HBitmap operator & ( HBitmap const& ) const;
	HBitmap operator ^ ( HBitmap const& ) const;
	HBitmap operator << ( int long const& );
	HBitmap operator >> ( int long const& );
	HBitmap& operator += ( HBitmap const& );
	void clear( void );
	void fill( bool const& );
	void fill( int long const&, int long const&, bool const& );
	void push_back( bool const& );
	int long get_size( void ) const;
	int long size( void ) const;
	void const* raw( void ) const;
	void copy( void const*, int long const& );
	void use( void*, int long const& );
	void reserve( int long const& );
	const_iterator begin( void ) const;
	const_iterator find( int long const& ) const;
	const_iterator end( void ) const;
	const_iterator rbegin( void ) const;
	const_iterator rend( void ) const;
	iterator begin( void );
	iterator find( int long const& );
	iterator end( void );
	iterator rbegin( void );
	iterator rend( void );
	bool get( int long const& ) const;
	void set( int long const&, bool const& );
	void rotate_left( int long const&, int long const&, int long const& );
	void rotate_right( int long const&, int long const&, int long const& );
private:
	void ensure_pool( int long const& );
	int long octets_for_bits( int long const& ) const;
	void* block( void );
	void const* block( void ) const;
	};

typedef yaal::hcore::HExceptionT<HBitmap> HBitmapException;

namespace bitmap_type_helper
{

/*! \brief Get owner constness based on child type.
 *
 * \tparam input - child type.
 * \retval owner_t - const qualified owner type.
 */
template<typename input>
struct owner_const_qual_from_type
	{
	};

/*! \cond */
template<>
struct owner_const_qual_from_type<bool>
	{
	typedef HBitmap const* owner_t;
	};
template<>
struct owner_const_qual_from_type<HBitmap::HBit>
	{
	typedef HBitmap* owner_t;
	};
/*! \endcond */

}

/*! \brief Bit iterator, allows access to single bits in sequence.
 */
template<typename const_qual_t>
class HBitmap::HIterator
	{
	typedef typename bitmap_type_helper::owner_const_qual_from_type<const_qual_t>::owner_t owner_t;
	owner_t f_poOwner;
	int long f_lIndex;
public:
	HIterator( void ) : f_poOwner( NULL ), f_lIndex( 0 ) {}
	HIterator( HIterator const& it ) : f_poOwner( it.f_poOwner ), f_lIndex( it.f_lIndex ) {}
	HIterator& operator ++ ( void )
		{
		++ f_lIndex;
		return ( *this );
		}
	HIterator operator ++ ( int )
		{
		HIterator it( *this );
		++ f_lIndex;
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		-- f_lIndex;
		return ( *this );
		}
	HIterator operator -- ( int )
		{
		HIterator it( *this );
		-- f_lIndex;
		return ( it );
		}
	HIterator& operator = ( HIterator const& it )
		{
		if ( &it != this )
			{
			f_poOwner = it.f_poOwner;
			f_lIndex = it.f_lIndex;
			}
		return ( *this );
		}
	bool operator == ( HIterator const& it ) const
		{
		M_ASSERT( f_poOwner == it.f_poOwner );
		return ( f_lIndex == it.f_lIndex );
		}
	bool operator != ( HIterator const& it ) const
		{
		M_ASSERT( f_poOwner == it.f_poOwner );
		return ( f_lIndex != it.f_lIndex );
		}
	const_qual_t operator* ( void );
private:
	friend class yaal::tools::HBitmap;
	HIterator( owner_t a_poOwner, int long const& idx )
		: f_poOwner( a_poOwner ), f_lIndex( idx ) {}
	};

class HBitmap::HBit
	{
	HBitmap* f_poOwner;
	int long f_lIndex;
public:
	HBit( HBit const& );
	HBit& operator = ( HBit const& );
	HBit& operator = ( bool const& );
	bool operator == ( bool const& ) const;
	bool operator != ( bool const& ) const;
	operator bool ( void ) const;
private:
	friend class HBitmap::HIterator<HBitmap::HBit>;
	HBit( HBitmap*, int long const& );
	};

}

}

#endif /* not __YAAL_TOOLS_HBITMAP_H */

