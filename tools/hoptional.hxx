/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hoptional.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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

namespace yaal
{

namespace tools
{

/*! \brief HOptional<> class template - an optional value.
 * Provide interface for values that can be unitialized.
 */
template<typename type_t>
class HOptional
	{
public:
	struct SemanticContext { SemanticContext const& member( SemanticContext& ) { return ( *this ); } };
	typedef SemanticContext const& ( SemanticContext::* safe_bool_t )( SemanticContext& );
	typedef type_t value_type;
private:
	char _data[ sizeof ( value_type ) ];
	bool _initialized;
public:
	HOptional( void ) : _data(), _initialized( false ) {}
	HOptional( value_type const& value_ )
		: _data(), _initialized( true )
		{
		new ( _data ) value_type( value_ );
		}
	~HOptional( void )
		{
		if ( _initialized )
			operator->()->~value_type();
		}
	HOptional( HOptional const& other_ )
		: _data(), _initialized( other_._initialized )
		{
		if ( _initialized )
			new ( _data ) value_type( *other_ );
		}
	HOptional& operator = ( HOptional const& other_ )
		{
		if ( &other_ != this )
			{
			HOptional tmp( other_ );
			swap( tmp );
			}
		return ( *this );
		}
	void swap( HOptional& other_ )
		{
		if ( &other_ != this )
			{
			using yaal::swap;
			swap( _initialized, other_._initialized );
			swap_ranges( _data, _data + sizeof ( value_type ), other_._data );
			}
		return;
		}
	operator safe_bool_t()
		{
		return ( _initialized ? &SemanticContext::member : NULL );
		}
	bool operator! ( void ) const
		{
		return ( ! _initialized );
		}
	value_type const& operator* ( void ) const
		{
		M_ASSERT( _initialized );
		return ( *static_cast<value_type const*>( static_cast<void const*>( _data ) ) );
		}
	value_type& operator* ( void )
		{
		M_ASSERT( _initialized );
		return ( *static_cast<value_type*>( static_cast<void*>( _data ) ) );
		}
	value_type const* operator->( void ) const
		{
		M_ASSERT( _initialized );
		return ( static_cast<value_type const*>( static_cast<void const*>( _data ) ) );
		}
	value_type* operator->( void )
		{
		M_ASSERT( _initialized );
		return ( static_cast<value_type*>( static_cast<void*>( _data ) ) );
		}
	};

/*! \cond */
template<typename type_t>
class HOptional<type_t&>
	{
public:
	struct SemanticContext { SemanticContext const& member( SemanticContext& ) { return ( *this ); } };
	typedef SemanticContext const& ( SemanticContext::* safe_bool_t )( SemanticContext& );
	typedef typename trait::strip_reference<type_t>::type value_type;
private:
	char _data[ sizeof ( value_type* ) ];
	bool _initialized;
public:
	HOptional( void ) : _data(), _initialized( false ) {}
	HOptional( value_type const& value_ )
		: _data(), _initialized( true )
		{
		*static_cast<value_type**>( static_cast<void*>( _data ) ) = &value_;
		}
	HOptional( HOptional const& other_ )
		: _data(), _initialized( other_._initialized )
		{
		if ( _initialized )
			*static_cast<value_type**>( static_cast<void*>( _data ) ) = *static_cast<value_type const* const*>( static_cast<void const*>( other_._data ) );
		}
	HOptional& operator = ( HOptional const& other_ )
		{
		if ( &other_ != this )
			{
			HOptional tmp( other_ );
			swap( tmp );
			}
		return ( *this );
		}
	void swap( HOptional& other_ )
		{
		if ( &other_ != this )
			{
			using yaal::swap;
			swap( _initialized, other_._initialized );
			swap( *static_cast<value_type**>( static_cast<void*>( _data ) ), *static_cast<value_type**>( static_cast<void*>( other_._data ) ) );
			}
		return;
		}
	operator safe_bool_t()
		{
		return ( _initialized ? &SemanticContext::member : NULL );
		}
	bool operator! ( void ) const
		{
		return ( ! _initialized );
		}
	value_type const& operator* ( void ) const
		{
		M_ASSERT( _initialized );
		return ( **static_cast<value_type const* const*>( static_cast<void const*>( _data ) ) );
		}
	value_type& operator* ( void )
		{
		M_ASSERT( _initialized );
		return ( **static_cast<value_type**>( static_cast<void*>( _data ) ) );
		}
	value_type const* operator->( void ) const
		{
		M_ASSERT( _initialized );
		return ( *static_cast<value_type const* const*>( static_cast<void const*>( _data ) ) );
		}
	value_type* operator->( void )
		{
		M_ASSERT( _initialized );
		return ( *static_cast<value_type**>( static_cast<void*>( _data ) ) );
		}
	};
/*! \endcond */

}

template<typename type_t>
inline void swap( yaal::tools::HOptional<type_t>& a, yaal::tools::HOptional<type_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_TOOLS_HOPTIONAL_HXX_INCLUDED */

