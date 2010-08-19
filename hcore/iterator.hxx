/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	iterator.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_ITERATOR_HXX_INCLUDED
#define YAAL_HCORE_ITERATOR_HXX_INCLUDED

namespace yaal
{

namespace hcore
{

template<typename T>
struct iterator_interface
	{
	typedef T value_type;
	};

template<typename iterator>
struct iterator_traits
	{
	typedef typename iterator::value_type value_type;
	};

template<typename T>
struct iterator_traits<T*>
	{
	typedef T value_type;
	};

/*! \brief (Back)Insertion concept for HInsertIterator.
 *
 * May be used for collections that support back insertion.
 */
template<typename tType>
class HBackInsertionConcept
	{
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.push_back( elem ); }
	};

/*! \brief (Front)Insertion concept for HInsertIterator.
 *
 * May be used for collections that support front insertion.
 */
template<typename tType>
class HFrontInsertionConcept
	{
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.push_front( elem ); }
	};

/*! \brief Insertion concept for HInsertIterator.
 *
 * Shall be used when insertion to sets/maps is required.
 */
template<typename tType>
class HInsertionConcept
	{
public:
	template<typename elem_t>
	static void insert( tType& coll, elem_t const& elem )
		{ coll.insert( elem ); }
	};

/*! \brief Iterator class that allows extending existing collections.
 *
 * \tparam tType - collection type.
 * \tparam inserter - insertion concept, tell how collection shall be extended.
 */
template<typename tType, typename inserter>
class HInsertIterator
	{
	tType& _coll;
public:
	HInsertIterator( tType& coll ) : _coll( coll ) {}
	HInsertIterator& operator ++ ( void )
		{ return ( *this ); }
	HInsertIterator& operator* ( void )
		{ return ( *this ); }
	template<typename arg_t>
	HInsertIterator& operator = ( arg_t const& elem )
		{
		inserter::insert( _coll, elem );
		return ( *this );
		}
	};

template<typename tType>
HInsertIterator<tType, HBackInsertionConcept<tType> > back_insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HBackInsertionConcept<tType> >( coll ) ); }

template<typename tType>
HInsertIterator<tType, HFrontInsertionConcept<tType> > front_insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HFrontInsertionConcept<tType> >( coll ) ); }

template<typename tType>
HInsertIterator<tType, HInsertionConcept<tType> > insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType, HInsertionConcept<tType> >( coll ) ); }

}

}

#endif /* not YAAL_HCORE_ITERATOR_HXX_INCLUDED */

