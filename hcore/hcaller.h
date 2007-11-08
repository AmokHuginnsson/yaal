/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcaller.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HCALLER_H
#define __YAAL_HCORE_HCALLER_H

#line 31

#define D_VCSID_YAAL_HCORE_HCALLER_H "$Id$"

#include "hcore/hpointer.h"

namespace yaal
{

namespace hcore
{

class HCallerInterface
	{
public:
	class HPseudoType {};
	typedef yaal::hcore::HPointer<HCallerInterface, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> ptr_t;
	virtual ~HCallerInterface( void ) {}
	virtual void invoke( void ) = 0;
	};

template<typename call_t,
	typename argA_t = HCallerInterface::HPseudoType,
	typename argB_t = HCallerInterface::HPseudoType,
	typename argC_t = HCallerInterface::HPseudoType,
	typename argD_t = HCallerInterface::HPseudoType,
	typename argE_t = HCallerInterface::HPseudoType,
	typename argF_t = HCallerInterface::HPseudoType,
	typename argG_t = HCallerInterface::HPseudoType,
	typename argH_t = HCallerInterface::HPseudoType,
	typename argI_t = HCallerInterface::HPseudoType,
	typename argJ_t = HCallerInterface::HPseudoType>
class HFunctor
	{
	call_t& call;
public:
	HFunctor( call_t& a_call ) : call( a_call ) {}
	void method(
			argA_t a0, argB_t a1, argC_t a2, argD_t a3, argE_t a4,
			argF_t a5, argG_t a6, argH_t a7, argI_t a8, argJ_t a9 )
		{ call( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
	void method(
			argA_t a0, argB_t a1, argC_t a2, argD_t a3, argE_t a4,
			argF_t a5, argG_t a6, argH_t a7, argI_t a8 )
		{ call( a0, a1, a2, a3, a4, a5, a6, a7, a8 ); }
	void method(
			argA_t a0, argB_t a1, argC_t a2, argD_t a3, argE_t a4,
			argF_t a5, argG_t a6, argH_t a7 )
		{ call( a0, a1, a2, a3, a4, a5, a6, a7 ); }
	void method(
			argA_t a0, argB_t a1, argC_t a2, argD_t a3, argE_t a4,
			argF_t a5, argG_t a6 )
		{ call( a0, a1, a2, a3, a4, a5, a6 ); }
	void method(
			argA_t a0, argB_t a1, argC_t a2, argD_t a3, argE_t a4,
			argF_t a5 )
		{ call( a0, a1, a2, a3, a4, a5 ); }
	void method(
			argA_t a0, argB_t a1, argC_t a2, argD_t a3, argE_t a4 )
		{ call( a0, a1, a2, a3, a4 ); }
	void method( argA_t a0, argB_t a1, argC_t a2, argD_t a3 )
		{ call( a0, a1, a2, a3 ); }
	void method( argA_t a0, argB_t a1, argC_t a2 )
		{ call( a0, a1, a2 ); }
	void method( argA_t a0, argB_t a1 )
		{ call( a0, a1 ); }
	void method( argA_t a0 )
		{ call( a0 ); }
	void method( void )
		{ call(); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename argA_t = HCallerInterface::HPseudoType,
	typename argB_t = HCallerInterface::HPseudoType,
	typename argC_t = HCallerInterface::HPseudoType,
	typename argD_t = HCallerInterface::HPseudoType,
	typename argE_t = HCallerInterface::HPseudoType,
	typename argF_t = HCallerInterface::HPseudoType,
	typename argG_t = HCallerInterface::HPseudoType,
	typename argH_t = HCallerInterface::HPseudoType,
	typename argI_t = HCallerInterface::HPseudoType,
	typename argJ_t = HCallerInterface::HPseudoType>
class HCaller : public HCallerInterface
	{
	CLASS_t f_oObiect;
	METHOD_t METHOD;
	argA_t f_xArgA;
	argB_t f_xArgB;
	argC_t f_xArgC;
	argD_t f_xArgD;
	argE_t f_xArgE;
	argF_t f_xArgF;
	argG_t f_xArgG;
	argH_t f_xArgH;
	argI_t f_xArgI;
	argJ_t f_xArgJ;
public:
	HCaller(
			CLASS_t, METHOD_t,
			argA_t = argA_t(), argB_t = argB_t(), argC_t = argC_t(), argD_t = argD_t(),
			argE_t = argE_t(), argF_t = argF_t(), argG_t = argG_t(), argH_t = argH_t(),
			argI_t = argI_t(), argJ_t = argJ_t()
			);
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, argE_t const*,
			argF_t const*, argG_t const*, argH_t const*, argI_t const*, argJ_t const* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD, f_xArgE, f_xArgF, f_xArgG, f_xArgH, f_xArgI, f_xArgJ );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, argE_t const*,
			argF_t const*, argG_t const*, argH_t const*, argI_t const*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD, f_xArgE, f_xArgF, f_xArgG, f_xArgH, f_xArgI );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, argE_t const*,
			argF_t const*, argG_t const*, argH_t const*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD, f_xArgE, f_xArgF, f_xArgG, f_xArgH );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, argE_t const*,
			argF_t const*, argG_t const*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD, f_xArgE, f_xArgF, f_xArgG );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, argE_t const*,
			argF_t const*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD, f_xArgE, f_xArgF );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, argE_t const*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD, f_xArgE );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, argD_t const*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC, f_xArgD );
		}
	void invoke( 
			argA_t const*, argB_t const*, argC_t const*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB, f_xArgC );
		}
	void invoke( 
			argA_t const*, argB_t const*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA, f_xArgB );
		}
	void invoke( 
			argA_t const*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)( f_xArgA );
		}
	void invoke( 
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*,
			HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType*, HCallerInterface::HPseudoType* )
		{
		(f_oObiect.*METHOD)();
		}
	virtual void invoke( void )
		{
		invoke(
				static_cast<argA_t*>( NULL ),
				static_cast<argB_t*>( NULL ),
				static_cast<argC_t*>( NULL ),
				static_cast<argD_t*>( NULL ),
				static_cast<argE_t*>( NULL ),
				static_cast<argF_t*>( NULL ),
				static_cast<argG_t*>( NULL ),
				static_cast<argH_t*>( NULL ),
				static_cast<argI_t*>( NULL ),
				static_cast<argJ_t*>( NULL ) );
		}
	};

template<typename CLASS_t, typename METHOD_t,
	typename argA_t, typename argB_t, typename argC_t, typename argD_t,
	typename argE_t, typename argF_t, typename argG_t, typename argH_t,
	typename argI_t, typename argJ_t>
HCaller<CLASS_t, METHOD_t, argA_t, argB_t, argC_t, argD_t, argE_t, argF_t, argG_t, argH_t, argI_t, argJ_t>::HCaller(
			CLASS_t a_oObject, METHOD_t A_METHOD,
			argA_t a_xArgA, argB_t a_xArgB,
			argC_t a_xArgC, argD_t a_xArgD,
			argE_t a_xArgE, argF_t a_xArgF,
			argG_t a_xArgG, argH_t a_xArgH,
			argI_t a_xArgI, argJ_t a_xArgJ
			) : f_oObiect( a_oObject ), METHOD( A_METHOD ),
	f_xArgA( a_xArgA ), f_xArgB( a_xArgB ),
	f_xArgC( a_xArgC ), f_xArgD( a_xArgD ),
	f_xArgE( a_xArgE ), f_xArgF( a_xArgF ),
	f_xArgG( a_xArgG ), f_xArgH( a_xArgH ),
	f_xArgI( a_xArgI ), f_xArgJ( a_xArgJ ) {}

}

}

#endif /* not __YAAL_HCORE_HCALLER_H */

