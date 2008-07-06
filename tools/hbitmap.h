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

class HBitmap
	{
	int long f_lAllocatedBytes;
	int long f_lSize;
	void* f_pvBlock;
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
	void clear( void );
	int long get_size( void ) const;
	int long size( void ) const;
	void const* raw( void ) const;
	void copy( void const*, int long const& );
	void use( void*, int long const& );
	bool get( int long const& ) const;
	void set( int long const&, bool const& );
	void rotate_left( int long const&, int long const&, int long const& );
	void rotate_right( int long const&, int long const&, int long const& );
private:
	int long octets_for_bits( int long const& ) const;
	};

}

}

#endif /* not __YAAL_TOOLS_HBITMAP_H */

