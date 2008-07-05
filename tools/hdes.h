/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdes.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_TOOLS_HDES_H
#define __YAAL_TOOLS_HDES_H

namespace yaal
{

namespace tools
{

struct DES
	{
	static int const D_BITS_IN_BYTE  = yaal::power<2,3>::value;
	static int const D_SIDES_COUNT   = 2;
	static int const D_IKEY_SIZE     = 6;
	static int const D_BLOCK_SIZE    = 8;
	static int const D_IKEYS_COUNT   = 16;
	static int const D_PASSWORD_SIZE = 16;
	static int const D_HALF_KEY_SIZE = 28;
	};

/*! \brief Cryptography interface.
 * 
 * Small class incorporates simple cryptography functionality.
 */
class HDes
	{
	u8_t f_pppcIKeys[ DES::D_SIDES_COUNT ][ DES::D_IKEYS_COUNT ][ DES::D_IKEY_SIZE ];
public:
	HDes( void );
	virtual ~HDes( void );
	void crypt( u8_t*, int, int );
	void generate_keys( u8_t* );
	void flush_keys( void );
private:
	void _des( u8_t* /* block */, int /* side */, int /* part */ );
	void _3des( u8_t* /* block */, int /* side */ );
	void permutate( u8_t* /* buffer */, const u8_t* /* tab */, int /* len */ ) const;
	};

}

}

#endif /* not __YAAL_TOOLS_HDES_H */

