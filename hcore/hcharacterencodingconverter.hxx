/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcharacterencodingconverter.hxx
 * \brief Declaration HCharacterEncodingConverter class.
 */

#ifndef YAAL_HCORE_HCHARACTERENCODINGCONVERTER_HXX_INCLUDED
#define YAAL_HCORE_HCHARACTERENCODINGCONVERTER_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hchunk.hxx"

namespace yaal {

namespace hcore {

class HCharacterEncodingConversionResult;

class HCharacterEncodingConverter final {
public:
	typedef HCharacterEncodingConverter this_type;
	typedef yaal::u64_t descriptor_t;
private:
	yaal::hcore::HString _nameFrom;
	yaal::hcore::HString _nameTo;
	descriptor_t _descriptor;
	yaal::hcore::HChunk _cache;
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
	void convert( char const*, int long, HCharacterEncodingConversionResult& );
private:
	void init( void );
};

typedef yaal::hcore::HExceptionT<HCharacterEncodingConverter> HCharacterEncodingConverterException;

class HCharacterEncodingConversionResult {
	yaal::hcore::HChunk _data;
	int long _size;
public:
	HCharacterEncodingConversionResult( void );
	char const* c_str( void ) const;
	int long size( void ) const;
private:
	friend void HCharacterEncodingConverter::convert( char const*, int long, HCharacterEncodingConversionResult& );
	HCharacterEncodingConversionResult( HCharacterEncodingConversionResult const& ) = delete;
	HCharacterEncodingConversionResult& operator = ( HCharacterEncodingConversionResult const& ) = delete;
};

inline void swap( yaal::hcore::HCharacterEncodingConverter& a_, yaal::hcore::HCharacterEncodingConverter& b_ ) {
	a_.swap( b_ );
}

}

}

#endif /* #ifndef YAAL_HCORE_HCHARACTERENCODINGCONVERTER_HXX_INCLUDED */

