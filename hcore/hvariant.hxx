/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
 * Represent variable that can be used as one of statically declared types.
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
		static_cast<int>( sizeof ( t0_t ) ),
		static_cast<int>( sizeof ( t1_t ) ),
		static_cast<int>( sizeof ( t2_t ) ),
		static_cast<int>( sizeof ( t3_t ) ),
		static_cast<int>( sizeof ( t4_t ) ),
		static_cast<int>( sizeof ( t5_t ) ),
		static_cast<int>( sizeof ( t6_t ) ),
		static_cast<int>( sizeof ( t7_t ) ),
		static_cast<int>( sizeof ( t8_t ) ),
		static_cast<int>( sizeof ( t9_t ) ),
		static_cast<int>( sizeof ( t10_t ) ),
		static_cast<int>( sizeof ( t11_t ) ),
		static_cast<int>( sizeof ( t12_t ) ),
		static_cast<int>( sizeof ( t13_t ) ),
		static_cast<int>( sizeof ( t14_t ) ),
		static_cast<int>( sizeof ( t15_t ) ),
		static_cast<int>( sizeof ( t16_t ) ),
		static_cast<int>( sizeof ( t17_t ) ),
		static_cast<int>( sizeof ( t18_t ) ),
		static_cast<int>( sizeof ( t19_t ) ),
		static_cast<int>( sizeof ( t20_t ) )>::value;
	static int const INVALID = -1;
private:
	typedef typename memory::aligned<SIZE>::type aligner_t;
	aligner_t _mem;
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
			case ( INVALID ): break;
			case ( 0 ): { t0_t const* v( reinterpret_cast<t0_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t0_t( *v ); } break;
			case ( 1 ): { t1_t const* v( reinterpret_cast<t1_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t1_t( *v ); } break;
			case ( 2 ): { t2_t const* v( reinterpret_cast<t2_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t2_t( *v ); } break;
			case ( 3 ): { t3_t const* v( reinterpret_cast<t3_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t3_t( *v ); } break;
			case ( 4 ): { t4_t const* v( reinterpret_cast<t4_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t4_t( *v ); } break;
			case ( 5 ): { t5_t const* v( reinterpret_cast<t5_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t5_t( *v ); } break;
			case ( 6 ): { t6_t const* v( reinterpret_cast<t6_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t6_t( *v ); } break;
			case ( 7 ): { t7_t const* v( reinterpret_cast<t7_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t7_t( *v ); } break;
			case ( 8 ): { t8_t const* v( reinterpret_cast<t8_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t8_t( *v ); } break;
			case ( 9 ): { t9_t const* v( reinterpret_cast<t9_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t9_t( *v ); } break;
			case ( 10 ): { t10_t const* v( reinterpret_cast<t10_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t10_t( *v ); } break;
			case ( 11 ): { t11_t const* v( reinterpret_cast<t11_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t11_t( *v ); } break;
			case ( 12 ): { t12_t const* v( reinterpret_cast<t12_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t12_t( *v ); } break;
			case ( 13 ): { t13_t const* v( reinterpret_cast<t13_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t13_t( *v ); } break;
			case ( 14 ): { t14_t const* v( reinterpret_cast<t14_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t14_t( *v ); } break;
			case ( 15 ): { t15_t const* v( reinterpret_cast<t15_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t15_t( *v ); } break;
			case ( 16 ): { t16_t const* v( reinterpret_cast<t16_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t16_t( *v ); } break;
			case ( 17 ): { t17_t const* v( reinterpret_cast<t17_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t17_t( *v ); } break;
			case ( 18 ): { t18_t const* v( reinterpret_cast<t18_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t18_t( *v ); } break;
			case ( 19 ): { t19_t const* v( reinterpret_cast<t19_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t19_t( *v ); } break;
			case ( 20 ): { t20_t const* v( reinterpret_cast<t20_t const*>( v_._mem.mem() ) ); new ( _mem.mem() ) t20_t( *v ); } break;
			default: M_ASSERT( 0 && "Absurd type number." ); break;
		}
		_type = v_._type;
	}
	HVariant( HVariant&& v_ ) noexcept
		: _mem()
		, _type( INVALID ) {
		swap( v_ );
	}
	~HVariant( void ) {
		M_PROLOG
		reset();
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
		new ( _mem.mem() ) tType( obj );
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
		new ( _mem.mem() ) real_t( yaal::forward<real_t>( obj ) );
		_type = t;
	}
	HVariant& operator = ( HVariant const& v_ ) {
		if ( &v_ != this ) {
			this_type tmp( v_ );
			swap( tmp );
		}
		return ( *this );
	}
	HVariant& operator = ( HVariant&& v_ ) noexcept {
		if ( &v_ != this ) {
			swap( v_ );
			destroy( v_._type, v_._mem.mem() );
			v_._type = INVALID;
		}
		return ( *this );
	}
	void reset( void ) {
		destroy( _type, _mem.mem() );
		_type = INVALID;
	}
	void swap( HVariant& v_ ) noexcept {
		if ( &v_ != this ) {
			if ( _type == v_._type ) {
				swap( _type, _mem.mem(), v_._mem.mem() );
			} else if ( ( _type != INVALID ) && ( v_._type != INVALID ) ) {
				aligner_t tmp{};
				move( _type, tmp.mem(), _mem.mem() );
				move( v_._type, _mem.mem(), v_._mem.mem() );
				move( _type, v_._mem.mem(), tmp.mem() );
			} else if ( _type != INVALID ) {
				move( _type, v_._mem.mem(), _mem.mem() );
			} else if ( v_._type != INVALID ) {
				move( v_._type, _mem.mem(), v_._mem.mem() );
			}
			yaal::swap( _type, v_._type );
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
		tType* v( reinterpret_cast<tType*>( _mem.mem() ) );
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
		tType const* v( reinterpret_cast<tType const*>( _mem.mem() ) );
		return ( *v );
	}
	int type( void ) const {
		return ( _type );
	}
	bool operator == ( HVariant const& other_ ) const {
		bool eq( _type == other_._type );
		if ( eq ) {
			switch ( _type ) {
				case ( INVALID ): break;
				case ( 0 ): eq = equals( *reinterpret_cast<t0_t const*>( _mem.mem() ), *reinterpret_cast<t0_t const*>( other_._mem.mem() ) ); break;
				case ( 1 ): eq = equals( *reinterpret_cast<t1_t const*>( _mem.mem() ), *reinterpret_cast<t1_t const*>( other_._mem.mem() ) ); break;
				case ( 2 ): eq = equals( *reinterpret_cast<t2_t const*>( _mem.mem() ), *reinterpret_cast<t2_t const*>( other_._mem.mem() ) ); break;
				case ( 3 ): eq = equals( *reinterpret_cast<t3_t const*>( _mem.mem() ), *reinterpret_cast<t3_t const*>( other_._mem.mem() ) ); break;
				case ( 4 ): eq = equals( *reinterpret_cast<t4_t const*>( _mem.mem() ), *reinterpret_cast<t4_t const*>( other_._mem.mem() ) ); break;
				case ( 5 ): eq = equals( *reinterpret_cast<t5_t const*>( _mem.mem() ), *reinterpret_cast<t5_t const*>( other_._mem.mem() ) ); break;
				case ( 6 ): eq = equals( *reinterpret_cast<t6_t const*>( _mem.mem() ), *reinterpret_cast<t6_t const*>( other_._mem.mem() ) ); break;
				case ( 7 ): eq = equals( *reinterpret_cast<t7_t const*>( _mem.mem() ), *reinterpret_cast<t7_t const*>( other_._mem.mem() ) ); break;
				case ( 8 ): eq = equals( *reinterpret_cast<t8_t const*>( _mem.mem() ), *reinterpret_cast<t8_t const*>( other_._mem.mem() ) ); break;
				case ( 9 ): eq = equals( *reinterpret_cast<t9_t const*>( _mem.mem() ), *reinterpret_cast<t9_t const*>( other_._mem.mem() ) ); break;
				case ( 10 ): eq = equals( *reinterpret_cast<t10_t const*>( _mem.mem() ), *reinterpret_cast<t10_t const*>( other_._mem.mem() ) ); break;
				case ( 11 ): eq = equals( *reinterpret_cast<t11_t const*>( _mem.mem() ), *reinterpret_cast<t11_t const*>( other_._mem.mem() ) ); break;
				case ( 12 ): eq = equals( *reinterpret_cast<t12_t const*>( _mem.mem() ), *reinterpret_cast<t12_t const*>( other_._mem.mem() ) ); break;
				case ( 13 ): eq = equals( *reinterpret_cast<t13_t const*>( _mem.mem() ), *reinterpret_cast<t13_t const*>( other_._mem.mem() ) ); break;
				case ( 14 ): eq = equals( *reinterpret_cast<t14_t const*>( _mem.mem() ), *reinterpret_cast<t14_t const*>( other_._mem.mem() ) ); break;
				case ( 15 ): eq = equals( *reinterpret_cast<t15_t const*>( _mem.mem() ), *reinterpret_cast<t15_t const*>( other_._mem.mem() ) ); break;
				case ( 16 ): eq = equals( *reinterpret_cast<t16_t const*>( _mem.mem() ), *reinterpret_cast<t16_t const*>( other_._mem.mem() ) ); break;
				case ( 17 ): eq = equals( *reinterpret_cast<t17_t const*>( _mem.mem() ), *reinterpret_cast<t17_t const*>( other_._mem.mem() ) ); break;
				case ( 18 ): eq = equals( *reinterpret_cast<t18_t const*>( _mem.mem() ), *reinterpret_cast<t18_t const*>( other_._mem.mem() ) ); break;
				case ( 19 ): eq = equals( *reinterpret_cast<t19_t const*>( _mem.mem() ), *reinterpret_cast<t19_t const*>( other_._mem.mem() ) ); break;
				case ( 20 ): eq = equals( *reinterpret_cast<t20_t const*>( _mem.mem() ), *reinterpret_cast<t20_t const*>( other_._mem.mem() ) ); break;
				default: M_ASSERT( 0 && "Absurd type number." ); break;
			}
		}
		return ( eq );
	}
	bool operator != ( HVariant const& other_ ) const {
		return ( ! operator == ( other_ ) );
	}
private:
	static void swap( int type_, void* a_, void* b_ ) {
		using yaal::swap;
		switch ( type_ ) {
			case ( INVALID ): break;
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
	static void move( int type_, void* to_, void* from_ ) {
		switch ( type_ ) {
			case ( INVALID ): break;
			case ( 0 ): new ( to_ ) t0_t( yaal::move( *static_cast<t0_t*>( from_ ) ) ); break;
			case ( 1 ): new ( to_ ) t1_t( yaal::move( *static_cast<t1_t*>( from_ ) ) ); break;
			case ( 2 ): new ( to_ ) t2_t( yaal::move( *static_cast<t2_t*>( from_ ) ) ); break;
			case ( 3 ): new ( to_ ) t3_t( yaal::move( *static_cast<t3_t*>( from_ ) ) ); break;
			case ( 4 ): new ( to_ ) t4_t( yaal::move( *static_cast<t4_t*>( from_ ) ) ); break;
			case ( 5 ): new ( to_ ) t5_t( yaal::move( *static_cast<t5_t*>( from_ ) ) ); break;
			case ( 6 ): new ( to_ ) t6_t( yaal::move( *static_cast<t6_t*>( from_ ) ) ); break;
			case ( 7 ): new ( to_ ) t7_t( yaal::move( *static_cast<t7_t*>( from_ ) ) ); break;
			case ( 8 ): new ( to_ ) t8_t( yaal::move( *static_cast<t8_t*>( from_ ) ) ); break;
			case ( 9 ): new ( to_ ) t9_t( yaal::move( *static_cast<t9_t*>( from_ ) ) ); break;
			case ( 10 ): new ( to_ ) t10_t( yaal::move( *static_cast<t10_t*>( from_ ) ) ); break;
			case ( 11 ): new ( to_ ) t11_t( yaal::move( *static_cast<t11_t*>( from_ ) ) ); break;
			case ( 12 ): new ( to_ ) t12_t( yaal::move( *static_cast<t12_t*>( from_ ) ) ); break;
			case ( 13 ): new ( to_ ) t13_t( yaal::move( *static_cast<t13_t*>( from_ ) ) ); break;
			case ( 14 ): new ( to_ ) t14_t( yaal::move( *static_cast<t14_t*>( from_ ) ) ); break;
			case ( 15 ): new ( to_ ) t15_t( yaal::move( *static_cast<t15_t*>( from_ ) ) ); break;
			case ( 16 ): new ( to_ ) t16_t( yaal::move( *static_cast<t16_t*>( from_ ) ) ); break;
			case ( 17 ): new ( to_ ) t17_t( yaal::move( *static_cast<t17_t*>( from_ ) ) ); break;
			case ( 18 ): new ( to_ ) t18_t( yaal::move( *static_cast<t18_t*>( from_ ) ) ); break;
			case ( 19 ): new ( to_ ) t19_t( yaal::move( *static_cast<t19_t*>( from_ ) ) ); break;
			case ( 20 ): new ( to_ ) t20_t( yaal::move( *static_cast<t20_t*>( from_ ) ) ); break;
			default: M_ASSERT( 0 && "Absurd type number." ); break;
		}
		destroy( type_, from_ );
	}
	static void destroy( int type_, void* mem_ ) {
		try {
			switch ( type_ ) {
				case ( INVALID ): break;
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
					.append( "'." )
			);
		} catch ( ... ) {
			kill_interior(
				__FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `"_ys
					.append( yaal::hcore::type_name<this_type>() )
					.append( "[" )
					.append( type_ )
					.append( "]' throws in `" )
					.append( __PRETTY_FUNCTION__ )
					.append("'." )
			);
		}
	}
	template<typename T>
	static bool equals( T const& a_, T const& b_ ) {
		return ( a_ == b_ );
	}
	static bool equals( trait::no_type const&, trait::no_type const& ) {
		return ( false );
	}
};

template<
	typename t0_t, typename t1_t, typename t2_t, typename t3_t,
	typename t4_t, typename t5_t, typename t6_t, typename t7_t,
	typename t8_t, typename t9_t, typename t10_t, typename t11_t,
	typename t12_t, typename t13_t, typename t14_t, typename t15_t,
	typename t16_t, typename t17_t, typename t18_t, typename t19_t,
	typename t20_t
>
inline void swap(
	yaal::hcore::HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>& a_,
	yaal::hcore::HVariant<t0_t, t1_t, t2_t, t3_t, t4_t, t5_t, t6_t, t7_t, t8_t, t9_t, t10_t, t11_t, t12_t, t13_t, t14_t, t15_t, t16_t, t17_t, t18_t, t19_t, t20_t>& b_
) {
	a_.swap( b_ );
}

}

}

#endif /* #ifndef YAAL_HCORE_HVARIANT_HXX_INCLUDED */

