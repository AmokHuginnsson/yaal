/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/model.hxx
 * \brief Declaration of \e `model` namespace.
 */

#ifndef YAAL_TOOLS_MODEL_HXX_INCLUDED
#define YAAL_TOOLS_MODEL_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hvariant.hxx"
#include "hcore/hnumber.hxx"
#include "hcore/htime.hxx"
#include "hcore/hexception.hxx"

namespace yaal {

namespace tools {

/*! \brief A namespace for Hierarchical/`Tree based` Data Object/Value Model related classes.
 */
namespace model {

class HValue {
public:
	typedef HValue this_type;
	enum class TYPE {
		UNINITIALIZED = -1,
		STRING = 0,
		INTEGER = 1,
		REAL = 2,
		NUMBER = 3,
		TIME = 4,
		ARRAY = 5,
		MAP = 6,
		LITERAL = 7
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
	typedef yaal::hcore::HOrderedHashMap<yaal::hcore::HString, HValue> members_t;
	typedef yaal::hcore::HVariant<yaal::hcore::HString, int long long, double long, yaal::hcore::HNumber, yaal::hcore::HTime, array_t, members_t, LITERAL> data_t;
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
	HValue( yaal::hcore::HTime const& );
	HValue( array_t const& );
	HValue( array_t&& );
	HValue( members_t const& );
	HValue( members_t&& );
	HValue( LITERAL );
	bool operator == ( HValue const& ) const;
	bool operator != ( HValue const& other_ ) const {
		return ( ! operator == ( other_ ) );
	}
	void push_back( HValue const& );
	void push_back( HValue&& );
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
	yaal::hcore::HTime const& get_time( void ) const;
	LITERAL get_literal( void ) const;
	array_t const& get_elements( void ) const;
	array_t& get_elements( void );
	members_t const& get_members( void ) const;
	members_t& get_members( void );
	TYPE type( void ) const;
	void reset( void );
};

struct Model {};
typedef yaal::hcore::HExceptionT<Model> HModelException;

}

}

}

#endif /* #ifndef YAAL_TOOLS_MODEL_HXX_INCLUDED */

