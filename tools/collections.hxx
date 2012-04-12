/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/collections.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_COLLECTIONS_HXX_INCLUDED
#define YAAL_TOOLS_COLLECTIONS_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/algorithm.hxx"
#include "hcore/hlist.hxx"

namespace yaal {

namespace tools {

namespace collections {

template<typename T, typename comparator_t>
inline void sort( yaal::hcore::HList<T>& list_, comparator_t comp_ ) {
	M_PROLOG
	list_.sort( comp_ );
	return;
	M_EPILOG
}

template<typename T>
inline void sort( yaal::hcore::HList<T>& list_ ) {
	M_PROLOG
	list_.sort();
	return;
	M_EPILOG
}

template<typename collection, typename comparator_t>
inline void sort( collection& coll_, comparator_t comp_ ) {
	M_PROLOG
	yaal::sort( coll_.begin(), coll_.end(), comp_ );
	return;
	M_EPILOG
}

template<typename collection>
inline void sort( collection& coll_ ) {
	M_PROLOG
	yaal::sort( coll_.begin(), coll_.end(), yaal::less<typename collection::value_type>() );
	return;
	M_EPILOG
}

template<typename collection, typename T, typename comparator_t>
inline void add_orderly( collection& coll_, T const& value_, comparator_t comp_ ) {
	M_PROLOG
	typename collection::iterator it( yaal::lower_bound( coll_.begin(), coll_.end(), value_, comp_ ) );
	coll_.insert( it, value_ );
	return;
	M_EPILOG
}

template<typename collection, typename T>
inline void add_orderly( collection& coll_, T const& value_ ) {
	M_PROLOG
	add_orderly( coll_, value_, yaal::less<typename collection::value_type>() );
	return;
	M_EPILOG
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_COLLECTIONS_HXX_INCLUDED */

