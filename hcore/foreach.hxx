/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	foreach.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/foreach.hxx
 * \brief Definition of YAAL_FOREACH() macro.
 *
 * YAAL_FOREACH() gives for ( type x : col ) functionality to C++2003.
 */

#ifndef YAAL_HCORE_FOREACH_HXX_INCLUDED
#define YAAL_HCORE_FOREACH_HXX_INCLUDED 1

#include "hcore/trait.hxx"

namespace yaal {

namespace foreach_helper {

template<typename collection_t>
struct iterator_type {
	typedef typename collection_t::iterator iterator;
};

template<typename collection_t>
struct iterator_type<collection_t const&> {
	typedef typename collection_t::const_iterator iterator;
};

template<typename collection_t>
struct iterator_type<collection_t const> {
	typedef typename collection_t::const_iterator iterator;
};

template<typename collection_t, int const SIZE>
struct iterator_type<collection_t[SIZE]> {
	typedef collection_t* iterator;
};

template<typename collection_t, int const SIZE>
struct iterator_type<collection_t const[SIZE]> {
	typedef collection_t const* iterator;
};

typedef struct auto_storage_base {
} const& auto_storage_t;

template<typename T>
struct auto_storage : public auto_storage_base {
	mutable T _val;
	auto_storage( T const& val_ )
		: _val( val_ ) {
	}
};

template<typename T>
auto_storage<T> make_iterator( T iterator ) {
	return ( auto_storage<T>( iterator ) );
}

template<typename collection_t>
typename iterator_type<collection_t>::iterator& auto_storage_pick( auto_storage_t autoStorage_, collection_t& ) {
	return ( static_cast<auto_storage<typename iterator_type<collection_t>::iterator> const&>( autoStorage_ )._val );
}

}

}

#define YAAL_FOREACH_ID( name ) M_CONCAT( name, __LINE__ )
#if CXX_STANDARD >= 2011
#	define YAAL_FOREACH( var, collection ) for ( var: collection )
#else /* #if CXX_STANDARD >= 2011 */
#	ifdef HAVE_DECLTYPE
#		define YAAL_FOREACH( var, collection ) \
		if ( bool YAAL_FOREACH_ID( _FOR_EACH_LOOP ) = false ) { \
		} else for ( yaal::foreach_helper::iterator_type<typeof ( collection )>::iterator YAAL_FOREACH_ID( it )( begin( collection ) ), \
				YAAL_FOREACH_ID( end )( end( collection ) ); \
				! YAAL_FOREACH_ID( _FOR_EACH_LOOP ) && ( YAAL_FOREACH_ID( it ) != YAAL_FOREACH_ID( end ) ) && ( YAAL_FOREACH_ID( _FOR_EACH_LOOP ) = true ); \
				++ YAAL_FOREACH_ID( it ) ) \
			if ( bool YAAL_FOREACH_ID( _FOR_EACH_STEP ) = false ) { \
			} else for ( var( *YAAL_FOREACH_ID( it ) ); ! YAAL_FOREACH_ID( _FOR_EACH_STEP ); \
					YAAL_FOREACH_ID( _FOR_EACH_LOOP ) = false, YAAL_FOREACH_ID( _FOR_EACH_STEP ) = true )
#	else /* # ifdef HAVE_DECLTYPE */
#		define YAAL_FOREACH( var, collection ) \
		if ( bool YAAL_FOREACH_ID( _FOR_EACH_LOOP ) = false ) { \
		} else for ( yaal::foreach_helper::auto_storage_t YAAL_FOREACH_ID( it ) = yaal::foreach_helper::make_iterator( begin( collection ) ), \
				YAAL_FOREACH_ID( end ) = yaal::foreach_helper::make_iterator( end( collection ) ); \
				! YAAL_FOREACH_ID( _FOR_EACH_LOOP ) \
				&& yaal::foreach_helper::auto_storage_pick( YAAL_FOREACH_ID( it ), collection ) \
				!= yaal::foreach_helper::auto_storage_pick( YAAL_FOREACH_ID( end ), collection ) \
				&& ( YAAL_FOREACH_ID( _FOR_EACH_LOOP ) = true ); \
				++ yaal::foreach_helper::auto_storage_pick( YAAL_FOREACH_ID( it ), collection ) ) \
			if ( bool YAAL_FOREACH_ID( _FOR_EACH_STEP ) = false ) { \
			} else for ( var( *yaal::foreach_helper::auto_storage_pick( YAAL_FOREACH_ID( it ), collection ) ); \
					! YAAL_FOREACH_ID( _FOR_EACH_STEP ); YAAL_FOREACH_ID( _FOR_EACH_LOOP ) = false, YAAL_FOREACH_ID( _FOR_EACH_STEP ) = true )
#	endif /* #	else # ifdef HAVE_DECLTYPE */
#endif /* #else #if CXX_STANDARD >= 2011 */

#endif /* #ifndef YAAL_HCORE_FOREACH_HXX_INCLUDED */

