/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

template<typename collection>
inline typename collection::iterator n_th( collection& coll_, int long nth_ ) {
	M_PROLOG
	M_ENSURE( ( nth_ >= 0 ) && ( nth_ < coll_.size() ) );
	typename collection::iterator it( coll_.begin() );
	yaal::advance( it, nth_ );
	return it;
	M_EPILOG
}

template<typename collection>
inline typename collection::const_iterator n_th( collection const& coll_, int long nth_ ) {
	M_PROLOG
	M_ENSURE( ( nth_ >= 0 ) && ( nth_ < coll_.size() ) );
	typename collection::iterator it( coll_.begin() );
	yaal::advance( it, nth_ );
	return it;
	M_EPILOG
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_COLLECTIONS_HXX_INCLUDED */

