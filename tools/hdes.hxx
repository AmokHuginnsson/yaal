/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hdes.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HDES_HXX_INCLUDED
#define YAAL_TOOLS_HDES_HXX_INCLUDED 1

#include "hcore/pod.hxx"
#include "hcore/hstring.hxx"

namespace yaal {

namespace tools {

/*! \brief DES algorithm constants.
 */
struct DES {
	static int const BITS_IN_BYTE  = yaal::meta::power<2,3>::value;
	static int const SIDES_COUNT   = 2;
	static int const IKEY_SIZE     = 6;
	static int const BLOCK_SIZE    = 8;
	static int const IKEYS_COUNT   = 16;
	static int const PASSWORD_SIZE = 16;
	static int const HALF_KEY_SIZE = 28;
};

/*! \brief Cryptography interface.
 *
 * Small class incorporates simple cryptography functionality.
 */
class HDes final {
public:
	typedef HDes this_type;
	typedef enum {
		CRYPT = 0,
		DECRYPT = 1
	} action_t;
private:
	u32_t _IKeys[ DES::SIDES_COUNT ][ DES::IKEYS_COUNT ][ ( DES::IKEY_SIZE + static_cast<int>( sizeof ( u32_t ) ) - 1 ) / static_cast<int>( sizeof ( u32_t ) ) ];
public:
	HDes( yaal::hcore::HString const& );
	~HDes( void );
	void crypt( u8_t*, int long, action_t const& );
	void flush_keys( void );
private:
	void generate_keys( u8_t const*, int );
	void _des( u32_t* /* block */, int /* action */, int /* part */ );
	void _3des( u32_t* /* block */, int /* action */ );
	void permute( u8_t* /* buffer */, const u8_t* /* tab */, int /* len */ ) const;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HDES_HXX_INCLUDED */

