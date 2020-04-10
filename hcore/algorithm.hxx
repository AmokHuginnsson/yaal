/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/algorithm.hxx
 * \brief Basic algorithms definitions.
 */

#ifndef YAAL_HCORE_ALGORITHM_HXX_INCLUDED
#define YAAL_HCORE_ALGORITHM_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/algorithm_low.hxx"
#include "hcore/trait.hxx"
#include "hcore/hpair.hxx"
#include "hcore/iterator.hxx"
#include "hcore/functional.hxx"
#include "hcore/hauxiliarybuffer.hxx"
#include "hcore/random.hxx"

namespace yaal {

/*! \brief Find first occurrence of given value in given range of elements.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param v - look for this value in given range.
 * \return iterator pointing to found value or end of range.
 */
template<typename iterator_t, typename value_t>
inline iterator_t find( iterator_t it, iterator_t end, value_t const& v ) {
	for ( ; ( it != end ) && ( *it != v ); ++ it ) {
	}
	return ( it );
}

/*! \brief Find first element in range that meets a given condition.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param cond - condition which must be satisfied.
 * \return iterator pointing to found element or end of range.
 */
template<typename iterator_t, typename condition_t>
inline iterator_t find_if( iterator_t it, iterator_t end, condition_t cond ) {
	for ( ; ( it != end ) && ( ! cond( *it ) ); ++ it ) {
	}
	return ( it );
}

/*! \brief Find first element in range that does not meet a given condition.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param cond - condition which check must fail.
 * \return iterator pointing to found element or end of range.
 */
template<typename iterator_t, typename condition_t>
inline iterator_t find_if_not( iterator_t it, iterator_t end, condition_t cond ) {
	for ( ; ( it != end ) && cond( *it ); ++ it ) {
	}
	return ( it );
}

/*! \brief Tell if all element of a range meet predicate condition.
 *
 * \param it - beginning of the range to check for the condition.
 * \param end - one past the end of the range to check for the condition.
 * \param cond - condition which must be satisfied.
 * \return True iff every single element from given range fulfills given condition.
 */
template<typename iterator_t, typename condition_t>
inline bool all_of( iterator_t it, iterator_t end, condition_t cond ) {
	return ( find_if_not( it, end, cond ) == end );
}

/*! \brief Tell if any of the elements in a range meet predicate condition.
 *
 * \param it - beginning of the range to check for the condition.
 * \param end - one past the end of the range to check for the condition.
 * \param cond - condition which must be satisfied.
 * \return True iff at least one element in given range fulfills the condition.
 */
template<typename iterator_t, typename condition_t>
inline bool any_of( iterator_t it, iterator_t end, condition_t cond ) {
	return ( find_if( it, end, cond ) != end );
}

/*! \brief Tell if none of the elements in a range meet predicate condition.
 *
 * \param it - beginning of the range to check for the condition.
 * \param end - one past the end of the range to check for the condition.
 * \param cond - condition which check must fail.
 * \return True iff none of the element in given range fulfill given condition.
 */
template<typename iterator_t, typename condition_t>
inline bool none_of( iterator_t it, iterator_t end, condition_t cond ) {
	return ( find_if( it, end, cond ) == end );
}

/*! \brief Find first occurrence of value from given set in given range of elements.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param first2_ - beginning of range of elements to look in.
 * \param last2_ - one past the end of range of elements to look in.
 * \return iterator pointing to found value or end of range.
 */
template<typename iterator_t, typename iter2_t>
inline iterator_t find_first_of( iterator_t it, iterator_t end, iter2_t first2_, iter2_t last2_ ) {
	for ( ; ( it != end ); ++ it ) {
		if ( find( first2_, last2_, *it ) != last2_ ) {
			break;
		}
	}
	return ( it );
}

/*! \brief Find first occurrence of value from given set in given range of elements, using condition to match elements.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param first2_ - beginning of range of elements to look in.
 * \param last2_ - one past the end of range of elements to look in.
 * \param cond_ - use condition for matching elements.
 * \return iterator pointing to found value or end of range.
 */
template<typename iterator_t, typename iter2_t, typename cond_t>
inline iterator_t find_first_of( iterator_t it, iterator_t end, iter2_t first2_, iter2_t last2_, cond_t cond_ ) {
	for ( ; ( it != end ); ++ it ) {
		iter2_t v( first2_ );
		for ( ; ( v != last2_ ) && ( ! cond_( *it, *v ) ); ++ v ) {
		}
		if ( v != last2_ ) {
			break;
		}
	}
	return ( it );
}

/*! \brief Find first occurrence of value that is not in given set of values in given range of elements.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param first2_ - beginning of range of elements to look in.
 * \param last2_ - one past the end of range of elements to look in.
 * \return iterator pointing to found value or end of range.
 */
template<typename iterator_t, typename iter2_t>
inline iterator_t find_first_not_of( iterator_t it, iterator_t end, iter2_t first2_, iter2_t last2_ ) {
	for ( ; ( it != end ); ++ it ) {
		if ( ! ( find( first2_, last2_, *it ) != last2_ ) ) {
			break;
		}
	}
	return ( it );
}

/*! \brief Find first occurrence of value that is not in given set of values in given range of elements, using condition to match elements.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param first2_ - beginning of range of elements to look in.
 * \param last2_ - one past the end of range of elements to look in.
 * \param cond_ - use condition for matching elements.
 * \return iterator pointing to found value or end of range.
 */
template<typename iterator_t, typename iter2_t, typename cond_t>
inline iterator_t find_first_not_of( iterator_t it, iterator_t end, iter2_t first2_, iter2_t last2_, cond_t cond_ ) {
	for ( ; ( it != end ); ++ it ) {
		iter2_t v( first2_ );
		for ( ; ( v != last2_ ) && ( ! cond_( *it, *v ) ); ++ v ) {
		}
		if ( ! ( v != last2_ ) ) {
			break;
		}
	}
	return ( it );
}

/*! \brief Count number of occurrences of element in range.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param v - look for this value in given range.
 * \return Number of occurrences of v in range.
 */
template<typename iterator_t, typename value_t>
inline int long count( iterator_t it, iterator_t end, value_t const& v ) {
	int long counter( 0 );
	for ( ; it != end; ++ it ) {
		if ( *it == v ) {
			++ counter;
		}
	}
	return ( counter );
}

/*! \brief Count number of element meeting a predicate in range.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param cond - condition which must be satisfied.
 * \return Number of elements in range that satisfy a given condition.
 */
template<typename iterator_t, typename condition_t>
inline int long count_if( iterator_t it, iterator_t end, condition_t cond ) {
	int long counter( 0 );
	for ( ; it != end; ++ it ) {
		if ( cond( *it ) ) {
			++ counter;
		}
	}
	return ( counter );
}

/*! \brief For each element in range pass its value into function.
 *
 * \param it - beginning of the range.
 * \param end - one past the end of range.
 * \param CALL - invoke this function and pass values from range to it.
 */
template<typename iterator_t, typename call_t>
inline void for_each( iterator_t it, iterator_t const& end, call_t CALL ) {
	for ( ; it != end; ++ it ) {
		CALL( *it );
	}
}

/*! \brief Replace elements in destination container by transformed elements from source range.
 *
 * \param it - beginning of source range.
 * \param end - one past the end of source range.
 * \param dst - beginning of destination container.
 * \param op - transforming operation, an unary function.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename operation_t>
inline dst_iter_t transform( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op ) {
	for ( ; it != end; ++ it, ++ dst ) {
		*dst = op( *it );
	}
	return ( dst );
}

/*! \brief Replace elements in destination container by transformed elements, from source range, that are not equal to some value.
 *
 * \param it - beginning of source range.
 * \param end - one past the end of source range.
 * \param dst - beginning of destination container.
 * \param skip_ - value that should be skipped while performing range transformation.
 * \param op - transforming operation, an unary function.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename skip_t, typename operation_t>
inline dst_iter_t remove_transform( src_iter_t it, src_iter_t end, dst_iter_t dst, skip_t skip_, operation_t op ) {
	for ( ; it != end; ++ it ) {
		if ( *it != skip_ ) {
			*dst = op( *it );
			++ dst;
		}
	}
	return ( dst );
}

/*! \brief Replace elements in destination container by transformed elements, from source range, that do not satisfy some predicate.
 *
 * \param it - beginning of source range.
 * \param end - one past the end of source range.
 * \param dst - beginning of destination container.
 * \param predicate_ - predicate used to filter out unwanted elements.
 * \param op - transforming operation, an unary function.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename predicate_t, typename operation_t>
inline dst_iter_t remove_transform_if( src_iter_t it, src_iter_t end, dst_iter_t dst, predicate_t predicate_, operation_t op ) {
	for ( ; it != end; ++ it ) {
		if ( ! predicate_( *it ) ) {
			*dst = op( *it );
			++ dst;
		}
	}
	return ( dst );
}

/*! \brief Replace elements in destination container by elements from source range conditionally transformed.
 *
 * \param it - beginning of source range.
 * \param end - one past the end of source range.
 * \param dst - beginning of destination container.
 * \param op - transforming operation, an unary function.
 * \param cond - condition that has to be met to perform a transformation.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename operation_t, typename condition_t>
inline dst_iter_t transform_if( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op, condition_t cond ) {
	for ( ; it != end; ++ it, ++ dst ) {
		if ( cond( *it ) ) {
			*dst = op( *it );
		}	else {
			*dst = *it;
		}
	}
	return ( dst );
}

/*! \brief Replace elements in destination container by elements resulting from transformation of elements from source range and another container.
 *
 * \param it - beginning of source range.
 * \param end - one past the end of source range.
 * \param arg - beginning of second source container.
 * \param dst - beginning of destination container.
 * \param op - transforming operation, a binary function.
 * \return end of output range.
 */
template<typename src_iter_t, typename arg_iter_t, typename dst_iter_t, typename operation_t>
inline dst_iter_t transform( src_iter_t it, src_iter_t end, arg_iter_t arg, dst_iter_t dst, operation_t op ) {
	for ( ; it != end; ++ it, ++ arg, ++ dst ) {
		*dst = op( *it, *arg );
	}
	return ( dst );
}

/*! \brief Replace all occurrences of given value in range with new value.
 * \param it - beginning of range to modify.
 * \param end - one past the end of range to modify.
 * \param oldVal_ - old value that shall to be replaced.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename value_t>
inline void replace( iterator_t it, iterator_t end, value_t const& oldVal_, value_t const& newVal_ ) {
	for ( ; it != end; ++ it ) {
		if ( *it == oldVal_ ) {
			*it = newVal_;
		}
	}
	return;
}

/*! \brief Replace all occurrences of values in range fulfilling given predicate with new value.
 * \param it - beginning of range to modify.
 * \param end - one past the end of range to modify.
 * \param predicate_ - a predicate to fulfill so a value could be replaced.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename value_t, typename predicate_t>
inline void replace_if( iterator_t it, iterator_t end, predicate_t predicate_, value_t const& newVal_ ) {
	for ( ; it != end; ++ it ) {
		if ( predicate_( *it ) ) {
			*it = newVal_;
		}
	}
	return;
}

/*! \brief Copies one range onto the another but with replacing all occurrences of given value with new value.
 * \param it - beginning of range to copy.
 * \param end - one past the end of range to copy.
 * \param out_ - beginning of output range.
 * \param oldVal_ - old value that shall to be replaced.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename output_iterator_t, typename value_t>
inline void replace_copy( iterator_t it, iterator_t end, output_iterator_t out, value_t const& oldVal_, value_t const& newVal_ ) {
	for ( ; it != end; ++ it, ++ out ) {
		if ( *it == oldVal_ ) {
			*out = newVal_;
		}	else {
			*out = *it;
		}
	}
	return;
}

/*! \brief Copies one range onto the another but with replacing all occurrences of value satisfying a given predicate with new value.
 * \param it - beginning of range to copy.
 * \param end - one past the end of range to copy.
 * \param out_ - beginning of output range.
 * \param predicate_ - replace all elements that satisfy this predicate.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename output_iterator_t, typename predicate_t, typename value_t>
inline void replace_copy_if( iterator_t it, iterator_t end, output_iterator_t out, predicate_t predicate_, value_t const& newVal_ ) {
	for ( ; it != end; ++ it, ++ out ) {
		if ( predicate_( *it ) ) {
			*out = newVal_;
		}	else {
			*out = *it;
		}
	}
	return;
}

/*! \brief Copy range of values onto another range.
 *
 * \param src - beginning of source range of elements.
 * \param end - one past the end of source range of elements.
 * \param dst - beginning of destination range.
 * \return one past end of destination range.
 */
template<typename src_it_t, typename dst_it_t>
inline dst_it_t copy( src_it_t src, src_it_t const& end, dst_it_t dst ) {
	for ( ; src != end; ++ src, ++ dst ) {
		*dst = *src;
	}
	return ( dst );
}

/*! \brief Copy range of values onto another range.
 *
 * \param first - beginning of source range of elements.
 * \param last - one past the end of source range of elements.
 * \param dst - one past end of destination range.
 * \return beginning of destination range.
 */
template<typename src_it_t, typename dst_it_t>
inline dst_it_t copy_backward( src_it_t const& first, src_it_t last, dst_it_t dst ) {
	if ( first != last ) {
		-- last;
		-- dst;
		for ( ; first != last; -- last, -- dst ) {
			*dst = *last;
		}
		*dst = *last;
	}
	return ( dst );
}

/*! \brief Copy given number of elements from range of values onto another range.
 *
 * \param src - beginning of source range of elements.
 * \param count - number of elements to copy.
 * \param dst - beginning of destination range.
 * \return one past end of destination range.
 */
template<typename src_it_t, typename dst_it_t>
inline dst_it_t copy_n( src_it_t src, int long count, dst_it_t dst ) {
	for ( int long i = 0; i < count; ++ i, ++ src, ++ dst ) {
		*dst = *src;
	}
	return ( dst );
}

/*! \brief Move range of values onto another range.
 *
 * \param src - beginning of source range of elements.
 * \param end - one past the end of source range of elements.
 * \param dst - beginning of destination range.
 * \return one past end of destination range.
 */
template<typename src_it_t, typename dst_it_t>
inline dst_it_t move( src_it_t src, src_it_t const& end, dst_it_t dst ) {
	for ( ; src != end; ++ src, ++ dst ) {
		*dst = yaal::move( *src );
	}
	return ( dst );
}

/*! \brief Move range of values onto another range.
 *
 * \param first - beginning of source range of elements.
 * \param last - one past the end of source range of elements.
 * \param dst - one past end of destination range.
 * \return beginning of destination range.
 */
template<typename src_it_t, typename dst_it_t>
inline dst_it_t move_backward( src_it_t const& first, src_it_t last, dst_it_t dst ) {
	if ( first != last ) {
		-- last;
		-- dst;
		for ( ; first != last; -- last, -- dst ) {
			*dst = yaal::move( *last );
		}
		*dst = yaal::move( *last );
	}
	return ( dst );
}

/*! \brief Move given number of elements from range of values onto another range.
 *
 * \param src - beginning of source range of elements.
 * \param count - number of elements to move.
 * \param dst - beginning of destination range.
 * \return one past end of destination range.
 */
template<typename src_it_t, typename dst_it_t>
inline dst_it_t move_n( src_it_t src, int long count, dst_it_t dst ) {
	for ( int long i = 0; i < count; ++ i, ++ src, ++ dst ) {
		*dst = yaal::move( *src );
	}
	return ( dst );
}

/*! \brief Remove all occurrences of given value from range.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param value_ - a value to be removed.
 * \return new end of range - none of elements in resulting range meet a condition.
 */
template<typename iterator_t, typename value_t>
inline iterator_t remove( iterator_t first_, iterator_t const& last_, value_t value_ ) {
	bool move( false );
	for ( iterator_t it( first_ ); it != last_; ++ it ) {
		if ( *it == value_ ) {
			move = true;
			continue;
		}
		if ( move ) {
			*first_ = *it;
		}
		++ first_;
	}
	return ( first_ );
}

/*! \brief Remove elements meeting a predicate from range.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param condition_ - a predicate that element must fulfill to be removed.
 * \return new end of range - none of elements in resulting range meet a condition.
 */
template<typename iterator_t, typename condition_t>
inline iterator_t remove_if( iterator_t first_, iterator_t const& last_, condition_t condition_ ) {
	bool move( false );
	for ( iterator_t it( first_ ); it != last_; ++ it ) {
		if ( condition_( *it ) ) {
			move = true;
			continue;
		}
		if ( move ) {
			*first_ = *it;
		}
		++ first_;
	}
	return ( first_ );
}

/*! \brief Copy elements all elements not equal to given value from one range to another.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param res_ - beginning of output range.
 * \param value_ - elements not equal to this value will be copied..
 * \return end of output range - none of elements in resulting range are equal to given value.
 */
template<typename iter1_t, typename iter2_t, typename value_t>
inline iter2_t remove_copy( iter1_t first_, iter1_t const& last_, iter2_t res_, value_t value_ ) {
	for ( ; first_ != last_; ++ first_ ) {
		if ( *first_ != value_ ) {
			*res_ = *first_;
			++ res_;
		}
	}
	return ( res_ );
}

/*! \brief Copy elements that do not meet given predicate from first one to another.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param res_ - beginning of output range.
 * \param condition_ - a predicate that element must fulfill to be removed.
 * \return end of output range - none of elements in resulting range meet a condition.
 */
template<typename iter1_t, typename iter2_t, typename condition_t>
inline iter2_t remove_copy_if( iter1_t first_, iter1_t const& last_, iter2_t res_, condition_t condition_ ) {
	for ( ; first_ != last_; ++ first_ ) {
		if ( ! condition_( *first_ ) ) {
			*res_ = *first_;
			++ res_;
		}
	}
	return ( res_ );
}

/*! \brief Checks if two ranges are of same size and have same set of values.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past last element of second range.
 * \param equity - an element equity predicate that has to be met for all compared elements.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t, typename equity_t>
inline bool safe_equal( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2, equity_t equity ) {
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && equity( *it1, *it2 ); ++ it1, ++ it2 ) {
	}
	return ( ( it1 == end1 ) && ( it2 == end2 ) );
}

/*! \brief Checks if two ranges are of same size and have same set of values.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past last element of second range.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t>
inline bool safe_equal( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 ) {
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 ) {
	}
	return ( ( it1 == end1 ) && ( it2 == end2 ) );
}

/*! \cond */
template<typename iter1_t, typename iter2_t, typename equity_t>
inline bool search_try_subsequence( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2, equity_t equity ) {
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && equity( *it1, *it2 ); ++ it1, ++ it2 ) {
	}
	return ( it1 == end1 );
}
/*! \condend */

/*! \brief Search for sequence of elements in another sequence.
 * \param first1 - beginning of sequence that we search through.
 * \param last1 - one past the end of sequence that we search through.
 * \param first2 - beginning of sequence that we are looking for.
 * \param last2 - one past the end of the sequence that we are looking for.
 * \param equity - a predicate that shall be met for elements in respective ranges.
 * \return Beginning of found sequence or one past the end of sequence that we searched through.
 */
template<typename iter1_t, typename iter2_t, typename equity_t>
inline iter1_t search( iter1_t first1, iter1_t last1, iter2_t first2, iter2_t last2, equity_t equity ) {
	/* Naive - quadratic complexity implementation. */
	while ( ( first1 != last1 ) && ! search_try_subsequence( first2, last2, first1, last1, equity ) ) {
		++ first1;
	}
	return ( first1 );
}

/*! \brief Search for sequence of elements in another sequence.
 * \param first1 - beginning of sequence that we search through.
 * \param last1 - one past the end of sequence that we search through.
 * \param first2 - beginning of sequence that we are looking for.
 * \param last2 - one past the end of the sequence that we are looking for.
 * \return Beginning of found sequence or one past the end of sequence that we searched through.
 */
template<typename iter1_t, typename iter2_t>
inline iter1_t search( iter1_t first1, iter1_t last1, iter2_t first2, iter2_t last2 ) {
	return ( search( first1, last1, first2, last2, equal_to<typename hcore::iterator_traits<iter1_t>::value_type>() ) );
}

/*! \brief Checks if two ranges have same set of values.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t>
inline bool equal( iter1_t it1, iter1_t end1, iter2_t it2 ) {
	for ( ; ( it1 != end1 ) && ( *it1 == *it2 ); ++ it1, ++ it2 ) {
	}
	return ( it1 == end1 );
}

/*! \brief Checks if two ranges are equivalent according to some predicate.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param predicate_ - a predicate to test, an equity equivalence.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t, typename predicate_t>
inline bool equal( iter1_t it1, iter1_t end1, iter2_t it2, predicate_t predicate_ ) {
	for ( ; ( it1 != end1 ) && predicate_( *it1, *it2 ); ++ it1, ++ it2 ) {
	}
	return ( it1 == end1 );
}

/*! \brief Checks if one range is lexicographically before another range.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past last element of second range.
 * \param compare - comparison function.
 * \return true if and only if first range is lexicographically before second range.
 */
template<typename iter1_t, typename iter2_t, typename compare_t>
inline bool lexicographical_compare( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2, compare_t compare ) {
	/* skip equals */
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( ! compare( *it1, *it2 ) ) && ( ! compare( *it2, *it1 ) ); ++ it1, ++ it2 ) {
	}
	return ( ( ( it1 != end1 ) && ( it2 != end2 ) && compare( *it1, *it2 ) ) || ( ( it1 == end1 ) && ( it2 != end2 ) ) );
}

/*! \brief Checks if one range is lexicographically before another range.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past last element of second range.
 * \return true if and only if first range is lexicographically before second range.
 */
template<typename iter1_t, typename iter2_t>
inline bool lexicographical_compare( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 ) {
	/* skip equals */
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 ) {
	}
	return ( ( ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 < *it2 ) ) || ( ( it1 == end1 ) && ( it2 != end2 ) ) );
}

/*! \brief Find first difference in ranges values or difference in range sizes.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past last element of second range.
 * \return A pair of iterators which point to first differentiating element.
 */
template<typename iter1_t, typename iter2_t>
inline yaal::hcore::HPair<iter1_t, iter2_t> safe_mismatch( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 ) {
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 ) {
	}
	return ( make_pair( it1, it2 ) );
}

/*! \brief Find first difference in ranges of values.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \return A pair of iterators which point to first differentiating element.
 */
template<typename iter1_t, typename iter2_t>
inline yaal::hcore::HPair<iter1_t, iter2_t> mismatch( iter1_t it1, iter1_t end1, iter2_t it2 ) {
	for ( ; ( it1 != end1 ) && ( *it1 == *it2 ); ++ it1, ++ it2 ) {
	}
	return ( make_pair( it1, it2 ) );
}

/*! \brief Find first difference in ranges of values.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - beginning of second range.
 * \param predicate_ - a predicate to test a differences with.
 * \return A pair of iterators which point to first differentiating element.
 */
template<typename iter1_t, typename iter2_t, typename predicate_t>
inline yaal::hcore::HPair<iter1_t, iter2_t> mismatch( iter1_t it1, iter1_t end1, iter2_t it2, predicate_t predicate_ ) {
	for ( ; ( it1 != end1 ) && predicate_( *it1, *it2 ); ++ it1, ++ it2 ) {
	}
	return ( make_pair( it1, it2 ) );
}

/*! \brief Rotate range of elements, or "swap" two consecutive ranges of elements.
 * \param first_ - beginning of range to rotate, or beginning of first range to "swap".
 * \param mid_ - the rotation point, or one past end of first range to "swap" and beginning of the second range.
 * \param last_ - one past end of range to rotate, or one past end of the second range to "swap".
 * \return A rotation point that could be used to revert the operation, or new beginning of new second range.
 */
template<typename iterator_t>
inline iterator_t rotate( iterator_t first_, iterator_t mid_, iterator_t last_ ) {
	iterator_t newMid( last_ );
	if ( mid_ != last_ ) {
		iterator_t it( mid_ );
		iterator_t mid( mid_ );
		while ( first_ != it ) {
			swap( *first_, *it );
			++ first_;
			++ it;
			++ mid;
			if ( it == last_ ) {
				it = mid_;
			} else if ( first_ == mid_ ) {
				mid_ = it;
			}
			if ( mid == last_ ) {
				newMid = first_;
				mid = first_;
			}
		}
	}
	return ( newMid );
}

/*! \brief Rotate range of elements, or "swap" two consecutive ranges of elements storing result in another range.
 * \param first_ - beginning of range to rotate, or beginning of first range to "swap".
 * \param mid_ - the rotation point, or one past end of first range to "swap" and beginning of the second range.
 * \param last_ - one past end of range to rotate, or one past end of the second range to "swap".
 * \param out_ - beginning of destination range.
 * \return A rotation point that could be used to revert the operation, or new beginning of new second range.
 */
template<typename iterator_t, typename out_it_t>
inline out_it_t rotate_copy( iterator_t first_, iterator_t mid_, iterator_t last_, out_it_t out_ ) {
	out_it_t it( copy( mid_, last_, out_ ) );
	it = copy( first_, mid_, it );
	return ( it );
}

/*! \cond */
template<typename iterator_t, typename value_t, typename compare_t>
inline iterator_t lower_bound( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_, hcore::iterator_category::forward ) {
	iterator_t it( first_ );
	iterator_t mid( first_ );
	while ( it != last_ ) {
		++ it;
		if ( ! ( it != last_ ) ) {
			break;
		}
		++ mid;
		++ it;
	}
	if ( mid != first_ ) {
		if ( comp_( *mid, value_ ) ) {
			mid = lower_bound( mid, last_, value_, comp_, hcore::iterator_category::forward() );
		}	else {
			mid = lower_bound( first_, mid, value_, comp_, hcore::iterator_category::forward() );
		}
	} else {
		if ( ( mid != last_ ) && comp_( *mid, value_ ) ) {
			++ mid;
		}
	}
	return ( mid );
}
template<typename iterator_t, typename value_t, typename compare_t>
inline iterator_t lower_bound( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_, hcore::iterator_category::random_access ) {
	iterator_t mid( first_ + ( last_ - first_ ) / 2 );
	while ( mid != first_ ) {
		if ( comp_( *mid, value_ ) ) {
			first_ = mid;
		}	else {
			last_ = mid;
		}
		mid = ( first_ + ( last_ - first_ ) / 2 );
	}
	if ( ( mid != last_ ) && comp_( *mid, value_ ) ) {
		++ mid;
	}
	return ( mid );
}
/*! \endcond */

/*! \brief Find first position in ordered (where order is given) range where given value could be inserted without violating ordering.
 *
 * \pre Range first_ - last_ must be ordered in given order.
 *
 * \param first_ - beginning of ordered range to search through.
 * \param last_ - one past the end of ordered range to search through.
 * \param value_ - lower bound value.
 * \param comp_ - comparison operator that defines order.
 * \return First position in range where value could be inserted without violating ordering.
 */
template<typename iterator_t, typename value_t, typename compare_t>
inline iterator_t lower_bound( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_ ) {
	return ( lower_bound( first_, last_, value_, comp_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
}

/*! \brief Find first position in ordered range where given value could be inserted without violating ordering.
 *
 * \pre Range first_ - last_ must be ordered.
 *
 * \param first_ - beginning of ordered range to search through.
 * \param last_ - one past the end of ordered range to search through.
 * \param value_ - lower bound value.
 * \return First position in range where value could be inserted without violating ordering.
 */
template<typename iterator_t, typename value_t>
inline iterator_t lower_bound( iterator_t first_, iterator_t last_, value_t const& value_ ) {
	return ( lower_bound( first_, last_, value_, less<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \cond */
template<typename iterator_t, typename value_t, typename compare_t>
inline iterator_t upper_bound( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_, hcore::iterator_category::forward ) {
	iterator_t it( first_ );
	iterator_t mid( first_ );
	while ( it != last_ ) {
		++ it;
		if ( ! ( it != last_ ) ) {
			break;
		}
		++ mid;
		++ it;
	}
	if ( mid != first_ ) {
		if ( ! comp_( value_, *mid ) ) {
			mid = upper_bound( mid, last_, value_, comp_, hcore::iterator_category::forward() );
		}	else {
			mid = upper_bound( first_, mid, value_, comp_, hcore::iterator_category::forward() );
		}
	} else {
		if ( ( mid != last_ ) && ! comp_( value_, *mid ) ) {
			++ mid;
		}
	}
	return ( mid );
}
template<typename iterator_t, typename value_t, typename compare_t>
inline iterator_t upper_bound( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_, hcore::iterator_category::random_access ) {
	iterator_t mid( first_ + ( last_ - first_ ) / 2 );
	while ( mid != first_ ) {
		if ( ! comp_( value_, *mid ) ) {
			first_ = mid;
		}	else {
			last_ = mid;
		}
		mid = ( first_ + ( last_ - first_ ) / 2 );
	}
	if ( ( mid != last_ ) && ! comp_( value_, *mid ) ) {
		++ mid;
	}
	return ( mid );
}
/*! \endcond */

/*! \brief Find last position in ordered (where order is given) range where given value could be inserted without violating ordering.
 *
 * \pre Range first_ - last_ must be ordered in given order.
 *
 * \param first_ - beginning of ordered range to search through.
 * \param last_ - one past the end of ordered range to search through.
 * \param value_ - upper bound value.
 * \param comp_ - comparison operator that defines order.
 * \return Last position in range where value could be inserted without violating ordering.
 */
template<typename iterator_t, typename value_t, typename compare_t>
inline iterator_t upper_bound( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_ ) {
	return ( upper_bound( first_, last_, value_, comp_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
}

/*! \brief Find last position in ordered range where given value could be inserted without violating ordering.
 *
 * \pre Range first_ - last_ must be ordered.
 *
 * \param first_ - beginning of ordered range to search through.
 * \param last_ - one past the end of ordered range to search through.
 * \param value_ - upper bound value.
 * \return Last position in range where value could be inserted without violating ordering.
 */
template<typename iterator_t, typename value_t>
inline iterator_t upper_bound( iterator_t first_, iterator_t last_, value_t const& value_ ) {
	return ( upper_bound( first_, last_, value_, less<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \cond */
template<typename iterator_t, typename value_t, typename compare_t>
inline bool binary_search( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_, hcore::iterator_category::forward ) {
	bool found( false );
	iterator_t it( first_ );
	iterator_t mid( first_ );
	while ( it != last_ ) {
		++ it;
		if ( ! ( it != last_ ) ) {
			break;
		}
		++ mid;
		++ it;
	}
	if ( mid != first_ ) {
		if ( ! comp_( value_, *mid ) ) {
			if ( ! comp_( *mid, value_ ) ) {
				found = true;
			}	else {
				found = binary_search( mid, last_, value_, comp_, hcore::iterator_category::forward() );
			}
		} else {
			found = binary_search( first_, mid, value_, comp_, hcore::iterator_category::forward() );
		}
	} else {
		found = ( ! comp_( value_, *mid ) ) && ( ! comp_( *mid, value_ ) );
	}
	return ( found );
}
template<typename iterator_t, typename value_t, typename compare_t>
inline bool binary_search( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_, hcore::iterator_category::random_access ) {
	bool found( false );
	iterator_t mid( first_ + ( last_ - first_ ) / 2 );
	while ( ! found && ( mid != first_ ) ) {
		if ( ! comp_( value_, *mid ) ) {
			if ( ! comp_( *mid, value_ ) ) {
				found = true;
				break;
			}
			first_ = mid;
		} else {
			last_ = mid;
		}
		mid = ( first_ + ( last_ - first_ ) / 2 );
	}
	if ( ! found ) {
		found = ( ! comp_( value_, *mid ) ) && ( ! comp_( *mid, value_ ) );
	}
	return ( found );
}
/*! \endcond */

/*! \brief Verify if Given value exists in ordered (where order is given) range.
 *
 * \param first_ - beginning of ordered range to search through.
 * \param last_ - one past the end of ordered range to search through.
 * \param value_ - value to look for in ordered range.
 * \param comp_ - comparator operator that defines order.
 * \return True iff given value is present in given ordered range.
 */
template<typename iterator_t, typename value_t, typename compare_t>
inline bool binary_search( iterator_t first_, iterator_t last_, value_t const& value_, compare_t comp_ ) {
	return ( binary_search( first_, last_, value_, comp_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
}

/*! \brief Verify if Given value exists in ordered range.
 *
 * \param first_ - beginning of ordered range to search through.
 * \param last_ - one past the end of ordered range to search through.
 * \param value_ - value to look for in ordered range.
 * \return True iff given value is present in given ordered range.
 */
template<typename iterator_t, typename value_t>
inline bool binary_search( iterator_t first_, iterator_t last_, value_t const& value_ ) {
	return ( binary_search( first_, last_, value_, less<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \brief Swap contents (values) of two ranges of elements.
 *
 * \param first - beginning of first range.
 * \param end - one past the end of first range.
 * \param second - beginning of second range.
 */
template<typename iterator_t>
inline void swap_ranges( iterator_t first, iterator_t const& end, iterator_t second ) {
	if ( first != second ) {
		for ( ; first != end; ++ first, ++ second ) {
			swap( *first, *second );
		}
	}
	return;
}
template<typename first_it_t, typename second_it_t>
inline void swap_ranges( first_it_t first, first_it_t const& end, second_it_t second ) {
	for ( ; first != end; ++ first, ++ second ) {
		swap( *first, *second );
	}
	return;
}

/*! \brief Joins two sorted ranges of elements into one sorted range of elements.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 * \param comp_ - comparison operator.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t, typename compare_t>
inline iter_out_t merge( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out, compare_t comp_ ) {
	for ( ; it1 != end1; ++ it1, ++ out ) {
		for ( ; ( it2 != end2 ) && comp_( *it2, *it1 ); ++ it2, ++ out ) {
			*out = *it2;
		}
		*out = *it1;
	}
	return ( copy( it2, end2, out ) );
}

/*! \brief Joins two sorted ranges of elements into one sorted range of elements.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
inline iter_out_t merge( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out ) {
	return ( merge( it1, end1, it2, end2, out, less<typename hcore::iterator_traits<iter_in1_t>::value_type>() ) );
}

/*! \cond */
template<typename iterator_t, typename compare_t>
inline void inplace_merge_impl( iterator_t first_, iterator_t mid_, iterator_t last_, compare_t comp_ ) {
	iterator_t it( mid_ );
	while ( ( first_ != it ) && ( it != last_ ) ) {
		while ( ( first_ != it ) && ! comp_( *it, *first_ ) ) {
			++ first_;
		}
		while ( ( it != last_ ) && comp_( *it, *first_ ) ) {
			++ it;
		}
		first_ = rotate( first_, mid_, it );
		mid_ = it;
		if ( it == last_ ) {
			it = mid_;
		}	else if ( first_ == mid_ ) {
			mid_ = it;
		}
	}
	return;
}
template<typename iterator_t, typename compare_t>
inline void inplace_merge_impl( iterator_t first_, iterator_t mid_, iterator_t last_, compare_t comp_,
	hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iterator_t>::value_type>& aux_ ) {
	typedef hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iterator_t>::value_type> aux_t;
	if ( aux_.get_size() > 0 ) {
		if ( aux_.get_size() == aux_.get_requested_size() ) {
			merge( aux_.begin(), aux_.end(), mid_, last_, first_, comp_ );
		}	else {
			typename aux_t::value_type* out( aux_.begin() );
			typename aux_t::value_type* end( aux_.end() );
			iterator_t it1( first_ );
			iterator_t it2( mid_ );
			for ( ; it1 != mid_; ++ it1, ++ out ) {
				for ( ; ( it2 != last_ ) && comp_( *it2, *it1 ); ++ it2, ++ out ) {
					if ( out == end ) {
						it1 = copy_backward( it1, mid_, it2 );
						mid_ = it2;
						swap_ranges( aux_.begin(), end, first_ );
						first_ = it1;
						out = aux_.begin();
					}
					*out = *it2;
				}
				if ( out == end ) {
					it1 = copy_backward( it1, mid_, it2 );
					mid_ = it2;
					swap_ranges( aux_.begin(), end, first_ );
					first_ = it1;
					out = aux_.begin();
				}
				*out = *it1;
			}
			swap_ranges( aux_.begin(), out, first_ );
		}
	} else {
		inplace_merge_impl( first_, mid_, last_, comp_ );
	}
	return;
}
/*! \endcond */

/*! \brief Merges in place two consecutive sorted ranges of elements into one sorted range of elements.
 *
 * \param first_ - beginning of first range.
 * \param mid_ - one past the end of first range and beginning of second range.
 * \param last_ - end of second range.
 * \param comp_ - comparison operator.
 */
template<typename iterator_t, typename compare_t>
inline void inplace_merge( iterator_t first_, iterator_t mid_, iterator_t last_, compare_t comp_ ) {
	typedef typename hcore::iterator_traits<iterator_t>::value_type value_t;
	typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
	aux_t aux( first_, mid_ );
	if ( aux.get_size() > 0 ) {
		inplace_merge_impl( first_, mid_, last_, comp_, aux );
	}	else {
		inplace_merge_impl( first_, mid_, last_, comp_ );
	}
	return;
}

/*! \brief Merges in place two consecutive sorted ranges of elements into one sorted range of elements.
 *
 * \param first_ - beginning of first range.
 * \param mid_ - one past the end of first range and beginning of second range.
 * \param last_ - end of second range.
 */
template<typename iterator_t>
inline void inplace_merge( iterator_t first_, iterator_t mid_, iterator_t last_ ) {
	inplace_merge( first_, mid_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \brief Create union of two sorted ranges of elements.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
inline iter_out_t set_union( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out ) {
	while ( ( it1 != end1 ) || ( it2 != end2 ) ) {
		if ( it1 != end1 ) {
			if ( it2 != end2 ) {
				if ( *it1 < *it2 ) {
					*out = *it1;
					++ it1;
				} else if ( *it2 < *it1 ) {
					*out = *it2;
					++ it2;
				} else {
					*out = *it1;
					++ it1;
					++ it2;
				}
			} else {
				*out = *it1;
				++ it1;
			}
		} else if ( it2 != end2 ) {
			*out = *it2;
			++ it2;
		}
		++ out;
	}
	return ( out );
}

/*! \brief Create intersection of two sorted ranges of elements.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 * \return One past end iterator for the output range.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
inline iter_out_t set_intersection( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out ) {
	while ( ( it1 != end1 ) && ( it2 != end2 ) ) {
		if ( *it1 < *it2 ) {
			++ it1;
			continue;
		}
		if ( *it2 < *it1 ) {
			++ it2;
			continue;
		}
		*out = *it1;
		++ out;
		++ it1;
		++ it2;
	}
	return ( out );
}

/*! \brief Check if given two sorted ranges intersect.
 *
 * \param it1 - beginning of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - beginning of second range.
 * \param end2 - one past the end of second range.
 * \return True iff two ranges do intersect.
 */
template<typename iter_in1_t, typename iter_in2_t>
inline bool does_intersect( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2 ) {
	bool intersect( false );
	while ( ( it1 != end1 ) && ( it2 != end2 ) ) {
		if ( *it1 < *it2 ) {
			++ it1;
			continue;
		}
		if ( *it2 < *it1 ) {
			++ it2;
			continue;
		}
		intersect = true;
		break;
	}
	return ( intersect );
}

/*! \brief Fill range of elements with given value.
 *
 * \param it - beginning of range to fill.
 * \param end - one past the end of range.
 * \param filler - value to be set for all elements in range.
 */
template<typename dst_it_t, typename filler_t>
inline void fill( dst_it_t it, dst_it_t end, filler_t const& filler ) {
	for ( ; it != end; ++ it ) {
		*it = filler;
	}
	return;
}

/*! \brief Fill specified amount of elements with given value.
 *
 * \param it - beginning of range to fill.
 * \param count - amount of elements to set.
 * \param filler - value to be set for all elements in range.
 */
template<typename dst_it_t, typename filler_t>
inline void fill_n( dst_it_t it, int long count, filler_t const& filler ) {
	for ( int long i = 0; i < count; ++ i, ++ it ) {
		*it = filler;
	}
	return;
}

/*! \brief Fill specified range with run-time generated values.
 *
 * \param it - beginning of the range.
 * \param end - one past last element in range.
 * \param generator - function object that creates new values for range.
 */
template<typename iterator_t, typename generator_t>
inline void generate( iterator_t it, iterator_t const& end, generator_t generator ) {
	for ( ; it != end; ++ it ) {
		*it = static_cast<typename hcore::iterator_traits<iterator_t>::value_type>( generator() );
	}
	return;
}

/*! \brief Fill specified range with n run-time generated values.
 *
 * \param it - beginning of the container.
 * \param count - number of the elements to be generated.
 * \param generator - function object that creates new values for range.
 */
template<typename iterator_t, typename generator_t>
inline void generate_n( iterator_t it, int long count, generator_t generator ) {
	for ( int long i = 0; i < count; ++ i, ++ it ) {
		*it = static_cast<typename hcore::iterator_traits<iterator_t>::value_type>( generator() );
	}
	return;
}

/*! \brief Reverses order of elements in range.
 *
 * \param it - beginning of range of elements to be reverted.
 * \param end - one past last element of range to be reverted.
 */
template<typename iterator_t>
inline void reverse( iterator_t it, iterator_t end ) {
	if ( it != end ) {
		-- end;
		while ( it != end ) {
			swap( *it, *end );
			++ it;
			if ( it != end ) {
				-- end;
			}	else {
				break;
			}
		}
	}
	return;
}

/*! \brief Generate next, in lexicographical order, permutation of the range of elements.
 *
 * \param it - beginning of range of elements for permutation.
 * \param end - one past last element of range for permutation.
 * \return true iff last, in lexicographical order, permutation has been generated.
 */
template<typename iterator_t>
inline bool next_permutation( iterator_t it, iterator_t end ) {
	int long count = 0;
	iterator_t itLast;
	for ( iterator_t itEnd = it; itEnd != end; ++ itEnd, ++ count ) {
		itLast = itEnd;
	}
	bool gotNext = false;
	if ( count > 1 ) {
		do {
			iterator_t i = itLast;
			-- i;
			-- count;
			iterator_t ie = itLast;
			while ( *i >= *ie ) {
				-- count;
				if ( ! count ) {
					break;
				}
				-- i, -- ie;
			}
			if ( ! count ) {
				break;
			}
			iterator_t j = itLast;
			while ( *j <= *i ) {
				-- j;
			}
			swap( *i, *j );
			++ i;
			reverse( i, end );
			gotNext = true;
		} while ( 0 );
		if ( ! gotNext ) {
			reverse( it, end );
		}
	}
	return ( gotNext );
}

/*! \brief Generate previous, in lexicographical order, permutation of the range of elements.
 *
 * \param it - beginning of range of elements for permutation.
 * \param end - one past last element of range for permutation.
 * \return true iff first, in lexicographical order, permutation has been generated.
 */
template<typename iterator_t>
inline bool prev_permutation( iterator_t it, iterator_t end ) {
	int long count = 0;
	iterator_t itLast;
	for ( iterator_t itEnd = it; itEnd != end; ++ itEnd, ++ count ) {
		itLast = itEnd;
	}
	bool gotPrev = false;
	if ( count > 1 ) {
		do {
			iterator_t i = itLast;
			-- i;
			-- count;
			iterator_t ie = itLast;
			while ( *i <= *ie ) {
				-- count;
				if ( ! count ) {
					break;
				}
				-- i, -- ie;
			}
			if ( ! count ) {
				break;
			}
			iterator_t j = itLast;
			while ( *j >= *i ) {
				-- j;
			}
			swap( *i, *j );
			++ i;
			reverse( i, end );
			gotPrev = true;
		} while ( 0 );
		if ( ! gotPrev ) {
			reverse( it, end );
		}
	}
	return ( gotPrev );
}

/*! \brief Calculate sum of elements in range.
 *
 * \param it - beginning of range of elements to summed up.
 * \param end - one past last element of range to be summed up.
 * \param ret - starting value for sum operation.
 * \return Sum of all elements in [it, end) + ret.
 */
template<typename iterator_t, typename return_t>
inline return_t accumulate( iterator_t it, iterator_t end, return_t ret ) {
	for ( ; it != end; ++ it ) {
		ret += *it;
	}
	return ( ret );
}

/*! \brief Calculate generalized sum of elements in range.
 *
 * \param it - beginning of range of elements to summed up.
 * \param end - one past last element of range to be summed up.
 * \param ret - starting value for sum operation.
 * \param oper - operator used to calculate generalized sum.
 * \return Generalized sum of all elements in [it, end) + ret.
 */
template<typename iterator_t, typename return_t, typename operator_t>
inline return_t accumulate( iterator_t it, iterator_t end, return_t ret, operator_t oper ) {
	for ( ; it != end; ++ it ) {
		ret = oper( ret, *it );
	}
	return ( ret );
}

/*! \brief Calculate inner product of elements in given two ranges of equal length.
 *
 * Calculate inner product of two ranges as sum of products of respective pairs
 * of elements from both ranges.
 *
 * \param itLeft - beginning of first range of elements to calculate inner product.
 * \param endLeft - one past last element of first range to calculate inner product.
 * \param itRight - beginning of second range of elements to calculate inner product.
 * \param endRight - one past last element of second range to calculate inner product.
 * \param ret - starting value for sum operation.
 * \return Inner product of all elements in ranges [itLeft, endLeft) and [itRight, endRight) + ret.
 */
template<typename iter_left_t, typename iter_right_t, typename return_t>
inline return_t inner_product( iter_left_t itLeft, iter_left_t endLeft,
	iter_right_t itRight, return_t ret ) {
	for ( ; itLeft != endLeft; ++ itLeft, ++ itRight ) {
		ret += ( ( *itLeft ) * ( *itRight ) );
	}
	return ( ret );
}

/*! \brief Calculate generalized inner product of elements in given two ranges of equal length.
 *
 * Calculate generalized inner product of two ranges
 * as generalized sum of generalized products of respective pairs
 * of elements from both ranges.
 *
 * \param itLeft - beginning of first range of elements to calculate inner product.
 * \param endLeft - one past last element of first range to calculate inner product.
 * \param itRight - beginning of second range of elements to calculate inner product.
 * \param endRight - one past last element of second range to calculate inner product.
 * \param ret - starting value for sum operation.
 * \param summator - generalized sum operator.
 * \param multiplicator - generalized multiplication operator.
 * \return Generalized inner product of all elements in ranges [itLeft, endLeft) and [itRight, endRight) + ret.
 */
template<typename iter_left_t, typename iter_right_t, typename return_t,
	typename summator_t, typename multiplicator_t>
inline return_t inner_product( iter_left_t itLeft, iter_left_t endLeft,
	iter_right_t itRight, return_t ret,
	summator_t summator, multiplicator_t multiplicator ) {
	for ( ; itLeft != endLeft; ++ itLeft, ++ itRight ) {
		ret = summator( ret, multiplicator( *itLeft, *itRight ) );
	}
	return ( ret );
}

/*! \brief Find minimum element in a range.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t>
inline iterator_t min_element( iterator_t it, iterator_t end ) {
	iterator_t min( it );
	++ it;
	for ( ; ( it != end ); ++ it ) {
		if ( *it < *min ) {
			min = it;
		}
	}
	return ( min );
}

/*! \brief Find element in a range that is best with respect to some predicate.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param pred - predicate to test.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t, typename predicate_t>
inline iterator_t min_element( iterator_t it, iterator_t end, predicate_t predicate_ ) {
	iterator_t min( it );
	++ it;
	for ( ; ( it != end ); ++ it ) {
		if ( predicate_( *it, *min ) ) {
			min = it;
		}
	}
	return ( min );
}

template<typename tType, typename predicate_t>
tType min( std::initializer_list<tType> constants_, predicate_t predicate_ ) {
	return ( *min_element( constants_.begin(), constants_.end(), predicate_ ) );
}

template<typename tType>
tType min( std::initializer_list<tType> constants_ ) {
	return ( *min_element( constants_.begin(), constants_.end() ) );
}

/*! \brief Find maximum element in a range.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t>
inline iterator_t max_element( iterator_t it, iterator_t end ) {
	iterator_t max( it );
	++ it;
	for ( ; ( it != end ); ++ it ) {
		if ( *it > *max ) {
			max = it;
		}
	}
	return ( max );
}

/*! \brief Find element in a range that is best with respect to some predicate.
 *
 * \param it - beginning of the range to search through.
 * \param end - one past the end of the range to search through.
 * \param pred - predicate to test.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t, typename predicate_t>
inline iterator_t max_element( iterator_t it, iterator_t end, predicate_t predicate_ ) {
	iterator_t max( it );
	++ it;
	for ( ; ( it != end ); ++ it ) {
		if ( predicate_( *max, *it ) ) {
			max = it;
		}
	}
	return ( max );
}

template<typename tType, typename predicate_t>
tType max( std::initializer_list<tType> constants_, predicate_t predicate_ ) {
	return ( *max_element( constants_.begin(), constants_.end(), predicate_ ) );
}

template<typename tType>
tType max( std::initializer_list<tType> constants_ ) {
	return ( *max_element( constants_.begin(), constants_.end() ) );
}

/*! \brief Make heap from range of elements.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparison operator.
 */
template<typename iterator_t, typename compare_t>
inline void make_heap( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	int long size( last_ - first_ );
	for ( int long i( ( size / 2 ) - 1 ); i >= 0; -- i ) {
		int long pos( i );
		while ( pos < size ) {
			int long left( ( pos * 2 ) + 1 );
			if ( left < size ) {
				int long right( ( pos * 2 ) + 2 );
				int long child( right < size ? ( comp_( *( first_ + left ), *( first_ + right ) ) ? right : left ) : left );
				iterator_t fp( first_ + pos );
				iterator_t fc( first_ + child );
				if ( comp_( *fp, *fc ) ) {
					swap( *fp, *fc );
				} else {
					break;
				}
				pos = child;
			} else {
				break;
			}
		}
	}
	return;
}

/*! \brief Make heap from range of elements.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iterator_t>
inline void make_heap( iterator_t first_, iterator_t last_ ) {
	make_heap( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \brief Add element to the heap.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap and new element at the same time.
 * \param comp_ - comparison operator.
 */
template<typename iterator_t, typename compare_t>
inline void push_heap( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	int long pos( ( last_ - first_ ) - 1 ); /* zero based position of inserted element */
	while ( ( pos > 0 ) && comp_( *( first_ + ( ( pos - 1 ) / 2 ) ), *( first_ + pos ) ) ) {
		swap( *( first_ + ( ( pos - 1 ) / 2 ) ), *( first_ + pos ) );
		pos = ( pos - 1 ) / 2;
	}
	return;
}

/*! \brief Add element to the heap.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap and new element at the same time.
 */
template<typename iterator_t>
inline void push_heap( iterator_t first_, iterator_t last_ ) {
	push_heap( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \brief Retrieve top element from the heap.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparison operator.
 */
template<typename iterator_t, typename compare_t>
inline void pop_heap( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	int long size( ( last_ - first_ ) - 1 );
	if ( size > 0 ) {
		swap( *first_, *( last_ - 1 ) );
		int long pos( 0 );
		while ( pos < size ) {
			int long left( ( pos * 2 ) + 1 );
			if ( left < size ) {
				int long right( ( pos * 2 ) + 2 );
				int long child( right < size ? ( comp_( *( first_ + left ), *( first_ + right ) ) ? right : left ) : left );
				iterator_t fp( first_ + pos );
				iterator_t fc( first_ + child );
				if ( comp_( *fp, *fc ) ) {
					swap( *fp, *fc );
				} else {
					break;
				}
				pos = child;
			} else {
				break;
			}
		}
	}
	return;
}

/*! \brief Retrieve top element from the heap.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iterator_t>
inline void pop_heap( iterator_t first_, iterator_t last_ ) {
	pop_heap( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \brief Test if given range of elements forms a heap.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparison operator.
 */
template<typename iterator_t, typename compare_t>
inline bool is_heap( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	bool isHeap( true );
	int long size( last_ - first_ );

	int long children( 0 );

	while ( isHeap && ( children < size ) ) {
		int long roots( children );
		children = ( children * 2 ) + 1;
		for ( int long r( roots ), c( children ); ( r < children ) && ( c < size ); ++ r, ++ c ) {
			if ( comp_( *( first_ + r ), *( first_ + c ) ) ) {
				isHeap = false;
				break;
			}
			++ c;
			if ( ( c < size ) && comp_( *( first_ + r ), *( first_ + c ) ) ) {
				isHeap = false;
				break;
			}
		}
	}
	return ( isHeap );
}

/*! \brief Test if given range of elements forms a heap.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iterator_t>
inline bool is_heap( iterator_t first_, iterator_t last_ ) {
	return ( yaal::is_heap( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \brief Turn range of elements that is a heap into a sorted range.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparison operator.
 */
template<typename iterator_t, typename compare_t>
inline void sort_heap( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	if ( first_ != last_ ) {
		for ( iterator_t first( first_ + 1 ); last_ != first; -- last_ ) {
			pop_heap( first_, last_, comp_ );
		}
	}
	return;
}

/*! \brief Turn range of elements that is a heap into a sorted range.
 *
 * \param first_ - beginning of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iterator_t>
inline void sort_heap( iterator_t first_, iterator_t last_ ) {
	sort_heap( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \brief Sort range of elements (sorting algorithm is unstable with worst case complexity guarantee O(n*ln(n)) ).
 *
 * \param first_ - beginning of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 * \param comp_ - comparison operator used for sorting.
 */
template<typename iterator_t, typename compare_t>
inline void heap_sort( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	if ( first_ != last_ ) {
		make_heap( first_, last_, comp_ );
		sort_heap( first_, last_, comp_ );
	}
	return;
}

/*! \brief Sort range of elements (sorting algorithm is unstable with worst case complexity guarantee O(n*ln(n)) ).
 *
 * \param first_ - beginning of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 */
template<typename iterator_t>
inline void heap_sort( iterator_t first_, iterator_t last_ ) {
	heap_sort( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \cond */
template<typename iterator_t, typename compare_t>
inline void insert_sort( iterator_t first_, iterator_t last_, compare_t comp_, hcore::iterator_category::forward ) {
	typedef typename hcore::iterator_traits<iterator_t>::value_type value_t;
	iterator_t it( first_ );
	++ it;
	while ( it != last_ ) {
		iterator_t next( it );
		++ next;
		iterator_t sorted( first_ );
		while ( ( sorted != it ) && ! comp_( *it, *sorted ) ) {
			++ sorted;
		}
		if ( sorted != it ) {
			value_t tmp( *it );
			copy_backward( sorted, it, next );
			*sorted = tmp;
		}
		++ it;
	}
	return;
}
template<typename iterator_t, typename compare_t>
inline void insert_sort( iterator_t first_, iterator_t last_, compare_t comp_, hcore::iterator_category::random_access ) {
	typedef typename hcore::iterator_traits<iterator_t>::value_type value_t;
	iterator_t it( first_ );
	++ it;
	while ( it != last_ ) {
		int long dist( ( it - first_ ) / 2 );
		iterator_t sorted( first_ + dist );
		while ( dist > 1 ) {
			dist >>= 1;
			if ( comp_( *it, *sorted ) ) {
				sorted -= ( dist & 1 ? dist + 1 : dist );
			} else {
				sorted += dist;
			}
		}
		if ( ( sorted != first_ ) && comp_( *it, *sorted ) ) {
			-- sorted;
		}
		while ( ( sorted != it ) && ! comp_( *it, *sorted ) ) {
			++ sorted;
		}
		if ( sorted != it ) {
			value_t tmp( *it );
			copy_backward( sorted, it, it + 1 );
			*sorted = tmp;
		}
		++ it;
	}
	return;
}
/*! \endcond */

/*! \cond */
template<typename iterator_t, typename compare_t>
inline void selection_sort( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	for ( ; first_ != last_; ++ first_ ) {
		iterator_t min( min_element( first_, last_, comp_ ) );
		if ( min != first_ ) {
			swap( *min, *first_ );
		}
	}
}
/*! \endcond */

namespace {
static int const YAAL_MERGE_SORT_ALGO_THRESHOLD = 16;
static int const YAAL_QUICK_SORT_ALGO_THRESHOLD = 8;
}

/*! \cond */
template<typename iterator_t, typename compare_t>
inline void stable_sort_impl( iterator_t first_, iterator_t last_, compare_t comp_,
	hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iterator_t>::value_type>& aux_ ) {
	using yaal::distance;
	int long size( distance( first_, last_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
	if ( size < YAAL_MERGE_SORT_ALGO_THRESHOLD ) {
		insert_sort( first_, last_, comp_, typename hcore::iterator_traits<iterator_t>::category_type() );
	} else {
		iterator_t mid( first_ );
		using yaal::advance;
		advance( mid, size / 2, typename hcore::iterator_traits<iterator_t>::category_type() );
		stable_sort_impl( first_, mid, comp_, aux_ );
		stable_sort_impl( mid, last_, comp_, aux_ );
		aux_.init( first_, mid );
		inplace_merge_impl( first_, mid, last_, comp_, aux_ );
	}
	return;
}
/*! \endcond */

/*! \brief Perform stable sort of range of elements (sorting algorithm is unstable).
 *
 * \param first_ - beginning of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 * \param comp_ - comparison operator used for sorting.
 */
template<typename iterator_t, typename compare_t>
inline void stable_sort( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	using yaal::distance;
	if ( first_ != last_ ) {
		int long size( distance( first_, last_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
		if ( size < YAAL_MERGE_SORT_ALGO_THRESHOLD ) {
			insert_sort( first_, last_, comp_, typename hcore::iterator_traits<iterator_t>::category_type() );
		} else {
			iterator_t mid( first_ );
			using yaal::advance;
			advance( mid, size / 2, typename hcore::iterator_traits<iterator_t>::category_type() );
			typedef typename hcore::iterator_traits<iterator_t>::value_type value_t;
			typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
			aux_t aux( first_, mid );
			stable_sort_impl( first_, mid, comp_, aux );
			stable_sort_impl( mid, last_, comp_, aux );
			aux.init( first_, mid );
			inplace_merge_impl( first_, mid, last_, comp_, aux );
		}
	}
	return;
}

/*! \brief Perform stable sort of range of elements (sorting algorithm is unstable).
 *
 * \param first_ - beginning of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 */
template<typename iterator_t>
inline void stable_sort( iterator_t first_, iterator_t last_ ) {
	stable_sort( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \cond */
/* naive */
template<typename iterator_t, typename compare_t>
inline iterator_t choose_pivot( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	iterator_t mid( first_ + ( last_ - first_ ) / 2 );
	if ( comp_( *first_, *last_ ) ) {
		if ( comp_( *first_, *mid ) ) {
			if ( comp_( *last_, *mid ) ) {
				mid = last_;
			}
		} else {
			mid = first_;
		}
	} else {
		if ( comp_( *last_, *mid ) ) {
			if ( comp_( *first_, *mid ) ) {
				mid = first_;
			}
		} else {
			mid = last_;
		}
	}
	return ( mid );
}
/*! \endcond */

/*! \brief Partition range of elements according to specified predicate.
 *
 * \param first_ - beginning of the range to partition.
 * \param last_ - one past the end of the range to partition.
 * \param predicate_ - a predicate according to which partitioning shall be performed.
 * \return Iterator m for which all i in [first_, m) predicate_( *i ) is true and for all i in [m, last_) predicate_( *i ) is false.
 */
template<typename iterator_t, typename predicate_t>
inline iterator_t partition( iterator_t first_, iterator_t last_, predicate_t predicate_ ) {
	first_ = find_if_not( first_, last_, predicate_ );
	iterator_t fit( first_ );
	if ( first_ != last_ ) {
		for ( ++ first_; first_ != last_; ++ first_ ) {
			if ( predicate_( *first_ ) ) {
				using yaal::swap;
				swap( *first_, *fit );
				++ fit;
			}
		}
	}
	return ( fit );
}

template<typename iterator_t, typename compare_t>
inline iterator_t partition_by_pivot(
	iterator_t first_,
	iterator_t last_,
	compare_t const& compare_,
	typename hcore::iterator_traits<iterator_t>::value_type const& pivot_,
	bool& leftConstant_
) {
	if ( first_ == last_ ) {
		return ( first_ );
	}
	iterator_t l( first_ );
	iterator_t r( last_ - 1 );
	using yaal::swap;
	while ( l != r ) {
		for ( ; ( l != r ) && compare_( *l, pivot_ ); ++ l ) {
		}
		for ( ; ( r != l ) && ! compare_( *r, pivot_ ); -- r ) {
		}
		if ( l != r ) {
			swap( *l, *r );
			++ l;
			if ( l != r ) {
				-- r;
			}	else {
				break;
			}
		} else {
			break;
		}
	}
	for ( ; ( l != last_ ) && compare_( *l, pivot_ ); ++ l ) {
	}
	leftConstant_ = false;
	if ( ! ( l != first_ ) ) {
		/* if l == first_ it means that pivot_ is minimum of a set
		 */
		leftConstant_ = true;
		for ( r = l; r != last_; ++ r ) {
			if ( ( ! compare_( pivot_, *r ) ) && ( r != l ) ) {
				swap( *r, *l );
				++ l;
			}
		}
	}
	return ( l );
}

/*! \brief Reorder range of elements so element at given position would be preserved if range were to be sorted.
 *
 * \param first_ - beginning of the range to reorder.
 * \param nth_ - element at this position shall acquire ordered position.
 * \param last_ - one past the end of the range to reorder.
 * \param comp_ - comparison operator used for reordering.
 */
template<typename iterator_t, typename compare_t>
inline void nth_element( iterator_t first_, iterator_t nth_, iterator_t last_, compare_t comp_ ) {
	while ( true ) {
		iterator_t pivotIt( choose_pivot( first_, last_ - 1, comp_ ) );
		using yaal::swap;
		swap( *pivotIt, *( last_ - 1 ) );
		bool leftConstant( false );
		pivotIt = partition_by_pivot( first_, last_ - 1, comp_, *( last_ - 1 ), leftConstant );
		swap( *pivotIt, *( last_ - 1 ) );
		if ( pivotIt == nth_ ) {
			break;
		}
		if ( pivotIt < nth_ ) {
			first_ = pivotIt + 1;
		} else if ( leftConstant ) {
			break;
		} else {
			last_ = pivotIt;
		}
	}
	return;
}

/*! \brief Reorder range of elements so element at given position would be preserved if range were to be sorted.
 *
 * \param first_ - beginning of the range to reorder.
 * \param nth_ - element at this position shall acquire ordered position.
 * \param last_ - one past the end of the range to reorder.
 */
template<typename iterator_t>
inline void nth_element( iterator_t first_, iterator_t nth_, iterator_t last_ ) {
	nth_element( first_, nth_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \cond */
template<typename iterator_t, typename predicate_t>
inline iterator_t stable_partition_impl( iterator_t first_, iterator_t last_, predicate_t predicate_ ) {
	using yaal::swap;
	for ( ; ( first_ != last_ ) && predicate_( *first_ ); ++ first_ ) {
	}
	iterator_t unfit( first_ );
	while ( first_ != last_ ) {
		for ( ; ( first_ != last_ ) && ! predicate_( *first_ ); ++ first_ ) {
		}
		iterator_t fit( first_ );
		for ( ; ( first_ != last_ ) && predicate_( *first_ ); ++ first_ ) {
		}
		if ( fit != last_ ) {
			unfit = rotate( unfit, fit, first_ );
		}
	}
	return ( unfit );
}
/*
 * Adaptive algorithm:
 * 1. Perform partition on sub-range as if auxiliary buffer were sufficient
 * for full partitioning.
 * 2. Remember boundary iterators (also partition boundary).
 * 3. Perform partition on next sub-range as if auxiliary buffer were sufficient.
 * 4. Merge two consecutive partitioned sub-ranges by rotation.
 * 5. Repeat steps 2, 3, 4 until whole range has been processed.
 */
template<typename iterator_t, typename predicate_t>
inline iterator_t stable_partition_impl( iterator_t first_, iterator_t last_, predicate_t predicate_,
	hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iterator_t>::value_type>& aux_ ) {
	typedef typename hcore::iterator_traits<iterator_t>::value_type value_t;
	value_t* aux( aux_.begin() );
	value_t* auxEnd( aux_.end() );
	iterator_t it( first_ );
	iterator_t fit( first_ );
	iterator_t unfit( last_ );
	while ( first_ != last_ ) {
		if ( predicate_( *first_ ) ) {
			*it = *first_;
			++ it;
			++ first_;
		} else {
			if ( aux != auxEnd ) {
				*aux = *first_;
				++ aux;
				++ first_;
			} else {
				aux = aux_.begin();
				copy( aux, auxEnd, it );
				if ( unfit != last_ ) {
					unfit = rotate( unfit, fit, it );
				} else {
					unfit = it;
				}
				while ( ( first_ != last_ ) && ! predicate_( *first_ ) ) {
					++ first_;
				}
				fit = it = first_;
				if ( first_ != last_ ) {
					*it = *first_;
					++ it;
					++ first_;
				}
			}
		}
	}
	copy( aux_.begin(), aux, it );
	if ( unfit != last_ ) {
		unfit = rotate( unfit, fit, it );
	}	else {
		unfit = it;
	}
	return ( unfit );
}
/*! \endcond */

/*! \brief Partition range of elements according to specified predicate preserving relative order of elements.
 *
 * \param first_ - beginning of the range to partition.
 * \param last_ - one past the end of the range to partition.
 * \param predicate_ - a predicate according to which partitioning shall be performed.
 * \return Iterator m for which all i in [first_, m) predicate_( *i ) is true and for all i in [m, last_) predicate_( *i ) is false.
 */
template<typename iterator_t, typename predicate_t>
inline iterator_t stable_partition( iterator_t first_, iterator_t last_, predicate_t predicate_ ) {
	typedef typename hcore::iterator_traits<iterator_t>::value_type value_t;
	typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
	iterator_t it( first_ );
	if ( first_ != last_ ) {
		aux_t aux( first_, last_ );
		if ( aux.get_size() > 0 ) {
			it = stable_partition_impl( first_, last_, predicate_, aux );
		} else {
			it = stable_partition_impl( first_, last_, predicate_ );
		}
	}
	return ( it );
}

/*! \brief Sort range of elements (sorting algorithm is unstable with worst case complexity O(n^2) ).
 *
 * \param first_ - beginning of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 * \param comp_ - comparison operator used for sorting.
 */
template<typename iterator_t, typename compare_t>
inline void sort( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	using yaal::distance;
	if ( first_ != last_ ) {
		int long size( distance( first_, last_, typename hcore::iterator_traits<iterator_t>::category_type() ) );
		if ( size < YAAL_QUICK_SORT_ALGO_THRESHOLD ) {
			insert_sort( first_, last_, comp_, typename hcore::iterator_traits<iterator_t>::category_type() );
		} else {
			typename hcore::iterator_traits<iterator_t>::value_type pivot( *choose_pivot( first_, last_ - 1, comp_ ) );
			bool leftConstant( false );
			iterator_t it( partition_by_pivot( first_, last_, comp_, pivot, leftConstant ) );
			if ( it != last_ ) {
				if ( ! leftConstant ) {
					sort( first_, it, comp_ );
				}
				sort( it, last_, comp_ );
			}
		}
	}
	return;
}

/*! \brief Sort range of elements (sorting algorithm is unstable with worst case complexity O(n^2) ).
 *
 * \param first_ - beginning of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 */
template<typename iterator_t>
inline void sort( iterator_t first_, iterator_t last_ ) {
	sort( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() );
	return;
}

/*! \brief Check is given range is sorted according to given ordering.
 *
 * \param first_ - beginning of the range to test.
 * \param last_ - one past the end of the range to test.
 * \param comp_ - definition of the ordering.
 * \return True iff given range is sorted according to given ordering.
 */
template<typename iterator_t, typename compare_t>
inline bool is_sorted( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	iterator_t it( first_ );
	bool sorted( true );
	if ( it != last_ ) {
		++ it;
		for ( ; it != last_; ++ it, ++ first_ ) {
			if ( comp_( *it, *first_ ) ) {
				sorted = false;
				break;
			}
		}
	}
	return ( sorted );
}

/*! \brief Check is given range is sorted according to given ordering.
 *
 * \param first_ - beginning of the range to test.
 * \param last_ - one past the end of the range to test.
 * \return True iff given range is sorted according to "<" ordering.
 */
template<typename iterator_t>
inline bool is_sorted( iterator_t first_, iterator_t last_ ) {
	return ( is_sorted( first_, last_, less<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param dest_ - beginning of destination range.
 * \param comp_ - uniqueness test predicate.
 */
template<typename iterator_t, typename iter_out_t, typename compare_t>
inline iterator_t unique_copy( iterator_t first_, iterator_t last_, iter_out_t dest_, compare_t comp_ ) {
	if ( first_ != last_ ) {
		*dest_ = *first_;
		++ first_;
		for ( ; first_ != last_; ++ first_ ) {
			while ( ( first_ != last_ ) && comp_( *first_, *dest_ ) ) {
				++ first_;
			}
			if ( first_ != last_ ) {
				++ dest_;
				*dest_ = *first_;
			} else {
				break;
			}
		}
		++ dest_;
	}
	return ( dest_ );
}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param dest_ - beginning of destination range.
 * \param comp_ - uniqueness test predicate.
 */
template<typename iterator_t, typename iter_out_t>
inline iterator_t unique_copy( iterator_t first_, iterator_t last_, iter_out_t dest_ ) {
	return ( unique_copy( first_, last_, dest_, equal_to<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param comp_ - uniqueness test predicate.
 */
template<typename iterator_t, typename compare_t>
inline iterator_t unique( iterator_t first_, iterator_t last_, compare_t comp_ ) {
	return ( unique_copy( first_, last_, first_, comp_ ) );
}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - beginning of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 */
template<typename iterator_t>
inline iterator_t unique( iterator_t first_, iterator_t last_ ) {
	return ( unique( first_, last_, equal_to<typename hcore::iterator_traits<iterator_t>::value_type>() ) );
}

/*! \brief Randomly shuffle elements in range using given function as source of randomness.
 *
 * \param first_ - beginning of range of elements to shuffle.
 * \param last_ - one past the end of range of elements to shuffle.
 * \param randomGenerator_ - random number generator used as source of randomness.
 */
template<typename iterator_t, typename generator_t>
inline void random_shuffle( iterator_t first_, iterator_t last_, generator_t randomGenerator_ ) {
	int long count( last_ - first_ );
	while ( count > 1 ) {
		int long idx( static_cast<int long>( randomGenerator_() % count ) );
		M_ASSERT( ( idx >= 0 ) && ( idx < count ) );
		-- count;
		using yaal::swap;
		if ( idx != count ) {
			swap( *( first_ + idx ), *( first_ + count ) );
		}
	}
	return;
}

/*! \brief Randomly shuffle elements in range.
 *
 * \param first_ - beginning of range of elements to shuffle.
 * \param last_ - one past the end of range of elements to shuffle.
 */
template<typename iterator_t>
inline void random_shuffle( iterator_t first_, iterator_t last_ ) {
	random_shuffle( first_, last_, random::rng_helper::make_random_number_generator() );
	return;
}

/*! \brief Randomly copy subset of elements from one range onto another range using given function as source of randomness.
 *
 * \param srcFirst_ - beginning of range of elements to copy from.
 * \param srcLast_ - one past the end of range of elements to copy from.
 * \param dstFirst_ - beginning of range of elements to copy to.
 * \param dstLast_ - one past the end of range of elements to copy to.
 * \param randomGenerator_ - random number generator used as source of randomness.
 */
template<typename source_iterator_t, typename destination_iterator_t, typename generator_t>
inline void random_sample( source_iterator_t srcFirst_, source_iterator_t srcLast_, destination_iterator_t dstFirst_, destination_iterator_t dstLast_, generator_t randomGenerator_ ) {
	source_iterator_t src( srcFirst_ );
	for ( destination_iterator_t it( dstFirst_ ); it != dstLast_; ++ it, ++ src ) {
		*it = *src;
	}
	int long dstCount( dstLast_ - dstFirst_ );
	int long srcTop( ( src - srcFirst_ ) + 1 );
	while ( src != srcLast_ ) {
		int long idx( static_cast<int long>( randomGenerator_() % srcTop ) );
		if ( idx < dstCount ) {
			*( dstFirst_ + idx ) = *src;
		}
		++ srcTop;
		++ src;
	}
	return;
}

/*! \brief Randomly copy subset of elements from one range onto another range.
 *
 * \param srcFirst_ - beginning of range of elements to copy from.
 * \param srcLast_ - one past the end of range of elements to copy from.
 * \param dstFirst_ - beginning of range of elements to copy to.
 * \param dstLast_ - one past the end of range of elements to copy to.
 * \param randomGenerator_ - random number generator used as source of randomness.
 */
template<typename source_iterator_t, typename destination_iterator_t>
inline void random_sample( source_iterator_t srcFirst_, source_iterator_t srcLast_, destination_iterator_t dstFirst_, destination_iterator_t dstLast_ ) {
	random_sample( srcFirst_, srcLast_, dstFirst_, dstLast_, random::rng_helper::make_random_number_generator() );
	return;
}

}

#endif /* #ifndef YAAL_HCORE_ALGORITHM_HXX_INCLUDED */

