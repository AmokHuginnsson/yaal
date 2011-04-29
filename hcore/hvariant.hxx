/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hvariant.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HVARIANT_HXX_INCLUDED
#define YAAL_HCORE_HVARIANT_HXX_INCLUDED 1

#include <new> /* holly shit */

#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Multi type container.
 *
 * A variant type without non-empty guarantee.
 * Represent variable that can be used as one of staticaly declared types.
 *
 * \tparam t0_t, t1_t, ... t9_t - component types.
 */
template<typename t0_t, typename t1_t = trait::no_type,
	typename t2_t = trait::no_type, typename t3_t = trait::no_type,
	typename t4_t = trait::no_type, typename t5_t = trait::no_type,
	typename t6_t = trait::no_type, typename t7_t = trait::no_type,
	typename t8_t = trait::no_type, typename t9_t = trait::no_type,
	typename t10_t = trait::no_type, typename t11_t = trait::no_type,
	typename t12_t = trait::no_type, typename t13_t = trait::no_type,
	typename t14_t = trait::no_type, typename t15_t = trait::no_type,
	typename t16_t = trait::no_type, typename t17_t = trait::no_type,
	typename t18_t = trait::no_type, typename t19_t = trait::no_type,
	typename t20_t = trait::no_type>
class HVariant
	{
public:
	typedef HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t> this_type;
	static int const SIZE = yaal::meta::max<
		sizeof ( t0_t ),
		sizeof ( t1_t ),
		sizeof ( t2_t ),
		sizeof ( t3_t ),
		sizeof ( t4_t ),
		sizeof ( t5_t ),
		sizeof ( t6_t ),
		sizeof ( t7_t ),
		sizeof ( t8_t ),
		sizeof ( t9_t ),
		sizeof ( t10_t ),
		sizeof ( t11_t ),
		sizeof ( t12_t ),
		sizeof ( t13_t ),
		sizeof ( t14_t ),
		sizeof ( t15_t ),
		sizeof ( t16_t ),
		sizeof ( t17_t ),
		sizeof ( t18_t ),
		sizeof ( t19_t ),
		sizeof ( t20_t )>::value;
	static int const INVALID;
private:
	char _mem[ SIZE ];
	int _type;
public:
	HVariant( void );
	HVariant( HVariant const& );
	~HVariant( void );
	template<typename tType>
	HVariant( tType const& );
	HVariant& operator = ( HVariant const& );
	void swap( HVariant& );
	template<typename tType>
	tType& get( void );
	template<typename tType>
	tType const& get( void ) const;
	int type( void ) const;
private:
	static void swap( int, void*, void* );
	static void default_construct( int, void* );
	static void destroy( int, void* );
	};

template<typename t0_t, typename t1_t, typename t2_t, typename t3_t,
	typename t4_t, typename t5_t, typename t6_t, typename t7_t,
	typename t8_t, typename t9_t, typename t10_t, typename t11_t,
	typename t12_t, typename t13_t, typename t14_t, typename t15_t,
	typename t16_t, typename t17_t, typename t18_t, typename t19_t,
	typename t20_t>
int const HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::INVALID = -1;

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::HVariant( void )
	: _mem(), _type( INVALID )
	{
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::HVariant( HVariant const& v )
	: _mem(), _type( INVALID )
	{
	switch ( v._type )
		{
		case ( -1 ): break;
		case ( 0 ): new ( _mem ) t0_t( *reinterpret_cast<t0_t const*>( v._mem ) ); break;
		case ( 1 ): new ( _mem ) t1_t( *reinterpret_cast<t1_t const*>( v._mem ) ); break;
		case ( 2 ): new ( _mem ) t2_t( *reinterpret_cast<t2_t const*>( v._mem ) ); break;
		case ( 3 ): new ( _mem ) t3_t( *reinterpret_cast<t3_t const*>( v._mem ) ); break;
		case ( 4 ): new ( _mem ) t4_t( *reinterpret_cast<t4_t const*>( v._mem ) ); break;
		case ( 5 ): new ( _mem ) t5_t( *reinterpret_cast<t5_t const*>( v._mem ) ); break;
		case ( 6 ): new ( _mem ) t6_t( *reinterpret_cast<t6_t const*>( v._mem ) ); break;
		case ( 7 ): new ( _mem ) t7_t( *reinterpret_cast<t7_t const*>( v._mem ) ); break;
		case ( 8 ): new ( _mem ) t8_t( *reinterpret_cast<t8_t const*>( v._mem ) ); break;
		case ( 9 ): new ( _mem ) t9_t( *reinterpret_cast<t9_t const*>( v._mem ) ); break;
		case ( 10 ): new ( _mem ) t10_t( *reinterpret_cast<t10_t const*>( v._mem ) ); break;
		case ( 11 ): new ( _mem ) t11_t( *reinterpret_cast<t11_t const*>( v._mem ) ); break;
		case ( 12 ): new ( _mem ) t12_t( *reinterpret_cast<t12_t const*>( v._mem ) ); break;
		case ( 13 ): new ( _mem ) t13_t( *reinterpret_cast<t13_t const*>( v._mem ) ); break;
		case ( 14 ): new ( _mem ) t14_t( *reinterpret_cast<t14_t const*>( v._mem ) ); break;
		case ( 15 ): new ( _mem ) t15_t( *reinterpret_cast<t15_t const*>( v._mem ) ); break;
		case ( 16 ): new ( _mem ) t16_t( *reinterpret_cast<t16_t const*>( v._mem ) ); break;
		case ( 17 ): new ( _mem ) t17_t( *reinterpret_cast<t17_t const*>( v._mem ) ); break;
		case ( 18 ): new ( _mem ) t18_t( *reinterpret_cast<t18_t const*>( v._mem ) ); break;
		case ( 19 ): new ( _mem ) t19_t( *reinterpret_cast<t19_t const*>( v._mem ) ); break;
		case ( 20 ): new ( _mem ) t20_t( *reinterpret_cast<t20_t const*>( v._mem ) ); break;
		default: M_ASSERT( ! "Absurd type number." ); break;
		}
	_type = v._type;
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::~HVariant( void )
	{
	M_PROLOG
	destroy( _type, _mem );
	M_DESTRUCTOR_EPILOG
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
template<typename tType>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::HVariant( tType const& obj )
	: _mem(), _type( INVALID )
	{
	int t( yaal::trait::find_type<tType, t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::value );
	M_ASSERT( ( t >= 0 ) && ( t <= 20 ) );
	new ( _mem ) tType( obj );
	_type = t;
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
void HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::default_construct( int type_, void* mem_ )
	{
	switch ( type_ )
		{
		case ( -1 ): break;
		case ( 0 ): new ( mem_ ) t0_t(); break;
		case ( 1 ): new ( mem_ ) t1_t(); break;
		case ( 2 ): new ( mem_ ) t2_t(); break;
		case ( 3 ): new ( mem_ ) t3_t(); break;
		case ( 4 ): new ( mem_ ) t4_t(); break;
		case ( 5 ): new ( mem_ ) t5_t(); break;
		case ( 6 ): new ( mem_ ) t6_t(); break;
		case ( 7 ): new ( mem_ ) t7_t(); break;
		case ( 8 ): new ( mem_ ) t8_t(); break;
		case ( 9 ): new ( mem_ ) t9_t(); break;
		case ( 10 ): new ( mem_ ) t10_t(); break;
		case ( 11 ): new ( mem_ ) t11_t(); break;
		case ( 12 ): new ( mem_ ) t12_t(); break;
		case ( 13 ): new ( mem_ ) t13_t(); break;
		case ( 14 ): new ( mem_ ) t14_t(); break;
		case ( 15 ): new ( mem_ ) t15_t(); break;
		case ( 16 ): new ( mem_ ) t16_t(); break;
		case ( 17 ): new ( mem_ ) t17_t(); break;
		case ( 18 ): new ( mem_ ) t18_t(); break;
		case ( 19 ): new ( mem_ ) t19_t(); break;
		case ( 20 ): new ( mem_ ) t20_t(); break;
		default: M_ASSERT( ! "Absurd type number." ); break;
		}
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
void HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::destroy( int type_, void* mem_ )
	{
	try
		{
		switch ( type_ )
			{
			case ( -1 ): break;
			case ( 0 ): reinterpret_cast<t0_t*>( mem_ )->~t0_t(); break;
			case ( 1 ): reinterpret_cast<t1_t*>( mem_ )->~t1_t(); break;
			case ( 2 ): reinterpret_cast<t2_t*>( mem_ )->~t2_t(); break;
			case ( 3 ): reinterpret_cast<t3_t*>( mem_ )->~t3_t(); break;
			case ( 4 ): reinterpret_cast<t4_t*>( mem_ )->~t4_t(); break;
			case ( 5 ): reinterpret_cast<t5_t*>( mem_ )->~t5_t(); break;
			case ( 6 ): reinterpret_cast<t6_t*>( mem_ )->~t6_t(); break;
			case ( 7 ): reinterpret_cast<t7_t*>( mem_ )->~t7_t(); break;
			case ( 8 ): reinterpret_cast<t8_t*>( mem_ )->~t8_t(); break;
			case ( 9 ): reinterpret_cast<t9_t*>( mem_ )->~t9_t(); break;
			case ( 10 ): reinterpret_cast<t10_t*>( mem_ )->~t10_t(); break;
			case ( 11 ): reinterpret_cast<t11_t*>( mem_ )->~t11_t(); break;
			case ( 12 ): reinterpret_cast<t12_t*>( mem_ )->~t12_t(); break;
			case ( 13 ): reinterpret_cast<t13_t*>( mem_ )->~t13_t(); break;
			case ( 14 ): reinterpret_cast<t14_t*>( mem_ )->~t14_t(); break;
			case ( 15 ): reinterpret_cast<t15_t*>( mem_ )->~t15_t(); break;
			case ( 16 ): reinterpret_cast<t16_t*>( mem_ )->~t16_t(); break;
			case ( 17 ): reinterpret_cast<t17_t*>( mem_ )->~t17_t(); break;
			case ( 18 ): reinterpret_cast<t18_t*>( mem_ )->~t18_t(); break;
			case ( 19 ): reinterpret_cast<t19_t*>( mem_ )->~t19_t(); break;
			case ( 20 ): reinterpret_cast<t20_t*>( mem_ )->~t20_t(); break;
			default: M_ASSERT( ! "Absurd type number." ); break;
			}
		}
	catch ( yaal::hcore::HException& e )
		{
		e.log( __FILE__, __PRETTY_FUNCTION__, __LINE__ );
		kill_inferior( ( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `" + type_name<this_type>() + "[" + type_ + "]' throws `" + e.what() + "' in `" + __PRETTY_FUNCTION__ + "'." ).raw() );
		}
	catch ( ... )
		{
		kill_inferior( ( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `" + type_name<this_type>() + "[" + type_ + "]' throws in `" + __PRETTY_FUNCTION__ + "'." ).raw() );
		}
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>&
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::operator = ( HVariant const& v )
	{
	if ( &v != this )
		{
		this_type tmp( v );
		swap( tmp );
		}
	return ( *this );
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
void HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::swap( int type_, void* a_, void* b_ )
	{
	using yaal::swap;
	switch ( type_ )
		{
		case ( -1 ): break;
		case ( 0 ): swap( *reinterpret_cast<t0_t*>( a_ ), *reinterpret_cast<t0_t*>( b_ ) ); break;
		case ( 1 ): swap( *reinterpret_cast<t1_t*>( a_ ), *reinterpret_cast<t1_t*>( b_ ) ); break;
		case ( 2 ): swap( *reinterpret_cast<t2_t*>( a_ ), *reinterpret_cast<t2_t*>( b_ ) ); break;
		case ( 3 ): swap( *reinterpret_cast<t3_t*>( a_ ), *reinterpret_cast<t3_t*>( b_ ) ); break;
		case ( 4 ): swap( *reinterpret_cast<t4_t*>( a_ ), *reinterpret_cast<t4_t*>( b_ ) ); break;
		case ( 5 ): swap( *reinterpret_cast<t5_t*>( a_ ), *reinterpret_cast<t5_t*>( b_ ) ); break;
		case ( 6 ): swap( *reinterpret_cast<t6_t*>( a_ ), *reinterpret_cast<t6_t*>( b_ ) ); break;
		case ( 7 ): swap( *reinterpret_cast<t7_t*>( a_ ), *reinterpret_cast<t7_t*>( b_ ) ); break;
		case ( 8 ): swap( *reinterpret_cast<t8_t*>( a_ ), *reinterpret_cast<t8_t*>( b_ ) ); break;
		case ( 9 ): swap( *reinterpret_cast<t9_t*>( a_ ), *reinterpret_cast<t9_t*>( b_ ) ); break;
		case ( 10 ): swap( *reinterpret_cast<t10_t*>( a_ ), *reinterpret_cast<t10_t*>( b_ ) ); break;
		case ( 11 ): swap( *reinterpret_cast<t11_t*>( a_ ), *reinterpret_cast<t11_t*>( b_ ) ); break;
		case ( 12 ): swap( *reinterpret_cast<t12_t*>( a_ ), *reinterpret_cast<t12_t*>( b_ ) ); break;
		case ( 13 ): swap( *reinterpret_cast<t13_t*>( a_ ), *reinterpret_cast<t13_t*>( b_ ) ); break;
		case ( 14 ): swap( *reinterpret_cast<t14_t*>( a_ ), *reinterpret_cast<t14_t*>( b_ ) ); break;
		case ( 15 ): swap( *reinterpret_cast<t15_t*>( a_ ), *reinterpret_cast<t15_t*>( b_ ) ); break;
		case ( 16 ): swap( *reinterpret_cast<t16_t*>( a_ ), *reinterpret_cast<t16_t*>( b_ ) ); break;
		case ( 17 ): swap( *reinterpret_cast<t17_t*>( a_ ), *reinterpret_cast<t17_t*>( b_ ) ); break;
		case ( 18 ): swap( *reinterpret_cast<t18_t*>( a_ ), *reinterpret_cast<t18_t*>( b_ ) ); break;
		case ( 19 ): swap( *reinterpret_cast<t19_t*>( a_ ), *reinterpret_cast<t19_t*>( b_ ) ); break;
		case ( 20 ): swap( *reinterpret_cast<t20_t*>( a_ ), *reinterpret_cast<t20_t*>( b_ ) ); break;
		default: M_ASSERT( ! "Absurd type number." ); break;
		}
	
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
void HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::swap( HVariant& v )
	{
	if ( &v != this )
		{
		if ( _type == v._type )
			swap( _type, _mem, v._mem );
		else if ( ( _type != INVALID ) && ( v._type != INVALID ) )
			{
			char tmp[SIZE];
			default_construct( _type, tmp );
			swap( _type, _mem, tmp );
			destroy( _type, _mem );
			default_construct( v._type, _mem );
			swap( v._type, _mem, v._mem );
			destroy( v._type, v._mem );
			default_construct( _type, v._mem );
			swap( _type, tmp, v._mem );
			destroy( _type, tmp );
			yaal::swap( _type, v._type );
			}
		else if ( _type != INVALID )
			{
			default_construct( _type, v._mem );
			swap( _type, _mem, v._mem );
			destroy( _type, _mem );
			yaal::swap( _type, v._type );
			}
		else if ( v._type != INVALID )
			{
			default_construct( v._type, _mem );
			swap( v._type, v._mem, _mem );
			destroy( v._type, v._mem );
			yaal::swap( _type, v._type );
			}
		}
	return;
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
template<typename tType>
tType& HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::get( void )
	{
	M_ASSERT(( _type == yaal::trait::find_type<tType, t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::value ));
	return ( *reinterpret_cast<tType*>( _mem ) );
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
template<typename tType>
tType const& HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::get( void ) const
	{
	M_ASSERT(( _type == yaal::trait::find_type<tType, t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::value ));
	return ( *reinterpret_cast<tType const*>( _mem ) );
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t,
	typename t10_t, typename t11_t,
	typename t12_t, typename t13_t,
	typename t14_t, typename t15_t,
	typename t16_t, typename t17_t,
	typename t18_t, typename t19_t,
	typename t20_t>
int HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>::type( void ) const
	{
	return ( _type );
	}

}

}

#endif /* #ifndef YAAL_HCORE_HVARIANT_HXX_INCLUDED */

