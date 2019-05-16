/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/hjson.hxx
 * \brief Declaration HJSON class.
 */

#ifndef YAAL_TOOLS_HJSON_HXX_INCLUDED
#define YAAL_TOOLS_HJSON_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hvariant.hxx"
#include "hcore/hnumber.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hexception.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

class HJSON {
public:
	typedef HJSON this_type;
	class HValue {
	public:
		typedef HValue this_type;
		enum class TYPE {
			UNINITIALIZED = -1,
			STRING = 0,
			INTEGER = 1,
			REAL = 2,
			NUMBER = 3,
			ARRAY = 4,
			MAP = 5,
			LITERAL = 6
		};
		enum class LITERAL {
#undef TRUE
			TRUE,
#undef FALSE
			FALSE,
#undef NULL
			NULL
		};
		typedef yaal::hcore::HArray<HValue> array_t;
		typedef yaal::hcore::HHashMap<yaal::hcore::HString, HValue> members_t;
		typedef yaal::hcore::HVariant<yaal::hcore::HString, int long long, double long, yaal::hcore::HNumber, array_t, members_t, LITERAL> data_t;
	private:
		data_t _data;
	public:
		HValue( void );
		HValue( HValue const& ) = default;
		HValue( HValue&& ) = default;
		HValue& operator = ( HValue const& ) = default;
		HValue& operator = ( HValue&& ) = default;
		HValue( char const* value_ ) : HValue( hcore::to_string( value_ ) ) {}
		HValue( yaal::hcore::HString const& );
		HValue( int long long );
		HValue( int long value_ ) : HValue( static_cast<int long long>( value_ ) ) {}
		HValue( int value_ ) : HValue( static_cast<int long long>( value_ ) ) {}
		HValue( int short value_ ) : HValue( static_cast<int long long>( value_ ) ) {}
		HValue( double long );
		HValue( double value_ ) : HValue( static_cast<double long>( value_ ) ) {}
		HValue( float value_ ) : HValue( static_cast<double long>( value_ ) ) {}
		HValue( yaal::hcore::HNumber const& );
		HValue( array_t const& );
		HValue( members_t const& );
		HValue( LITERAL );
		void push_back( HValue const& );
		HValue& operator[] ( int );
		HValue& operator[] ( int long );
		HValue& operator[] ( yaal::hcore::HString const& );
		HValue const& operator[] ( int ) const;
		HValue const& operator[] ( int long ) const;
		HValue const& operator[] ( yaal::hcore::HString const& ) const;
		yaal::hcore::HString const& get_string( void ) const;
		int long long get_integer( void ) const;
		double long get_real( void ) const;
		yaal::hcore::HNumber const& get_number( void ) const;
		LITERAL get_literal( void ) const;
		array_t const& get_elements( void ) const;
		members_t const& get_members( void ) const;
		TYPE type( void ) const;
	};
	typedef yaal::hcore::HResource<HValue> value_t;
private:
	HValue _element;
public:
	HJSON( void );
	HValue& element( void );
	void load( yaal::hcore::HStreamInterface& );
	void save( yaal::hcore::HStreamInterface&, bool = true );
};

typedef yaal::hcore::HExceptionT<HJSON> HJSONException;

}

}

#endif /* #ifndef YAAL_TOOLS_HJSON_HXX_INCLUDED */

