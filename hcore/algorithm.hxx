/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	algorithm.hxx - this file is integral part of `yaal' project.

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
#define YAAL_HCORE_ALGORITHM_HXX_INCLUDED

#include "hcore/trait.hxx"
#include "hcore/hpair.hxx"

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
 */
template<typename src_iter_t, typename dst_iter_t, typename operation_t>
void transform( src_iter_t it, src_iter_t end, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ dst )
		*dst = op( *it );
	}

/*! \brief Replace elements in destination container by elements resulting from transformation of elements from source range and another container.
 *
 * \param it - begining of source range.
 * \param end - one past the end of source range.
 * \param arg - begining of second source container.
 * \param dst - begining of destination container.
 * \param op - transforming operation, a binary function.
 */
template<typename src_iter_t, typename arg_iter_t, typename dst_iter_t, typename operation_t>
void transform( src_iter_t it, src_iter_t end, arg_iter_t arg, dst_iter_t dst, operation_t op )
	{
	for ( ; it != end; ++ it, ++ arg, ++ dst )
		*dst = op( *arg, *it );
	}

/*! \brief Copy range of values onto another range.
 *
 * \param src - begining of source range of elements.
 * \param end - one past the end of source range of elements.
 * \param dst - begining of destination range.
 */
template<typename src_it_t, typename dst_it_t>
void copy( src_it_t src, src_it_t const& end, dst_it_t dst )
	{
	for ( ; src != end; ++ src, ++ dst )
		*dst = *src;
	return;
	}

/*! \brief Copy given number of elements from range of values onto another range.
 *
 * \param src - begining of source range of elements.
 * \param count - number of elements to copy.
 * \param dst - begining of destination range.
 */
template<typename src_it_t, typename dst_it_t>
void copy_n( src_it_t src, int long const& count, dst_it_t dst )
	{
	for ( int long i = 0; i < count; ++ i, ++ src, ++ dst )
		*dst = *src;
	return;
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
bool equal( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( ( it1 == end1 ) && ( it2 == end2 ) );
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
yaal::hcore::HPair<iter1_t, iter2_t> mismatch( iter1_t it1, iter1_t end1, iter2_t it2, iter2_t end2 )
	{
	for ( ; ( it1 != end1 ) && ( it2 != end2 ) && ( *it1 == *it2 ); ++ it1, ++ it2 )
		;
	return ( make_pair( it1, it2 ) );
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
	for ( ; it1 != end1; ++ it1, ++ out )
		{
		for ( ; ( it2 != end2 ) && ( *it2 < *it1 ); ++ it2, ++ out )
			*out = *it2;
		*out = *it1;
		}
	for ( ; it2 != end2; ++ it2, ++ out )
		*out = *it2;
	return ( out );
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
void fill_n( dst_it_t it, int long const& count, filler_t const& filler )
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
void generate_n( iterator_t it, int long const& count, generator_t generator )
	{
	for ( int long i = 0; i < count; ++ i, ++ it )
		*it = generator();
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

/*! \brief Calculate distance between two iterators.
 *
 * \param first - iterator.
 * \param last - iterator.
 * \return last - first.
 */
template<typename iter_t>
int long distance( iter_t first, iter_t last )
	{
	int long dist = 0;
	while ( first != last )
		++ first, ++ dist;
	return ( dist );
	}

/*! \brief Move iterator forward.
 *
 * \param it - iterator to be moved.
 * \param dist - how far iterator shall be moved.
 */
template<typename iter_t>
void advance( iter_t& it, int long const& dist )
	{
	for ( int long i = 0; i < dist; ++ i, ++ it )
		;
	}

/*! \brief Get smaller of two values.
 *
 * \param left - first value to be considered as smaller.
 * \param right -  second value to be considered as smaller.
 * \return Smaller of two values.
 */
template<typename tType>
inline tType min( tType const& left, tType const& right )
	{
	return ( left < right ? left : right );
	}

/*! \brief Get bigger of two values.
 *
 * \param left - first value to be considered as bigger.
 * \param right -  second value to be considered as bigger.
 * \return Bigger of two values.
 */
template<typename tType>
inline tType max( tType const& left, tType const& right )
	{
	return ( left >= right ? left : right );
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

/*! \brief Generic less (object ordering) operator.
 *
 * \param left_ - first of the objects to compare.
 * \param right_ - second of the objects to compare.
 * \return True iff (if and only if) left_ < right_ - first objects is lesser than second object.
 */
template<typename tType>
bool less( tType const& left_, tType const& right_ )
	{
	return ( left_ < right_ );
	}

/*! \brief Callculate booleant exclisive or value.
 *
 * \param p - first predicate.
 * \param q - second predicate.
 * \return p exor q <=> ( p ^ q ) v ( ~ ( p v q ) )
 */
inline bool exor ( bool p, bool q )
	{
	return ( ( p && q ) || ( ! ( p || q ) ) );
	}

}

#endif /* not YAAL_HCORE_ALGORITHM_HXX_INCLUDED */

