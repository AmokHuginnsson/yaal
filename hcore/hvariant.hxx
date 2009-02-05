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
#define YAAL_HCORE_HVARIANT_HXX_INCLUDED

#include <new> /* holly shit */

namespace yaal
{

namespace hcore
{

/*! \brief Dummy type used as a type stub in default template type argument declaration.
 */
struct HNoType {};

/*! \brief Multi type container.
 *
 * A variant type without non-empty guarantee.
 * Represent variable that can be used as one of staticaly declared types.
 *
 * \tparam t0_t, t1_t, ... t9_t - component types.
 */
template<typename t0_t, typename t1_t = HNoType,
	typename t2_t = HNoType, typename t3_t = HNoType,
	typename t4_t = HNoType, typename t5_t = HNoType,
	typename t6_t = HNoType, typename t7_t = HNoType,
	typename t8_t = HNoType, typename t9_t = HNoType>
class HVariant
	{
protected:
	typedef HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t> self_t;
	static int const D_SIZE = yaal::static_max<
		sizeof ( t0_t ),
		sizeof ( t1_t ),
		sizeof ( t2_t ),
		sizeof ( t3_t ),
		sizeof ( t4_t ),
		sizeof ( t5_t ),
		sizeof ( t6_t ),
		sizeof ( t7_t ),
		sizeof ( t8_t ),
		sizeof ( t9_t )>::value;
	char _mem[ D_SIZE ];
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
	int type( void ) const;
	};

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::HVariant( void )
	: _mem(), _type( -1 )
	{
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::HVariant( HVariant const& v )
	: _mem(), _type( -1 )
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
		default: M_ASSERT( ! "Absurd type number." ); break;
		}
	_type = v._type;
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::~HVariant( void )
	{
	switch ( _type )
		{
		case ( -1 ): break;
		case ( 0 ): reinterpret_cast<t0_t*>( _mem )->~t0_t(); break;
		case ( 1 ): reinterpret_cast<t1_t*>( _mem )->~t1_t(); break;
		case ( 2 ): reinterpret_cast<t2_t*>( _mem )->~t2_t(); break;
		case ( 3 ): reinterpret_cast<t3_t*>( _mem )->~t3_t(); break;
		case ( 4 ): reinterpret_cast<t4_t*>( _mem )->~t4_t(); break;
		case ( 5 ): reinterpret_cast<t5_t*>( _mem )->~t5_t(); break;
		case ( 6 ): reinterpret_cast<t6_t*>( _mem )->~t6_t(); break;
		case ( 7 ): reinterpret_cast<t7_t*>( _mem )->~t7_t(); break;
		case ( 8 ): reinterpret_cast<t8_t*>( _mem )->~t8_t(); break;
		case ( 9 ): reinterpret_cast<t9_t*>( _mem )->~t9_t(); break;
		default: M_ASSERT( ! "Absurd type number." ); break;
		}
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
template<typename tType>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::HVariant( tType const& obj )
	: _mem(), _type( -1 )
	{
	new ( _mem ) tType( obj );
	_type = yaal::find_type<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, tType>::value;
	M_ASSERT( ( _type >= 0 ) && ( _type <= 9 ) );
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>&
HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::operator = ( HVariant const& v )
	{
	if ( &v != this )
		{
		self_t tmp( v );
		swap( tmp );
		}
	return ( *this );
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
void HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::swap( HVariant& v )
	{
	if ( &v != this )
		{
		using yaal::swap;
		swap( _type, v._type );
		yaal::swap_ranges( _mem, _mem + D_SIZE, v._mem );
		}
	return;
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
template<typename tType>
tType& HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::get( void )
	{
	M_ASSERT(( _type == yaal::find_type<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, tType>::value ));
	return ( *reinterpret_cast<tType*>( _mem ) );
	}

template<typename t0_t, typename t1_t,
	typename t2_t, typename t3_t,
	typename t4_t, typename t5_t,
	typename t6_t, typename t7_t,
	typename t8_t, typename t9_t>
int HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t>::type( void ) const
	{
	return ( _type );
	}

}

}

#endif /* not YAAL_HCORE_HVARIANT_HXX_INCLUDED */

