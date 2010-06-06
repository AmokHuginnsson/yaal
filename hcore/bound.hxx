/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	bound.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/bound.hxx
 * \brief HCall helpers for algorithms.
 */

#ifndef YAAL_HCORE_BOUND_HXX_INCLUDED
#define YAAL_HCORE_BOUND_HXX_INCLUDED

#include "hcore/hcall.hxx"

namespace yaal
{

namespace hcore
{

template<typename getter1_t, typename getter2_t>
class bound_less : public binary_function<bool, typename getter1_t::argument_type, typename getter2_t::argument_type>
	{
public:
private:
	getter1_t _getter1;
	getter2_t _getter2;
public:
	bound_less( getter1_t const& getter1_, getter2_t const& getter2_ ) : _getter1( getter1_ ), _getter2( getter2_ ) {}
	template<typename T1, typename T2>
	bool operator()( T1 const& left_, T2 const& right_ ) const
		{
		return ( _getter1( left_ ) < _getter2( right_ ) );
		}
	};

template<typename getter1_t, typename getter2_t>
class bound_greater : public binary_function<bool, typename getter1_t::argument_type, typename getter2_t::argument_type>
	{
public:
private:
	getter1_t _getter1;
	getter2_t _getter2;
public:
	bound_greater( getter1_t const& getter1_, getter2_t const& getter2_ ) : _getter1( getter1_ ), _getter2( getter2_ ) {}
	template<typename T1, typename T2>
	bool operator()( T1 const& left_, T2 const& right_ ) const
		{
		return ( _getter1( left_ ) > _getter2( right_ ) );
		}
	};

template<typename getter1_t, typename getter2_t>
class bound_less_equal : public binary_function<bool, typename getter1_t::argument_type, typename getter2_t::argument_type>
	{
public:
private:
	getter1_t _getter1;
	getter2_t _getter2;
public:
	bound_less_equal( getter1_t const& getter1_, getter2_t const& getter2_ ) : _getter1( getter1_ ), _getter2( getter2_ ) {}
	template<typename T1, typename T2>
	bool operator()( T1 const& left_, T2 const& right_ ) const
		{
		return ( _getter1( left_ ) <= _getter2( right_ ) );
		}
	};

template<typename getter1_t, typename getter2_t>
class bound_greater_equal : public binary_function<bool, typename getter1_t::argument_type, typename getter2_t::argument_type>
	{
public:
private:
	getter1_t _getter1;
	getter2_t _getter2;
public:
	bound_greater_equal( getter1_t const& getter1_, getter2_t const& getter2_ ) : _getter1( getter1_ ), _getter2( getter2_ ) {}
	template<typename T1, typename T2>
	bool operator()( T1 const& left_, T2 const& right_ ) const
		{
		return ( _getter1( left_ ) >= _getter2( right_ ) );
		}
	};

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
bound_less<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >
					operator < (  HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
							HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( bound_less<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >( left_, right_ ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
bound_greater<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >
					operator > (  HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
							HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( bound_greater<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >( left_, right_ ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
bound_less_equal<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >
					operator <= (  HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
							HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( bound_less_equal<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >( left_, right_ ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
bound_greater_equal<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >
					operator >= (  HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
							HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( bound_greater_equal<HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >( left_, right_ ) );
	}

}

}

#endif /* not YAAL_HCORE_BOUND_HXX_INCLUDED */

