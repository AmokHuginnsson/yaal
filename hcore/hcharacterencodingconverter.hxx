/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcharacterencodingconverter.hxx - this file is integral part of `yaal' project.

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

/*! \file hcharacterencodingconverter.hxx
 * \brief Declaration HCharacterEncodingConverter class.
 */

#ifndef YAAL_HCORE_HCHARACTERENCODINGCONVERTER_HXX_INCLUDED
#define YAAL_HCORE_HCHARACTERENCODINGCONVERTER_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace hcore {

class HCharacterEncodingConverter final {
public:
	typedef HCharacterEncodingConverter this_type;
	typedef yaal::u64_t descriptor_t;
private:
	yaal::hcore::HString _nameFrom;
	yaal::hcore::HString _nameTo;
	descriptor_t _descriptor;
	yaal::hcore::HString _cache;
public:
	HCharacterEncodingConverter( yaal::hcore::HString const&, yaal::hcore::HString const& );
	HCharacterEncodingConverter( HCharacterEncodingConverter const& );
	HCharacterEncodingConverter( HCharacterEncodingConverter&& );
	HCharacterEncodingConverter& operator = ( HCharacterEncodingConverter const& );
	HCharacterEncodingConverter& operator = ( HCharacterEncodingConverter&& );
	~HCharacterEncodingConverter( void );
	void swap( HCharacterEncodingConverter& );
	void reset( void );
	HString const& name_from( void ) const;
	HString const& name_to( void ) const;
	HString const& convert( HString const& );
	HString const& convert( char const*, int long );
	HString const& convert( char const* );
};

typedef yaal::hcore::HExceptionT<HCharacterEncodingConverter> HCharacterEncodingConverterException;

inline void swap( yaal::hcore::HCharacterEncodingConverter& a_, yaal::hcore::HCharacterEncodingConverter& b_ ) {
	a_.swap( b_ );
}

}

}

#endif /* #ifndef YAAL_HCORE_HCHARACTERENCODINGCONVERTER_HXX_INCLUDED */

