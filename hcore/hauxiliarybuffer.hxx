/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hauxiliarybuffer.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hauxiliarybuffer.hxx
 * \brief Implementation of HAuxiliaryBuffer<> class template.
 */

#ifndef YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED
#define YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED 1

namespace yaal
{

namespace hcore
{

/*! \brief Helper class for adaptive algorithms.
 */
template<typename type_t>
class HAuxiliaryBuffer
	{
public:
	typedef HAuxiliaryBuffer<type_t> this_type;
	typedef type_t value_type;
private:
	value_type* _data;
	int long _size;
public:
	template<typename iter_t>
	HAuxiliaryBuffer( iter_t, iter_t );
	~HAuxiliaryBuffer( void );
	value_type* begin( void );
	value_type* end( void );
	int long get_size( void ) const;
	};

template<typename type_t>
template<typename iter_t>
HAuxiliaryBuffer<type_t>::HAuxiliaryBuffer( iter_t first_, iter_t last_ )
	{
	return;
	}

template<typename type_t>
HAuxiliaryBuffer<type_t>::~HAuxiliaryBuffer( void )
	{
	return;
	}

template<typename type_t>
typename HAuxiliaryBuffer<type_t>::value_type* HAuxiliaryBuffer<type_t>::begin( void )
	{
	return ( _data );
	}

template<typename type_t>
typename HAuxiliaryBuffer<type_t>::value_type* HAuxiliaryBuffer<type_t>::end( void )
	{
	return ( _data + _size );
	}

template<typename type_t>
int long HAuxiliaryBuffer<type_t>::get_size( void ) const
	{
	return ( _size );
	}

}

}

#endif /* #ifndef YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED */

