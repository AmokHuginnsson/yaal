/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/htuple.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/htuple.hxx
 * \brief Declaration of HTuple<> class template.
 */

#ifndef YAAL_HCORE_HTUPLE_HXX_INCLUDED
#define YAAL_HCORE_HTUPLE_HXX_INCLUDED 1

#include "hcore/trait.hxx"

namespace yaal {

namespace hcore {

namespace tuple_helper {

template<typename tuple, int const N>
struct getter;

template<typename tuple>
struct getter<tuple, 0> {
	typedef typename tuple::T0_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e0 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e0 ); }
};

template<typename tuple>
struct getter<tuple, 1> {
	typedef typename tuple::T1_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e1 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e1 ); }
};

template<typename tuple>
struct getter<tuple, 2> {
	typedef typename tuple::T2_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e2 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e2 ); }
};

template<typename tuple>
struct getter<tuple, 3> {
	typedef typename tuple::T3_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e3 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e3 ); }
};

template<typename tuple>
struct getter<tuple, 4> {
	typedef typename tuple::T4_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e4 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e4 ); }
};

template<typename tuple>
struct getter<tuple, 5> {
	typedef typename tuple::T5_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e5 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e5 ); }
};

template<typename tuple>
struct getter<tuple, 6> {
	typedef typename tuple::T6_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e6 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e6 ); }
};

template<typename tuple>
struct getter<tuple, 7> {
	typedef typename tuple::T7_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e7 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e7 ); }
};

template<typename tuple>
struct getter<tuple, 8> {
	typedef typename tuple::T8_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e8 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e8 ); }
};

template<typename tuple>
struct getter<tuple, 9> {
	typedef typename tuple::T9_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e9 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e9 ); }
};

template<typename tuple>
struct getter<tuple, 10> {
	typedef typename tuple::T10_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e10 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e10 ); }
};

template<typename tuple>
struct getter<tuple, 11> {
	typedef typename tuple::T11_type type;
	static type& get( tuple* tuple_ )
		{ return ( tuple_->_e11 ); }
	static type const& get( tuple const* tuple_ )
		{ return ( tuple_->_e11 ); }
};

}

template<typename interface>
class HTupleBase {
public:
	virtual ~HTupleBase( void ) {}
	template<int const N>
	typename tuple_helper::getter<interface, N>::type& get( void )
		{ return ( tuple_helper::getter<interface, N>::get( static_cast<interface*>( this ) ) ); }
	template<int const N>
	typename tuple_helper::getter<interface, N>::type const& get( void ) const
		{ return ( tuple_helper::getter<interface, N>::get( static_cast<interface const*>( this ) ) ); }
};

template<typename T0, typename T1 = trait::no_type, typename T2 = trait::no_type, typename T3 = trait::no_type,
	typename T4 = trait::no_type, typename T5 = trait::no_type, typename T6 = trait::no_type, typename T7 = trait::no_type,
	typename T8 = trait::no_type, typename T9 = trait::no_type, typename T10 = trait::no_type, typename T11 = trait::no_type>
class HTuple;

/*! \brief HTuple<> - a tuple_helper concept implementation.
 */
template<typename T0>
class HTuple<T0> : public HTupleBase<HTuple<T0> > {
public:
	typedef HTuple<T0> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
private:
	T0 _e0;
public:
	HTuple( T0 const& e0_ = T0() )
		: _e0( e0_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1>
class HTuple<T0, T1> : public HTupleBase<HTuple<T0, T1> > {
public:
	typedef HTuple<T0, T1> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
private:
	T0 _e0;
	T1 _e1;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1() )
		: base_type(), _e0( e0_ ), _e1( e1_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2>
class HTuple<T0, T1, T2> : public HTupleBase<HTuple<T0, T1, T2> > {
public:
	typedef HTuple<T0, T1, T2> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3>
class HTuple<T0, T1, T2, T3> : public HTupleBase<HTuple<T0, T1, T2, T3> > {
public:
	typedef HTuple<T0, T1, T2, T3> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(), T3 const& e3_ = T3() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4>
class HTuple<T0, T1, T2, T3, T4> : public HTupleBase<HTuple<T0, T1, T2, T3, T4> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(), T3 const& e3_ = T3(), T4 const& e4_ = T4() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ), _e4( e4_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
class HTuple<T0, T1, T2, T3, T4, T5> : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(), T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ), _e4( e4_ ), _e5( e5_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class HTuple<T0, T1, T2, T3, T4, T5, T6> : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5, T6> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5, T6> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
	typedef T6 T6_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
	T6 _e6;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(),
			T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5(),
			T6 const& e6_ = T6() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ), _e4( e4_ ), _e5( e5_ ), _e6( e6_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class HTuple<T0, T1, T2, T3, T4, T5, T6, T7> : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5, T6, T7> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5, T6, T7> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
	typedef T6 T6_type;
	typedef T7 T7_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
	T6 _e6;
	T7 _e7;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(),
			T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5(),
			T6 const& e6_ = T6(), T7 const& e7_ = T7() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ), _e4( e4_ ), _e5( e5_ ), _e6( e6_ ), _e7( e7_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
class HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
	typedef T6 T6_type;
	typedef T7 T7_type;
	typedef T8 T8_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
	T6 _e6;
	T7 _e7;
	T8 _e8;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(),
			T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5(),
			T6 const& e6_ = T6(), T7 const& e7_ = T7(), T8 const& e8_ = T8() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ),
		_e4( e4_ ), _e5( e5_ ), _e6( e6_ ), _e7( e7_ ), _e8( e8_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
class HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
	typedef T6 T6_type;
	typedef T7 T7_type;
	typedef T8 T8_type;
	typedef T9 T9_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
	T6 _e6;
	T7 _e7;
	T8 _e8;
	T9 _e9;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(),
			T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5(),
			T6 const& e6_ = T6(), T7 const& e7_ = T7(), T8 const& e8_ = T8(),
			T9 const& e9_ = T9() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ),
		_e4( e4_ ), _e5( e5_ ), _e6( e6_ ), _e7( e7_ ), _e8( e8_ ), _e9( e9_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
class HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
	typedef T6 T6_type;
	typedef T7 T7_type;
	typedef T8 T8_type;
	typedef T9 T9_type;
	typedef T10 T10_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
	T6 _e6;
	T7 _e7;
	T8 _e8;
	T9 _e9;
	T10 _e10;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(),
			T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5(),
			T6 const& e6_ = T6(), T7 const& e7_ = T7(), T8 const& e8_ = T8(),
			T9 const& e9_ = T9(), T10 const& e10_ = T10() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ),
		_e4( e4_ ), _e5( e5_ ), _e6( e6_ ), _e7( e7_ ), _e8( e8_ ), _e9( e9_ ), _e10( e10_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
class HTuple : public HTupleBase<HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> > {
public:
	typedef HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> this_type;
	typedef HTupleBase<this_type> base_type;
	typedef T0 T0_type;
	typedef T1 T1_type;
	typedef T2 T2_type;
	typedef T3 T3_type;
	typedef T4 T4_type;
	typedef T5 T5_type;
	typedef T6 T6_type;
	typedef T7 T7_type;
	typedef T8 T8_type;
	typedef T9 T9_type;
	typedef T10 T10_type;
	typedef T11 T11_type;
private:
	T0 _e0;
	T1 _e1;
	T2 _e2;
	T3 _e3;
	T4 _e4;
	T5 _e5;
	T6 _e6;
	T7 _e7;
	T8 _e8;
	T9 _e9;
	T10 _e10;
	T11 _e11;
public:
	HTuple( T0 const& e0_ = T0(), T1 const& e1_ = T1(), T2 const& e2_ = T2(),
			T3 const& e3_ = T3(), T4 const& e4_ = T4(), T5 const& e5_ = T5(),
			T6 const& e6_ = T6(), T7 const& e7_ = T7(), T8 const& e8_ = T8(),
			T9 const& e9_ = T9(), T10 const& e10_ = T10(), T11 const& e11_ = T11() )
		: base_type(), _e0( e0_ ), _e1( e1_ ), _e2( e2_ ), _e3( e3_ ),
		_e4( e4_ ), _e5( e5_ ), _e6( e6_ ), _e7( e7_ ), _e8( e8_ ), _e9( e9_ ), _e10( e10_ ), _e11( e11_ ) {}
	template<typename tuple, int const N>
	friend struct tuple_helper::getter;
};

#if CXX_STANDARD >= 2011

template<typename... T>
HTuple<T...> make_tuple( T... e_ )
	{ return ( HTuple<T...>( e_... ) ); }

#else /* #if CXX_STANDARD >= 2011 */

template<typename T0>
HTuple<T0> make_tuple( T0 const& e0_ )
	{ return ( HTuple<T0>( e0_ ) ); }

template<typename T0, typename T1>
HTuple<T0, T1> make_tuple( T0 const& e0_, T1 const& e1_ )
	{ return ( HTuple<T0, T1>( e0_, e1_ ) ); }

template<typename T0, typename T1, typename T2>
HTuple<T0, T1, T2> make_tuple( T0 const& e0_, T1 const& e1_, T2 const& e2_ )
	{ return ( HTuple<T0, T1, T2>( e0_, e1_, e2_ ) ); }

template<typename T0, typename T1, typename T2, typename T3>
HTuple<T0, T1, T2, T3> make_tuple( T0 const& e0_, T1 const& e1_, T2 const& e2_, T3 const& e3_ )
	{ return ( HTuple<T0, T1, T2, T3>( e0_, e1_, e2_, e3_ ) ); }

template<typename T0, typename T1, typename T2, typename T3, typename T4>
HTuple<T0, T1, T2, T3, T4> make_tuple( T0 const& e0_, T1 const& e1_, T2 const& e2_, T3 const& e3_, T4 const& e4_ )
	{ return ( HTuple<T0, T1, T2, T3, T4>( e0_, e1_, e2_, e3_, e4_ ) ); }

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
HTuple<T0, T1, T2, T3, T4, T5> make_tuple( T0 const& e0_, T1 const& e1_, T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5>( e0_, e1_, e2_, e3_, e4_, e5_ ) ); }

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
HTuple<T0, T1, T2, T3, T4, T5, T6> make_tuple( T0 const& e0_, T1 const& e1_,
		T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_, T6 const& e6_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5, T6>( e0_, e1_, e2_, e3_, e4_, e5_, e6_ ) ); }

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
HTuple<T0, T1, T2, T3, T4, T5, T6, T7> make_tuple( T0 const& e0_, T1 const& e1_,
		T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_, T6 const& e6_,
		T7 const& e7_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5, T6, T7>( e0_, e1_, e2_, e3_, e4_, e5_, e6_, e7_ ) ); }

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> make_tuple( T0 const& e0_, T1 const& e1_,
		T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_, T6 const& e6_,
		T7 const& e7_, T8 const& e8_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8>( e0_, e1_, e2_, e3_, e4_, e5_, e6_, e7_, e8_ ) ); }

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> make_tuple( T0 const& e0_, T1 const& e1_,
		T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_, T6 const& e6_,
		T7 const& e7_, T8 const& e8_, T9 const& e9_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>( e0_, e1_, e2_, e3_, e4_, e5_, e6_, e7_, e8_, e9_ ) ); }

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> make_tuple( T0 const& e0_, T1 const& e1_,
		T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_, T6 const& e6_,
		T7 const& e7_, T8 const& e8_, T9 const& e9_, T10 const& e10_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>( e0_, e1_, e2_, e3_, e4_, e5_, e6_, e7_, e8_, e9_, e10_ ) ); }

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> make_tuple( T0 const& e0_, T1 const& e1_,
		T2 const& e2_, T3 const& e3_, T4 const& e4_, T5 const& e5_, T6 const& e6_,
		T7 const& e7_, T8 const& e8_, T9 const& e9_, T10 const& e10_, T11 const& e11_ )
	{ return ( HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>( e0_, e1_, e2_, e3_, e4_, e5_, e6_, e7_, e8_, e9_, e10_, e11_ ) ); }

#endif /* #else #if CXX_STANDARD >= 2011 */

}

}

#endif /* #ifndef YAAL_HCORE_HTUPLE_HXX_INCLUDED */

