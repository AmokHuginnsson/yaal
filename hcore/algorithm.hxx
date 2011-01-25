/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/algorithm.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/algorithm.hxx
 * \brief Basic alogrithms definitions.
 */

#ifndef YAAL_HCORE_ALGORITHM_HXX_INCLUDED
#define YAAL_HCORE_ALGORITHM_HXX_INCLUDED 1

#include "hcore/trait.hxx"
#include "hcore/hpair.hxx"
#include "hcore/iterator.hxx"
#include "hcore/functional.hxx"
#include "hcore/algorithm_impl.hxx"
#include "hcore/hauxiliarybuffer.hxx"

namespace yaal
{

/*! \brief Swap contents of two variables.
 *
 * \param left - first variable to be swapped.
 * \param right - second variable to be swapped.
 *
 * \post After the call left holds value of right from before call, and right holds value of left from before call.
 */
template<typename tType>
inline void swap( tType& left, tType& right )
	{
	if ( &left != &right )
		{
		tType tmp( left );
		left = right;
		right = tmp;
		}
	return;
	}

/*! \brief Find first occurence of given value in given range of elements.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \param v - look for this value in given range.
 * \return iterator pointing to found value or end of range.
 */
template<typename iterator_t, typename value_t>
iterator_t find( iterator_t it, iterator_t end, value_t const& v )
	{
	for ( ; ( it != end ) && ( *it != v ); ++ it )
		;
	return ( it );
	}

/*! \brief Find first element in range that meets a given condition.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \param cond - condition which must be satified.
 * \return iterator pointing to found element or end of range.
 */
template<typename iterator_t, typename condition_t>
iterator_t find_if( iterator_t it, iterator_t end, condition_t cond )
	{
	for ( ; ( it != end ) && ( ! cond( *it ) ); ++ it )
		;
	return ( it );
	}

/*! \brief Count number of occurences of element in range.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \param v - look for this value in given range.
 * \return Number of occurences of v in range.
 */
template<typename iterator_t, typename value_t>
int long count( iterator_t it, iterator_t end, value_t const& v )
	{
	int long counter( 0 );
	for ( ; it != end; ++ it )
		{
		if ( *it == v )
			++ counter;
		}
	return ( counter );
	}

/*! \brief Count number of element meeting a predicate in range.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \param cond - condition which must be satified.
 * \return Number of elements in range that satify a given condition.
 */
template<typename iterator_t, typename condition_t>
int long count_if( iterator_t it, iterator_t end, condition_t cond )
	{
	int long counter( 0 );
	for ( ; it != end; ++ it )
		{
		if ( cond( *it ) )
			++ counter;
		}
	return ( counter );
	}

/*! \brief For each element in range pass its value into function.
 *
 * \param it - begining of the range.
 * \param end - one past the end of range.
 * \param CALL - invoke this function and pass values from range to it.
 */
template<typename iterator_t, typename call_t>
void for_each( iterator_t it, iterator_t const& end, call_t CALL )
	{
	for ( ; it != end; ++ it )
		CALL( *it );
	}

/*! \brief Replace elements in destination container by transformed elements from source range.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param dst - begining of destination container.
 * \param op - transforming operation, an unary function.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename operation_t>
dst_iter_t transform( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ dst )
		*dst = op( *it );
	return ( dst );
	}

/*! \brief Replace elements in destination container by transformed elements, from source range, that are not equal to some value.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param dst - begining of destination container.
 * \param skip_ - value that should be skipped while performing range transformation.
 * \param op - transforming operation, an unary function.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename skip_t, typename operation_t>
dst_iter_t remove_transform( src_iter_t it, src_iter_t end, dst_iter_t dst, skip_t skip_, operation_t op )
	{
	for ( ; it != end; ++ it )
		{
		if ( *it != skip_ )
			{
			*dst = op( *it );
			++ dst;
			}
		}
	return ( dst );
	}

/*! \brief Replace elements in destination container by transformed elements, from source range, that do not satisfy some predicate.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param dst - begining of destination container.
 * \param predicate_ - predicate used to filter out unwantted elements.
 * \param op - transforming operation, an unary function.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename pred_t, typename operation_t>
dst_iter_t remove_transform_if( src_iter_t it, src_iter_t end, dst_iter_t dst, pred_t predicate_, operation_t op )
	{
	for ( ; it != end; ++ it )
		{
		if ( ! predicate_( *it ) )
			{
			*dst = op( *it );
			++ dst;
			}
		}
	return ( dst );
	}

/*! \brief Replace elements in destination container by elements from source range conditionaly transformed.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param dst - begining of destination container.
 * \param op - transforming operation, an unary function.
 * \param cond - condition that has to be met to perform a transformation.
 * \return one past end of output range.
 */
template<typename src_iter_t, typename dst_iter_t, typename operation_t, typename condition_t>
dst_iter_t transform_if( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op, condition_t cond )
	{
	for ( ; it != end; ++ it, ++ dst )
		{
		if ( cond( *it ) )
			*dst = op( *it );
		else
			*dst = *it;
		}
	return ( dst );
	}

/*! \brief Replace elements in destination container by elements resulting from transformation of elements from source range and another container.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param arg - begining of second source container.
 * \param dst - begining of destination container.
 * \param op - transforming operation, a binary function.
 * \return end of output range.
 */
template<typename src_iter_t, typename arg_iter_t, typename dst_iter_t, typename operation_t>
dst_iter_t transform( src_iter_t it, src_iter_t end, arg_iter_t arg, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ arg, ++ dst )
		*dst = op( *it, *arg );
	return ( dst );
	}

/*! \brief Replace all occurences of given value in range with new value.
 * \param it - begining of range to modify.
 * \param end - one past the end of range to modify.
 * \param oldVal_ - old value that shall to be replaced.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename value_t>
void replace( iterator_t it, iterator_t end, value_t const& oldVal_, value_t const& newVal_ )
	{
	for ( ; it != end; ++ it )
		{
		if ( *it == oldVal_ )
			*it = newVal_;
		}
	return;
	}

/*! \brief Replace all occurences of values in range fulfilling given predicate with new value.
 * \param it - begining of range to modify.
 * \param end - one past the end of range to modify.
 * \param predicate_ - a predicate to fulfill so a value could be replaced.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename value_t, typename pred_t>
void replace_if( iterator_t it, iterator_t end, pred_t predicate_, value_t const& newVal_ )
	{
	for ( ; it != end; ++ it )
		{
		if ( predicate_( *it ) )
			*it = newVal_;
		}
	return;
	}

/*! \brief Copies one range onto the another but with replacing all occurences of given value with new value.
 * \param it - begining of range to copy.
 * \param end - one past the end of range to copy.
 * \param out_ - begining of output range.
 * \param oldVal_ - old value that shall to be replaced.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename output_iterator_t, typename value_t>
void replace_copy( iterator_t it, iterator_t end, output_iterator_t out, value_t const& oldVal_, value_t const& newVal_ )
	{
	for ( ; it != end; ++ it, ++ out )
		{
		if ( *it == oldVal_ )
			*out = newVal_;
		else
			*out = *it;
		}
	return;
	}

/*! \brief Copies one range onto the another but with replacing all occurences of value satifying a given predicate with new value.
 * \param it - begining of range to copy.
 * \param end - one past the end of range to copy.
 * \param out_ - begining of output range.
 * \param predicate_ - replace all elements that satify this predicate.
 * \param newVal_ - a value that shall replace old values.
 */
template<typename iterator_t, typename output_iterator_t, typename pred_t, typename value_t>
void replace_copy_if( iterator_t it, iterator_t end, output_iterator_t out, pred_t predicate_, value_t const& newVal_ )
	{
	for ( ; it != end; ++ it, ++ out )
		{
		if ( predicate_( *it ) )
			*out = newVal_;
		else
			*out = *it;
		}
	return;
	}

/*! \brief Copy range of values onto another range.
 *
 * \param src - begining of source range of elements.
 * \param end - one past the end of source range of elements.
 * \param dst - begining of destination range.
 * \return one past end of destination range.
 */
template<typename src_it_t, typename dst_it_t>
dst_it_t copy( src_it_t src, src_it_t const& end, dst_it_t dst )
	{
	for ( ; src != end; ++ src, ++ dst )
		*dst = *src;
	return ( dst );
	}

/*! \brief Copy range of values onto another range.
 *
 * \param first - begining of source range of elements.
 * \param last - one past the end of source range of elements.
 * \param dst - one past end of destination range.
 * \return begining of destination range.
 */
template<typename src_it_t, typename dst_it_t>
dst_it_t copy_backward( src_it_t const& first, src_it_t last, dst_it_t dst )
	{
	if ( first != last )
		{
		-- last;
		-- dst;
		for ( ; first != last; -- last, -- dst )
			*dst = *last;
		*dst = *last;
		}
	return ( dst );
	}

/*! \brief Copy given number of elements from range of values onto another range.
 *
 * \param src - begining of source range of elements.
 * \param count - number of elements to copy.
 * \param dst - begining of destination range.
 * \return one past end of destination range.
 */
template<typename src_it_t, typename dst_it_t>
dst_it_t copy_n( src_it_t src, int long count, dst_it_t dst )
	{
	for ( int long i = 0; i < count; ++ i, ++ src, ++ dst )
		*dst = *src;
	return ( dst );
	}

/*! \brief Remove all occurences of given value from range.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param value_ - a value to be removed.
 * \return new end of range - none of elements in resulting range meet a condition.
 */
template<typename iterator_t, typename value_t>
iterator_t remove( iterator_t first_, iterator_t const& last_, value_t value_ )
	{
	bool move( false );
	for ( iterator_t it( first_ ); it != last_; ++ it )
		{
		if ( *it == value_ )
			{
			move = true;
			continue;
			}
		if ( move )
			*first_ = *it;
		++ first_;
		}
	return ( first_ );
	}

/*! \brief Remove elements meeting a predicate from range.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param condition_ - a predicate that element must fulfill to be removed.
 * \return new end of range - none of elements in resulting range meet a condition.
 */
template<typename iterator_t, typename condition_t>
iterator_t remove_if( iterator_t first_, iterator_t const& last_, condition_t condition_ )
	{
	bool move( false );
	for ( iterator_t it( first_ ); it != last_; ++ it )
		{
		if ( condition_( *it ) )
			{
			move = true;
			continue;
			}
		if ( move )
			*first_ = *it;
		++ first_;
		}
	return ( first_ );
	}

/*! \brief Copy elements all elements not equal to given value from one range to another.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param res_ - begining of output range.
 * \param value_ - elements not equal to this value will be copied..
 * \return end of output range - none of elements in resulting range are equal to given value.
 */
template<typename iter1_t, typename iter2_t, typename value_t>
iter2_t remove_copy( iter1_t first_, iter1_t const& last_, iter2_t res_, value_t value_ )
	{
	for ( ; first_ != last_; ++ first_ )
		{
		if ( *first_ != value_ )
			{
			*res_ = *first_;
			++ res_;
			}
		}
	return ( res_ );
	}

/*! \brief Copy elements that do not meet given predicate from first one to another.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param res_ - begining of output range.
 * \param condition_ - a predicate that element must fulfill to be removed.
 * \return end of output range - none of elements in resulting range meet a condition.
 */
template<typename iter1_t, typename iter2_t, typename condition_t>
iter2_t remove_copy_if( iter1_t first_, iter1_t const& last_, iter2_t res_, condition_t condition_ )
	{
	for ( ; first_ != last_; ++ first_ )
		{
		if ( ! condition_( *first_ ) )
			{
			*res_ = *first_;
			++ res_;
			}
		}
	return ( res_ );
	}

/*! \brief Checks if two ranges are same size and have same set of values.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past last element of second range.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t>
bool safe_equal( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( ( it1 == end1 ) && ( it2 == end2 ) );
	}

/*! \brief Checks if two ranges have same set of values.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t>
bool equal( iter1_t it1, iter1_t end1, iter2_t it2 )
	{
	for ( ; ( it1 != end1 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( it1 == end1 );
	}

/*! \brief Checks if two ranges are equivalent according to some predicate.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \param predicate_ - a predicate to test, an equity equivalence.
 * \return true if and only if ranges have same size and same contents.
 */
template<typename iter1_t, typename iter2_t, typename pred_t>
bool equal( iter1_t it1, iter1_t end1, iter2_t it2, pred_t predicate_ )
	{
	for ( ; ( it1 != end1 ) && predicate_( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( it1 == end1 );
	}

/*! \brief Checks if one range is lexicographicaly before another range.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past last element of second range.
 * \return true if and only if first range is lexicographicaly before second range.
 */
template<typename iter1_t, typename iter2_t>
bool lexicographical_compare( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	/* skip equals */
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( ( ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 < *it2 ) ) || ( ( it1 == end1 ) && ( it2 != end2 ) ) );
	}

/*! \brief Find first difference in ranges values or difference in range sizes.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past last element of second range.
 * \return A pair of iteators which point to first differentiating element.
 */
template<typename iter1_t, typename iter2_t>
yaal::hcore::HPair<iter1_t, iter2_t> safe_mismatch( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( make_pair( it1, it2 ) );
	}

/*! \brief Find first difference in ranges of values.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \return A pair of iteators which point to first differentiating element.
 */
template<typename iter1_t, typename iter2_t>
yaal::hcore::HPair<iter1_t, iter2_t> mismatch( iter1_t it1, iter1_t end1, iter2_t it2 )
	{
	for ( ; ( it1 != end1 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( make_pair( it1, it2 ) );
	}

/*! \brief Find first difference in ranges of values.
 * 
 * \param it1 - begining of first range.
 * \param end1 - one past last element of first range.
 * \param it2 - begining of second range.
 * \param predicate_ - a predicate to test a differences with.
 * \return A pair of iteators which point to first differentiating element.
 */
template<typename iter1_t, typename iter2_t, typename pred_t>
yaal::hcore::HPair<iter1_t, iter2_t> mismatch( iter1_t it1, iter1_t end1, iter2_t it2, pred_t predicate_ )
	{
	for ( ; ( it1 != end1 ) && predicate_( *it1, *it2 ); ++ it1, ++ it2 )
		;
	return ( make_pair( it1, it2 ) );
	}

/*! \brief Rotate range of elements, or "swap" two consecutive ranges of elements.
 * \param first_ - begining of range to rotate, or begining of first range to "swap".
 * \param mid_ - the rotation point, or one past end of first range to "swap" and begining of the second range.
 * \param last_ - one past end of range to rotate, or one past end of the second range to "swap".
 * \return A rotation point that could be used to revert the operation, or new begining of new second range.
 */
template<typename iter_t>
iter_t rotate( iter_t first_, iter_t mid_, iter_t last_ )
	{
	iter_t mid( mid_ );
	iter_t newMid( last_ );
	iter_t it( mid_ );
	while ( first_ != it )
		{
		swap( *first_, *it );
		++ first_, ++ it, ++ mid;
		if ( it == last_ )
			it = mid_;
		else if ( first_ == mid_ )
			mid_ = it;
		if ( mid == last_ )
			{
			newMid = first_;
			mid = first_;
			}
		}
	return ( newMid );
	}

/*! \brief Rotate range of elements, or "swap" two consecutive ranges of elements storing result in another range.
 * \param first_ - begining of range to rotate, or begining of first range to "swap".
 * \param mid_ - the rotation point, or one past end of first range to "swap" and begining of the second range.
 * \param last_ - one past end of range to rotate, or one past end of the second range to "swap".
 * \param out_ - begining of detination range.
 * \return A rotation point that could be used to revert the operation, or new begining of new second range.
 */
template<typename iter_t, typename out_it_t>
out_it_t rotate_copy( iter_t first_, iter_t mid_, iter_t last_, out_it_t out_ )
	{
	out_it_t it( copy( mid_, last_, out_ ) );
  it = copy( first_, mid_, it );
	return ( it );
	}

/*! \brief Joins two sorted ranges of elements into one sorted range of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 * \param comp_ - comparision operator.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t, typename compare_t>
iter_out_t merge( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out, compare_t comp_ )
	{
	for ( ; it1 != end1; ++ it1, ++ out )
		{
		for ( ; ( it2 != end2 ) && comp_( *it2, *it1 ); ++ it2, ++ out )
			*out = *it2;
		*out = *it1;
		}
	return ( copy( it2, end2, out ) );
	}

/*! \brief Joins two sorted ranges of elements into one sorted range of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
iter_out_t merge( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out )
	{
	return ( merge( it1, end1, it2, end2, out, less<typename hcore::iterator_traits<iter_in1_t>::value_type>() ) );
	}

/*! \cond */
template<typename iter_t, typename compare_t>
void inplace_merge_impl( iter_t first_, iter_t mid_, iter_t last_, compare_t comp_ )
	{
	iter_t it( mid_ );
	while ( ( first_ != it ) && ( it != last_ ) )
		{
		while ( ( first_ != it ) && ! comp_( *it, *first_ ) )
			++ first_;
		while ( ( it != last_ ) && comp_( *it, *first_ ) )
			++ it;
		first_ = rotate( first_, mid_, it );
		mid_ = it;
		if ( it == last_ )
			it = mid_;
		else if ( first_ == mid_ )
			mid_ = it;
		}
	return;
	}
template<typename iter_t, typename compare_t>
void inplace_merge_impl( iter_t first_, iter_t mid_, iter_t last_, compare_t comp_,
		hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iter_t>::value_type>& aux_ )
	{
	typedef hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iter_t>::value_type> aux_t;
	if ( aux_.get_size() > 0 )
		{
		if ( aux_.get_size() == aux_.get_requested_size() )
			merge( aux_.begin(), aux_.end(), mid_, last_, first_, comp_ );
		else
			{
			typename aux_t::value_type* out( aux_.begin() );
			typename aux_t::value_type* end( aux_.end() );
			iter_t it1( first_ );
			iter_t it2( mid_ );
			for ( ; it1 != mid_; ++ it1, ++ out )
				{
				for ( ; ( it2 != last_ ) && comp_( *it2, *it1 ); ++ it2, ++ out )
					{
					if ( out == end )
						{
						it1 = copy_backward( it1, mid_, it2 );
						mid_ = it2;
						swap_ranges( aux_.begin(), end, first_ );
						first_ = it1;
						out = aux_.begin();
						}
					*out = *it2;
					}
				if ( out == end )
					{
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
		}
	else
		inplace_merge_impl( first_, mid_, last_, comp_ );
	return;
	}
/*! \endcond */

/*! \brief Merges in place two consecutive sorted ranges of elements into one sorted range of elements.
 *
 * \param first_ - begining of first range.
 * \param mid_ - one past the end of first range and beginning of second range.
 * \param last_ - end of second range.
 * \param comp_ - comparision operator.
 */
template<typename iter_t, typename compare_t>
void inplace_merge( iter_t first_, iter_t mid_, iter_t last_, compare_t comp_ )
	{
	typedef typename hcore::iterator_traits<iter_t>::value_type value_t;
	typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
	aux_t aux( first_, mid_ );
	if ( aux.get_size() > 0 )
		inplace_merge_impl( first_, mid_, last_, comp_, aux );
	else
		inplace_merge_impl( first_, mid_, last_, comp_ );
	return;
	}

/*! \brief Merges in place two consecutive sorted ranges of elements into one sorted range of elements.
 *
 * \param first_ - begining of first range.
 * \param mid_ - one past the end of first range and beginning of second range.
 * \param last_ - end of second range.
 */
template<typename iter_t>
void inplace_merge( iter_t first_, iter_t mid_, iter_t last_ )
	{
	inplace_merge( first_, mid_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \brief Create union of two sorted ranges of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
iter_out_t set_union( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out )
	{
	while ( ( it1 != end1 ) || ( it2 != end2 ) )
		{
		if ( it1 != end1 )
			{
			if ( it2 != end2 )
				{
				if ( *it1 < *it2 )
					{
					*out = *it1;
					++ it1;
					}
				else if ( *it2 < *it1 )
					{
					*out = *it2;
					++ it2;
					}
				else
					{
					*out = *it1;
					++ it1;
					++ it2;
					}
				}
			else
				{
				*out = *it1;
				++ it1;
				}
			}
		else if ( it2 != end2 )
			{
			*out = *it2;
			++ it2;
			}
		++ out;
		}
	return ( out );
	}

/*! \brief Create intersection of two sorted ranges of elements.
 *
 * \param it1 - begining of first range.
 * \param end1 - one past the end of first range.
 * \param it2 - begining of second range.
 * \param end2 - one past the end of second range.
 * \param out - destination container extender functor.
 */
template<typename iter_in1_t, typename iter_in2_t, typename iter_out_t>
iter_out_t set_intersection( iter_in1_t it1, iter_in1_t end1, iter_in2_t it2, iter_in2_t end2, iter_out_t out )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ); ++ it1 )
		{
		for ( ; ( it2 != end2 ) && ( *it2 < *it1 ); ++ it2 )
			;
		if ( ( it2 != end2 ) && ! ( *it1 < *it2 ) )
			{
			*out = *it1;
			++ out;
			++ it2;
			}
		}
	return ( out );
	}

/*! \brief Swap contents (values) of two ranges of elements.
 *
 * \param first - begining of first range.
 * \param end - one past the end of first range.
 * \param second - begining of second range.
 */
template<typename first_it_t, typename second_it_t>
void swap_ranges( first_it_t first, first_it_t const& end, second_it_t second )
	{
	for ( ; first != end; ++ first, ++ second )
		swap( *first, *second ) ;
	return;
	}

/*! \brief Fill range of elements with given value.
 *
 * \param it - begining of range to fill.
 * \param end - one past the end of range.
 * \param filler - value to be set for all elements in range.
 */
template<typename dst_it_t, typename filler_t>
void fill( dst_it_t it, dst_it_t end, filler_t const& filler )
	{
	for ( ; it != end; ++ it )
		*it = filler;
	return;
	}

/*! \brief Fill specified amount of elements with given value.
 *
 * \param it - begining of range to fill.
 * \param count - amount of elements to set.
 * \param filler - value to be set for all elements in range.
 */
template<typename dst_it_t, typename filler_t>
void fill_n( dst_it_t it, int long count, filler_t const& filler )
	{
	for ( int long i = 0; i < count; ++ i, ++ it )
		*it = filler;
	return;
	}

/*! \brief Fill specified range with run-time generated values.
 *
 * \param it - begining of the range.
 * \param end - one past last element in range.
 * \param generator - function object that creates new values for range.
 */
template<typename iterator_t, typename generator_t>
void generate( iterator_t it, iterator_t const& end, generator_t generator )
	{
	for ( ; it != end; ++ it )
		*it = generator();
	return;
	}

/*! \brief Fill specified range with n run-time generated values.
 *
 * \param it - begining of the container.
 * \param count - numer of the elements to be generated.
 * \param generator - function object that creates new values for range.
 */
template<typename iterator_t, typename generator_t>
void generate_n( iterator_t it, int long count, generator_t generator )
	{
	for ( int long i = 0; i < count; ++ i, ++ it )
		*it = static_cast<typename hcore::iterator_traits<iterator_t>::value_type>( generator() );
	return;
	}

/*! \brief Reverses order of elements in range.
 *
 * \param it - begining of range of eleemnts to be reverted.
 * \param end - one past last element of range to be reverted.
 */
template<typename iterator_t>
void reverse( iterator_t it, iterator_t end )
	{
	int long count = 0;
	iterator_t itLast;
	for ( iterator_t itEnd = it; itEnd != end; ++ itEnd, ++ count )
		itLast = itEnd;
	count >>= 1;
	for ( int long i = 0; i < count; ++ i, ++ it, -- itLast )
		swap( *it, *itLast );
	return;
	}

/*! \brief Generate next, in lexographical order, permutation of the range of elements.
 *
 * \param it - begining of range of eleemnts for permutation.
 * \param end - one past last element of range for permutation.
 * \return true iff last, in lexographical order, permutation has been generated.
 */
template<typename iterator_t>
bool next_permutation( iterator_t it, iterator_t end )
	{
	int long count = 0;
	iterator_t itLast;
	for ( iterator_t itEnd = it; itEnd != end; ++ itEnd, ++ count )
		itLast = itEnd;
	bool gotNext = false;
	if ( count > 1 )
		{
		do
			{
			iterator_t i = itLast;
			-- i;
			-- count;
			iterator_t ie = itLast;
			while ( *i >= * ie )
				{
				-- count;
				if ( ! count )
					break;
				-- i, -- ie;
				}
			if ( ! count )
				break;
			iterator_t j = itLast;
			while ( *j <= *i )
				-- j;
			swap( *i, *j );
			++ i;
			reverse( i, end );
			gotNext = true;
			}
		while ( 0 );
		if ( ! gotNext )
			reverse( it, end );
		}
	return ( gotNext );
	}

/*! \brief Generate previous, in lexographical order, permutation of the range of elements.
 *
 * \param it - begining of range of eleemnts for permutation.
 * \param end - one past last element of range for permutation.
 * \return true iff first, in lexographical order, permutation has been generated.
 */
template<typename iterator_t>
bool prev_permutation( iterator_t it, iterator_t end )
	{
	int long count = 0;
	iterator_t itLast;
	for ( iterator_t itEnd = it; itEnd != end; ++ itEnd, ++ count )
		itLast = itEnd;
	bool gotPrev = false;
	if ( count > 1 )
		{
		do
			{
			iterator_t i = itLast;
			-- i;
			-- count;
			iterator_t ie = itLast;
			while ( *i <= * ie )
				{
				-- count;
				if ( ! count )
					break;
				-- i, -- ie;
				}
			if ( ! count )
				break;
			iterator_t j = itLast;
			while ( *j >= *i )
				-- j;
			swap( *i, *j );
			++ i;
			reverse( i, end );
			gotPrev = true;
			}
		while ( 0 );
		if ( ! gotPrev )
			reverse( it, end );
		}
	return ( gotPrev );
	}

/*! \brief Calculate sum of elements in range.
 *
 * \param it - begining of range of eleemnts to summed up.
 * \param end - one past last element of range to be summed up.
 * \param ret - starting value for sum operation.
 * \return sum of all elements in [it, end) + ret.
 */
template<typename iterator_t, typename return_t>
return_t accumulate( iterator_t it, iterator_t end, return_t ret )
	{
	for ( ; it != end; ++ it )
		ret += *it;
	return ( ret );
	}

/*! \brief Find minimum element in a range.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t>
iterator_t min_element( iterator_t it, iterator_t end )
	{
	iterator_t min( it );
	++ it;
	for ( ; ( it != end ); ++ it )
		{
		if ( *it < *min )
			min = it;
		}
	return ( min );
	}

/*! \brief Find element in a range that is best with respect to some predicate.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \param pred - predicate to test.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t, typename predicate_t>
iterator_t min_element( iterator_t it, iterator_t end, predicate_t predicate_ )
	{
	iterator_t min( it );
	++ it;
	for ( ; ( it != end ); ++ it )
		{
		if ( predicate_( *it, *min ) )
			min = it;
		}
	return ( min );
	}

/*! \brief Find maximum element in a range.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t>
iterator_t max_element( iterator_t it, iterator_t end )
	{
	iterator_t max( it );
	++ it;
	for ( ; ( it != end ); ++ it )
		{
		if ( *it > *max )
			max = it;
		}
	return ( max );
	}

/*! \brief Find element in a range that is best with respect to some predicate.
 *
 * \param it - begining of the range to search thru.
 * \param end - one past the end of the range to search thru.
 * \param pred - predicate to test.
 * \return iterator pointing to found position or end of range.
 */
template<typename iterator_t, typename predicate_t>
iterator_t max_element( iterator_t it, iterator_t end, predicate_t predicate_ )
	{
	return ( min_element( it, end, predicate_ ) );
	}

/*! \brief Calculate absolute value of a number.
 *
 * \param val - a number which absolute value shall be calculated.
 * \return |val|
 */
template<typename tType>
inline tType abs( tType const& val )
	{
	return ( val >= 0 ? val : - val );
	}

/*! \brief Make heap from range of elements.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparision operator.
 */
template<typename iter_t, typename compare_t>
void make_heap( iter_t first_, iter_t last_, compare_t comp_ )
	{
	int long size( last_ - first_ );
	for ( int long i( ( size / 2 ) - 1 ); i >= 0; -- i )
		{
		int long pos( i );
		while ( pos < size )
			{
			int long left( ( pos * 2 ) + 1 );
			int long right( ( pos * 2 ) + 2 );
			if ( left < size )
				{
				int long child( right < size ? ( comp_( *( first_ + left ), *( first_ + right ) ) ? right : left ) : left );
				if ( comp_( *( first_ + pos ), *( first_ + child ) ) )
					swap( *( first_ + pos ), *( first_ + child ) );
				else
					break;
				pos = child;
				}
			else
				break;
			}
		}
	return;
	}

/*! \brief Make heap from range of elements.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iter_t>
void make_heap( iter_t first_, iter_t last_ )
	{
	make_heap( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \brief Add element to the heap.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap and new element at the same time.
 * \param comp_ - comparision operator.
 */
template<typename iter_t, typename compare_t>
void push_heap( iter_t first_, iter_t last_, compare_t comp_ )
	{
	int long pos( ( last_ - first_ ) - 1 ); /* zero based position of inserted element */
	while ( ( pos > 0 ) && comp_( *( first_ + ( ( pos -1 ) / 2 ) ), *( first_ + pos ) ) )
		{
		swap( *( first_ + ( ( pos - 1 ) / 2 ) ), *( first_ + pos ) );
		pos = ( pos - 1 ) / 2;
		}
	return;
	}

/*! \brief Add element to the heap.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap and new element at the same time.
 */
template<typename iter_t>
void push_heap( iter_t first_, iter_t last_ )
	{
	push_heap( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \brief Retrieve top element from the heap.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparision operator.
 */
template<typename iter_t, typename compare_t>
void pop_heap( iter_t first_, iter_t last_, compare_t comp_ )
	{
	int long size( ( last_ - first_ ) - 1 );
	if ( size > 0 )
		{
		swap( *first_, *( last_ - 1 ) );
		int long pos( 0 );
		while ( pos < size )
			{
			int long left( ( pos * 2 ) + 1 );
			int long right( ( pos * 2 ) + 2 );
			if ( left < size )
				{
				int long child( right < size ? ( comp_( *( first_ + left ), *( first_ + right ) ) ? right : left ) : left );
				if ( comp_( *( first_ + pos ), *( first_ + child ) ) )
					swap( *( first_ + pos ), *( first_ + child ) );
				else
					break;
				pos = child;
				}
			else
				break;
			}
		}
	return;
	}

/*! \brief Retrieve top element from the heap.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iter_t>
void pop_heap( iter_t first_, iter_t last_ )
	{
	pop_heap( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \brief Test if given range of elements forms a heap.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparision operator.
 */
template<typename iter_t, typename compare_t>
bool is_heap( iter_t first_, iter_t last_, compare_t comp_ )
	{
	bool isHeap( true );
	int long size( last_ - first_ );

	int long childs( 0 );

	while ( isHeap && ( childs < size ) )
		{
		int long roots( childs );
		childs = ( childs * 2 ) + 1;
		for ( int long r( roots ), c( childs ); ( r < childs ) && ( c < size ); ++ r, ++ c )
			{
			if ( comp_( *( first_ + r ), *( first_ + c ) ) )
				{
				isHeap = false;
				break;
				}
			++ c;
			if ( ( c < size ) && comp_( *( first_ + r ), *( first_ + c ) ) )
				{
				isHeap = false;
				break;
				}
			}
		}
	return ( isHeap );
	}

/*! \brief Test if given range of elements forms a heap.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iter_t>
bool is_heap( iter_t first_, iter_t last_ )
	{
	return ( yaal::is_heap( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() ) );
	}

/*! \brief Turn range of elements that is a heap into a sorted range.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 * \param comp_ - comparision operator.
 */
template<typename iter_t, typename compare_t>
void sort_heap( iter_t first_, iter_t last_, compare_t comp_ )
	{
	for ( int long i( last_ - first_ ); i > 1; -- i )
		pop_heap( first_, first_ + i, comp_ );
	return;
	}

/*! \brief Turn range of elements that is a heap into a sorted range.
 *
 * \param first_ - begining of the heap.
 * \param last_ - one past the end of the heap.
 */
template<typename iter_t>
void sort_heap( iter_t first_, iter_t last_ )
	{
	sort_heap( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \brief Sort range of elements (sorting algorithm is unstable).
 *
 * \param first_ - begining of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 * \param comp_ - comparision operator used for sorting.
 */
template<typename iter_t, typename compare_t>
void sort( iter_t first_, iter_t last_, compare_t comp_ )
	{
	make_heap( first_, last_, comp_ );
	sort_heap( first_, last_, comp_ );
	return;
	}

/*! \brief Sort range of elements (sorting algorithm is unstable).
 *
 * \param first_ - begining of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 */
template<typename iter_t>
void sort( iter_t first_, iter_t last_ )
	{
	sort( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \cond */
template<typename iter_t, typename compare_t>
void insert_sort( iter_t first_, iter_t last_, compare_t comp_, hcore::iterator_category::forward )
	{
	typedef typename hcore::iterator_traits<iter_t>::value_type value_t;
	iter_t it( first_ );
	++ it;
	while ( it != last_ )
		{
		iter_t next( it );
		++ next;
		iter_t sorted( first_ );
		while ( ( sorted != it ) && ! comp_( *it, *sorted ) )
			++ sorted;
		if ( sorted != it )
			{
			value_t tmp( *it );
			copy_backward( sorted, it, next );
			*sorted = tmp;
			}
		++ it;
		}
	return;
	}
template<typename iter_t, typename compare_t>
void insert_sort( iter_t first_, iter_t last_, compare_t comp_, hcore::iterator_category::random_access )
	{
	typedef typename hcore::iterator_traits<iter_t>::value_type value_t;
	iter_t it( first_ );
	++ it;
	while ( it != last_ )
		{
		int long dist( ( it - first_ ) / 2 );
		iter_t sorted( first_ + dist );
		while ( dist > 1 )
			{
			dist >>= 1;
			if ( comp_( *it, *sorted ) )
				sorted -= ( dist & 1 ? dist + 1 : dist );
			else
				sorted += dist;
			}
		if ( ( sorted != first_ ) && comp_( *it, *sorted ) )
			-- sorted;
		while ( ( sorted != it ) && ! comp_( *it, *sorted ) )
			++ sorted;
		if ( sorted != it )
			{
			value_t tmp( *it );
			copy_backward( sorted, it, it + 1 );
			*sorted = tmp;
			}
		++ it;
		}
	return;
	}
/*! \endcond */

namespace
{
static int const YAAL_MERGE_ALGO_THRESHOLD = 16;
}

/*! \cond */
template<typename iter_t, typename compare_t>
void stable_sort_impl( iter_t first_, iter_t last_, compare_t comp_,
		hcore::HAuxiliaryBuffer<typename hcore::iterator_traits<iter_t>::value_type>& aux_ )
	{
	using yaal::distance;
	int long size( distance( first_, last_, typename hcore::iterator_traits<iter_t>::category_type() ) );
	if ( size < YAAL_MERGE_ALGO_THRESHOLD )
		insert_sort( first_, last_, comp_, typename hcore::iterator_traits<iter_t>::category_type() );
	else
		{
		iter_t mid( first_ );
		using yaal::advance;
		advance( mid, size / 2, typename hcore::iterator_traits<iter_t>::category_type() );
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
 * \param first_ - begining of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 * \param comp_ - comparision operator used for sorting.
 */
template<typename iter_t, typename compare_t>
void stable_sort( iter_t first_, iter_t last_, compare_t comp_ )
	{
	using yaal::distance;
	int long size( distance( first_, last_, typename hcore::iterator_traits<iter_t>::category_type() ) );
	if ( size < YAAL_MERGE_ALGO_THRESHOLD )
		insert_sort( first_, last_, comp_, typename hcore::iterator_traits<iter_t>::category_type() );
	else
		{
		iter_t mid( first_ );
		using yaal::advance;
		advance( mid, size / 2, typename hcore::iterator_traits<iter_t>::category_type() );
		typedef typename hcore::iterator_traits<iter_t>::value_type value_t;
		typedef hcore::HAuxiliaryBuffer<value_t> aux_t;
		aux_t aux( first_, mid );
		stable_sort_impl( first_, mid, comp_, aux );
		stable_sort_impl( mid, last_, comp_, aux );
		aux.init( first_, mid );
		inplace_merge_impl( first_, mid, last_, comp_, aux );
		}
	return;
	}

/*! \brief Perform stable sort of range of elements (sorting algorithm is unstable).
 *
 * \param first_ - begining of the range to be sorted.
 * \param last_ - one past the end of the range to be sorted.
 */
template<typename iter_t>
void stable_sort( iter_t first_, iter_t last_ )
	{
	stable_sort( first_, last_, less<typename hcore::iterator_traits<iter_t>::value_type>() );
	return;
	}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param dest_ - begining of destination range.
 * \param comp_ - uniqeness test predicate.
 */
template<typename iter_t, typename iter_out_t, typename compare_t>
iter_t unique_copy( iter_t first_, iter_t last_, iter_out_t dest_, compare_t comp_ )
	{
	if ( first_ != last_ )
		{
		*dest_ = *first_;
		++ first_;
		for ( ; first_ != last_; ++ first_ )
			{
			while ( comp_( *first_, *dest_ ) && ( first_ != last_ ) )
				++ first_;
			++ dest_;
			*dest_ = *first_;
			}
		++ dest_;
		}
	return ( dest_ );
	}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param dest_ - begining of destination range.
 * \param comp_ - uniqeness test predicate.
 */
template<typename iter_t, typename iter_out_t>
iter_t unique_copy( iter_t first_, iter_t last_, iter_out_t dest_ )
	{
	return ( unique_copy( first_, last_, dest_, equal_to<typename hcore::iterator_traits<iter_t>::value_type>() ) );
	}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 * \param comp_ - uniqeness test predicate.
 */
template<typename iter_t, typename compare_t>
iter_t unique( iter_t first_, iter_t last_, compare_t comp_ )
	{
	return ( unique_copy( first_, last_, first_, comp_ ) );
	}

/*! \brief Remove consecutive duplicates from range.
 *
 * \param first_ - begining of range of elements to filter.
 * \param last_ - one past the end of range of elements to filter.
 */
template<typename iter_t>
iter_t unique( iter_t first_, iter_t last_ )
	{
	return ( unique( first_, last_, equal_to<typename hcore::iterator_traits<iter_t>::value_type>() ) );
	}

}

#endif /* #ifndef YAAL_HCORE_ALGORITHM_HXX_INCLUDED */

