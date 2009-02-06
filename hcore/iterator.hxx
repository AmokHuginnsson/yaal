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

template<typename tType>
class HBackInsertIterator
	{
	tType& _coll;
public:
	HBackInsertIterator( tType& coll ) : _coll( coll ) {}
	HBackInsertIterator& operator ++ ( void )
		{ return ( *this ); }
	HBackInsertIterator& operator* ( void )
		{ return ( *this ); }
	template<typename arg_t>
	HBackInsertIterator& operator = ( arg_t const& elem )
		{
		_coll.push_back( elem );
		return ( *this );
		}
	};

template<typename tType>
HBackInsertIterator<tType> back_insert_iterator( tType& coll )
	{ return ( HBackInsertIterator<tType>( coll ) ); }

template<typename tType>
class HFrontInsertIterator
	{
	tType& _coll;
public:
	HFrontInsertIterator( tType& coll ) : _coll( coll ) {}
	HFrontInsertIterator& operator ++ ( void )
		{ return ( *this ); }
	HFrontInsertIterator& operator* ( void )
		{ return ( *this ); }
	template<typename arg_t>
	HFrontInsertIterator& operator = ( arg_t const& elem )
		{
		_coll.front_back( elem );
		return ( *this );
		}
	};

template<typename tType>
HFrontInsertIterator<tType> front_insert_iterator( tType& coll )
	{ return ( HFrontInsertIterator<tType>( coll ) ); }

template<typename tType>
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
		_coll.insert( elem );
		return ( *this );
		}
	};

template<typename tType>
HInsertIterator<tType> insert_iterator( tType& coll )
	{ return ( HInsertIterator<tType>( coll ) ); }

}

}

#endif /* not YAAL_HCORE_ITERATOR_HXX_INCLUDED */

