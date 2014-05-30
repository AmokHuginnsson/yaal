/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hoptional.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hoptional.hxx
 * \brief HOptional<> template class implementation.
 */

#ifndef YAAL_TOOLS_HOPTIONAL_HXX_INCLUDED
#define YAAL_TOOLS_HOPTIONAL_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace tools {

/*! \brief HOptional<> class template - an optional value.
 * Provide interface for values that can be unitialized.
 */
template<typename type_t>
class HOptional {
public:
	typedef HOptional<type_t> this_type;
	struct SemanticContext { SemanticContext const& member( SemanticContext& ) const { return ( *this ); } };
	typedef SemanticContext const& ( SemanticContext::* safe_bool_t )( SemanticContext& ) const;
	typedef type_t value_type;
private:
	char _data[ sizeof ( value_type ) ];
	bool _initialized;
public:
	HOptional( void ) : _data(), _initialized( false ) {}
	HOptional( value_type const& value_ )
		: _data(), _initialized( true ) {
		new ( _data ) value_type( value_ );
	}
	~HOptional( void ) {
		M_PROLOG
		if ( _initialized )
			M_SAFE( operator->()->~value_type() );
		M_DESTRUCTOR_EPILOG
	}
	HOptional( HOptional const& other_ )
		: _data(), _initialized( other_._initialized ) {
		if ( _initialized )
			new ( _data ) value_type( *other_ );
	}
	HOptional& operator = ( HOptional const& other_ ) {
		if ( &other_ != this ) {
			HOptional tmp( other_ );
			swap( tmp );
		}
		return ( *this );
	}
	void swap( HOptional& other_ ) {
		if ( &other_ != this ) {
			using yaal::swap;
			swap( _initialized, other_._initialized );
			swap_ranges( _data, _data + sizeof ( value_type ), other_._data );
		}
		return;
	}
	operator safe_bool_t() const {
		return ( _initialized ? &SemanticContext::member : NULL );
	}
	bool operator! ( void ) const {
		return ( ! _initialized );
	}
	value_type const& operator* ( void ) const {
		M_ASSERT( _initialized );
		value_type const* v( static_cast<value_type const*>( static_cast<void const*>( _data ) ) );
		return ( *v );
	}
	value_type& operator* ( void ) {
		M_ASSERT( _initialized );
		value_type* v( static_cast<value_type*>( static_cast<void*>( _data ) ) );
		return ( *v );
	}
	value_type const* operator->( void ) const {
		M_ASSERT( _initialized );
		return ( static_cast<value_type const*>( static_cast<void const*>( _data ) ) );
	}
	value_type* operator->( void ) {
		M_ASSERT( _initialized );
		return ( static_cast<value_type*>( static_cast<void*>( _data ) ) );
	}
};

/*! \cond */
template<typename type_t>
class HOptional<type_t&> {
public:
	struct SemanticContext { SemanticContext const& member( SemanticContext& ) { return ( *this ); } };
	typedef SemanticContext const& ( SemanticContext::* safe_bool_t )( SemanticContext& );
	typedef typename trait::strip_reference<type_t>::type value_type;
private:
	value_type* _value;
	bool _initialized;
public:
	HOptional( void ) : _value(), _initialized( false ) {}
	HOptional( value_type& value_ )
		: _value( &value_ ), _initialized( true ) {
	}
	HOptional( HOptional const& other_ )
		: _value(), _initialized( other_._initialized ) {
		if ( _initialized ) {
			_value = other_._value;
		}
	}
	template<typename other_t>
	explicit HOptional( HOptional<other_t&> const& other_ )
		: _value(), _initialized( other_ ) {
		STATIC_ASSERT(( trait::same_type<type_t, other_t>::value || trait::same_type<type_t, other_t const>::value ));
		if ( _initialized ) {
			_value = reinterpret_cast<HOptional const&>( other_ )._value;
		}
	}
	HOptional( HOptional<type_t>& other_ )
		: _value(), _initialized( other_ ) {
		if ( _initialized ) {
			_value = &*other_;
		}
	}
	HOptional& operator = ( HOptional const& other_ ) {
		if ( &other_ != this ) {
			HOptional tmp( other_ );
			swap( tmp );
		}
		return ( *this );
	}
	template<typename other_t>
	HOptional& operator = ( HOptional<other_t> const& other_ ) {
		if ( reinterpret_cast<HOptional const*>( &other_ ) != this ) {
			HOptional tmp( other_ );
			swap( tmp );
		}
		return ( *this );
	}
	void swap( HOptional& other_ ) {
		if ( &other_ != this ) {
			using yaal::swap;
			swap( _initialized, other_._initialized );
			swap( _value, other_._value );
		}
		return;
	}
	operator safe_bool_t() const {
		return ( _initialized ? &SemanticContext::member : NULL );
	}
	bool operator! ( void ) const {
		return ( ! _initialized );
	}
	value_type const& operator* ( void ) const {
		M_ASSERT( _initialized );
		return ( *_value );
	}
	value_type& operator* ( void ) {
		M_ASSERT( _initialized );
		return ( *_value );
	}
	value_type const* operator->( void ) const {
		M_ASSERT( _initialized );
		return ( _value );
	}
	value_type* operator->( void ) {
		M_ASSERT( _initialized );
		return ( _value );
	}
};
/*! \endcond */

template<typename type_t>
inline type_t get_optional_value_or( HOptional<type_t> const& value_, type_t default_ ) {
	return ( value_ ? *value_ : default_ );
}

template<typename type_t>
inline type_t const& get_optional_value_or( HOptional<type_t const&> const& value_, type_t const& default_ ) {
	return ( value_ ? *value_ : default_ );
}

}

template<typename type_t>
inline void swap( yaal::tools::HOptional<type_t>& a, yaal::tools::HOptional<type_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_TOOLS_HOPTIONAL_HXX_INCLUDED */

