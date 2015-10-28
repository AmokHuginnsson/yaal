/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hvariant.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hvariant.hxx
 * \brief Declaration and implementation of HVariant<> class template.
 */

#ifndef YAAL_HCORE_HVARIANT_HXX_INCLUDED
#define YAAL_HCORE_HVARIANT_HXX_INCLUDED 1

#include <new>

#include "hcore/algorithm.hxx"

namespace yaal {

namespace hcore {

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
class HVariant final {
public:
	typedef HVariant<
		t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t,
		t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t,
		t16_t, t17_t, t18_t, t19_t, t20_t
	> this_type;
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
	static int const INVALID = -1;
private:
	typedef typename yaal::trait::ternary<( SIZE > ALIGNOF_INT_LONG ), int long long,
					typename yaal::trait::ternary<( SIZE > ALIGNOF_INT ), int long,
					typename yaal::trait::ternary<( SIZE > ALIGNOF_INT_SHORT ), int,
					typename yaal::trait::ternary<( SIZE > static_cast<int>( sizeof ( char ) ) ), int short, char>::type>::type>::type>::type aligner_type;
	static int const ALIGNER_COUNT = SIZE / static_cast<int>( sizeof ( aligner_type ) ) + ( SIZE % static_cast<int>( sizeof ( aligner_type ) ) ? 1 : 0 );

	typedef  aligner_type (aligner_t)[ ALIGNER_COUNT ];
	aligner_t __attribute__((__may_alias__)) _mem;
	int _type;
public:
	HVariant( void )
		: _mem()
		, _type( INVALID ) {
	}
	HVariant( HVariant const& v_ )
		: _mem()
		, _type( INVALID ) {
		switch ( v_._type ) {
			case ( -1 ): break;
			case ( 0 ): { t0_t const* v( reinterpret_cast<t0_t const*>( v_._mem ) ); new ( _mem ) t0_t( *v ); } break;
			case ( 1 ): { t1_t const* v( reinterpret_cast<t1_t const*>( v_._mem ) ); new ( _mem ) t1_t( *v ); } break;
			case ( 2 ): { t2_t const* v( reinterpret_cast<t2_t const*>( v_._mem ) ); new ( _mem ) t2_t( *v ); } break;
			case ( 3 ): { t3_t const* v( reinterpret_cast<t3_t const*>( v_._mem ) ); new ( _mem ) t3_t( *v ); } break;
			case ( 4 ): { t4_t const* v( reinterpret_cast<t4_t const*>( v_._mem ) ); new ( _mem ) t4_t( *v ); } break;
			case ( 5 ): { t5_t const* v( reinterpret_cast<t5_t const*>( v_._mem ) ); new ( _mem ) t5_t( *v ); } break;
			case ( 6 ): { t6_t const* v( reinterpret_cast<t6_t const*>( v_._mem ) ); new ( _mem ) t6_t( *v ); } break;
			case ( 7 ): { t7_t const* v( reinterpret_cast<t7_t const*>( v_._mem ) ); new ( _mem ) t7_t( *v ); } break;
			case ( 8 ): { t8_t const* v( reinterpret_cast<t8_t const*>( v_._mem ) ); new ( _mem ) t8_t( *v ); } break;
			case ( 9 ): { t9_t const* v( reinterpret_cast<t9_t const*>( v_._mem ) ); new ( _mem ) t9_t( *v ); } break;
			case ( 10 ): { t10_t const* v( reinterpret_cast<t10_t const*>( v_._mem ) ); new ( _mem ) t10_t( *v ); } break;
			case ( 11 ): { t11_t const* v( reinterpret_cast<t11_t const*>( v_._mem ) ); new ( _mem ) t11_t( *v ); } break;
			case ( 12 ): { t12_t const* v( reinterpret_cast<t12_t const*>( v_._mem ) ); new ( _mem ) t12_t( *v ); } break;
			case ( 13 ): { t13_t const* v( reinterpret_cast<t13_t const*>( v_._mem ) ); new ( _mem ) t13_t( *v ); } break;
			case ( 14 ): { t14_t const* v( reinterpret_cast<t14_t const*>( v_._mem ) ); new ( _mem ) t14_t( *v ); } break;
			case ( 15 ): { t15_t const* v( reinterpret_cast<t15_t const*>( v_._mem ) ); new ( _mem ) t15_t( *v ); } break;
			case ( 16 ): { t16_t const* v( reinterpret_cast<t16_t const*>( v_._mem ) ); new ( _mem ) t16_t( *v ); } break;
			case ( 17 ): { t17_t const* v( reinterpret_cast<t17_t const*>( v_._mem ) ); new ( _mem ) t17_t( *v ); } break;
			case ( 18 ): { t18_t const* v( reinterpret_cast<t18_t const*>( v_._mem ) ); new ( _mem ) t18_t( *v ); } break;
			case ( 19 ): { t19_t const* v( reinterpret_cast<t19_t const*>( v_._mem ) ); new ( _mem ) t19_t( *v ); } break;
			case ( 20 ): { t20_t const* v( reinterpret_cast<t20_t const*>( v_._mem ) ); new ( _mem ) t20_t( *v ); } break;
			default: M_ASSERT( 0 && "Absurd type number." ); break;
		}
		_type = v_._type;
	}
	HVariant( HVariant&& v_ )
		: _mem()
		, _type( INVALID ) {
		switch ( v_._type ) {
			case ( -1 ): break;
			case ( 0 ): { t0_t* v( reinterpret_cast<t0_t*>( v_._mem ) ); new ( _mem ) t0_t( yaal::move( *v ) ); } break;
			case ( 1 ): { t1_t* v( reinterpret_cast<t1_t*>( v_._mem ) ); new ( _mem ) t1_t( yaal::move( *v ) ); } break;
			case ( 2 ): { t2_t* v( reinterpret_cast<t2_t*>( v_._mem ) ); new ( _mem ) t2_t( yaal::move( *v ) ); } break;
			case ( 3 ): { t3_t* v( reinterpret_cast<t3_t*>( v_._mem ) ); new ( _mem ) t3_t( yaal::move( *v ) ); } break;
			case ( 4 ): { t4_t* v( reinterpret_cast<t4_t*>( v_._mem ) ); new ( _mem ) t4_t( yaal::move( *v ) ); } break;
			case ( 5 ): { t5_t* v( reinterpret_cast<t5_t*>( v_._mem ) ); new ( _mem ) t5_t( yaal::move( *v ) ); } break;
			case ( 6 ): { t6_t* v( reinterpret_cast<t6_t*>( v_._mem ) ); new ( _mem ) t6_t( yaal::move( *v ) ); } break;
			case ( 7 ): { t7_t* v( reinterpret_cast<t7_t*>( v_._mem ) ); new ( _mem ) t7_t( yaal::move( *v ) ); } break;
			case ( 8 ): { t8_t* v( reinterpret_cast<t8_t*>( v_._mem ) ); new ( _mem ) t8_t( yaal::move( *v ) ); } break;
			case ( 9 ): { t9_t* v( reinterpret_cast<t9_t*>( v_._mem ) ); new ( _mem ) t9_t( yaal::move( *v ) ); } break;
			case ( 10 ): { t10_t* v( reinterpret_cast<t10_t*>( v_._mem ) ); new ( _mem ) t10_t( yaal::move( *v ) ); } break;
			case ( 11 ): { t11_t* v( reinterpret_cast<t11_t*>( v_._mem ) ); new ( _mem ) t11_t( yaal::move( *v ) ); } break;
			case ( 12 ): { t12_t* v( reinterpret_cast<t12_t*>( v_._mem ) ); new ( _mem ) t12_t( yaal::move( *v ) ); } break;
			case ( 13 ): { t13_t* v( reinterpret_cast<t13_t*>( v_._mem ) ); new ( _mem ) t13_t( yaal::move( *v ) ); } break;
			case ( 14 ): { t14_t* v( reinterpret_cast<t14_t*>( v_._mem ) ); new ( _mem ) t14_t( yaal::move( *v ) ); } break;
			case ( 15 ): { t15_t* v( reinterpret_cast<t15_t*>( v_._mem ) ); new ( _mem ) t15_t( yaal::move( *v ) ); } break;
			case ( 16 ): { t16_t* v( reinterpret_cast<t16_t*>( v_._mem ) ); new ( _mem ) t16_t( yaal::move( *v ) ); } break;
			case ( 17 ): { t17_t* v( reinterpret_cast<t17_t*>( v_._mem ) ); new ( _mem ) t17_t( yaal::move( *v ) ); } break;
			case ( 18 ): { t18_t* v( reinterpret_cast<t18_t*>( v_._mem ) ); new ( _mem ) t18_t( yaal::move( *v ) ); } break;
			case ( 19 ): { t19_t* v( reinterpret_cast<t19_t*>( v_._mem ) ); new ( _mem ) t19_t( yaal::move( *v ) ); } break;
			case ( 20 ): { t20_t* v( reinterpret_cast<t20_t*>( v_._mem ) ); new ( _mem ) t20_t( yaal::move( *v ) ); } break;
			default: M_ASSERT( 0 && "Absurd type number." ); break;
		}
		_type = v_._type;
		destroy( v_._type, v_._mem );
	}
	~HVariant( void ) {
		M_PROLOG
		destroy( _type, _mem );
		M_DESTRUCTOR_EPILOG
	}
	template<typename tType>
	explicit HVariant( tType const& obj )
		: _mem()
		, _type( INVALID ) {
		constexpr int const t(
			yaal::trait::find_type<
				tType, t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t,
				t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t,
				t16_t, t17_t, t18_t, t19_t, t20_t
			>::value
		);
		static_assert( ( t >= 0 ) && ( t <= 20 ), "type not declared in variant" );
		new ( _mem ) tType( obj );
		_type = t;
	}
	template<typename tType>
	explicit HVariant( tType&& obj )
		: _mem()
		, _type( INVALID ) {
		typedef typename trait::strip_reference<tType>::type real_t;
		constexpr int const t(
			yaal::trait::find_type<
				real_t,
				t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t,
				t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t,
				t16_t, t17_t, t18_t, t19_t, t20_t
			>::value
		);
		static_assert( ( t >= 0 ) && ( t <= 20 ), "type not declared in variant" );
		new ( _mem ) real_t( yaal::forward<real_t>( obj ) );
		_type = t;
	}
	HVariant& operator = ( HVariant const& v_ ) {
		if ( &v_ != this ) {
			this_type tmp( v_ );
			swap( tmp );
		}
		return ( *this );
	}
	HVariant& operator = ( HVariant&& v_ ) {
		if ( &v_ != this ) {
			swap( v_ );
			destroy( v_._type, v_._mem );
		}
		return ( *this );
	}
	void swap( HVariant& v_ ) {
		if ( &v_ != this ) {
			if ( _type == v_._type ) {
				swap( _type, _mem, v_._mem );
			} else if ( ( _type != INVALID ) && ( v_._type != INVALID ) ) {
				aligner_t tmp{};
				default_construct( _type, tmp );
				swap( _type, _mem, tmp );
				destroy( _type, _mem );
				default_construct( v_._type, _mem );
				swap( v_._type, _mem, v_._mem );
				destroy( v_._type, v_._mem );
				default_construct( _type, v_._mem );
				swap( _type, tmp, v_._mem );
				destroy( _type, tmp );
				yaal::swap( _type, v_._type );
			} else if ( _type != INVALID ) {
				default_construct( _type, v_._mem );
				swap( _type, _mem, v_._mem );
				destroy( _type, _mem );
				yaal::swap( _type, v_._type );
			} else if ( v_._type != INVALID ) {
				default_construct( v_._type, _mem );
				swap( v_._type, v_._mem, _mem );
				destroy( v_._type, v_._mem );
				yaal::swap( _type, v_._type );
			}
		}
		return;
	}
	template<typename tType>
	tType& get( void ) {
		M_ASSERT( _type != INVALID );
		M_ASSERT((
			_type == yaal::trait::find_type<
				tType,
				t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t,
				t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t
			>::value
		));
		tType* v( reinterpret_cast<tType*>( _mem ) );
		return ( *v );
	}
	template<typename tType>
	tType const& get( void ) const {
		M_ASSERT( _type != INVALID );
		M_ASSERT((
			_type == yaal::trait::find_type<
				tType,
				t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t,
				t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t
			>::value
		));
		tType const* v( reinterpret_cast<tType const*>( _mem ) );
		return ( *v );
	}
	int type( void ) const {
		return ( _type );
	}
private:
	static void swap( int type_, void* a_, void* b_ ) {
		using yaal::swap;
		switch ( type_ ) {
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
			default: M_ASSERT( 0 && "Absurd type number." ); break;
		}
		return;
	}
	static void default_construct( int type_, void* mem_ ) {
		switch ( type_ ) {
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
			default: M_ASSERT( 0 && "Absurd type number." ); break;
		}
	}
	static void destroy( int type_, void* mem_ ) {
		try {
			switch ( type_ ) {
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
				default: M_ASSERT( 0 && "Absurd type number." ); break;
			}
		} catch ( yaal::hcore::HException& e ) {
			e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ );
			kill_interior(
				__FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `"_ys
					.append( yaal::hcore::type_name<this_type>() )
					.append( "[" )
					.append( type_ )
					.append( "]' throws `" )
					.append( e.what() ).append( "' in `" )
					.append( __PRETTY_FUNCTION__ )
					.append( "'." ).raw()
			);
		} catch ( ... ) {
			kill_interior(
				__FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `"_ys
					.append( yaal::hcore::type_name<this_type>() )
					.append( "[" )
					.append( type_ )
					.append( "]' throws in `" )
					.append( __PRETTY_FUNCTION__ )
					.append("'." ).raw()
			);
		}
	}
};


}

}

#endif /* #ifndef YAAL_HCORE_HVARIANT_HXX_INCLUDED */

