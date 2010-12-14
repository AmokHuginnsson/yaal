/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	streamtools.hxx - this file is integral part of yaal project.

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

/*! \file tools/streamtools.hxx
 * \brief Various yaal::hcore::HStreamInterface utils.
 *
 * HBinaryFormatter - `bin' mainpulator for yaal::hcore::HStreamInterface.
 */

#ifndef YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED
#define YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "hcore/harray.hxx"
#include "hcore/hdeque.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hset.hxx"
#include "hcore/hhashset.hxx"
#include "hcore/htuple.hxx"
#include "tools/hring.hxx"

namespace yaal
{

namespace tools
{

/*! \brief New manipulator for yaal::hcore::HStreamInterface, `bin'.
 *
 * Output PODs as in binary number format.
 */
class HBinaryFormatter
	{
	yaal::hcore::HStreamInterface* _stream;
public:
	HBinaryFormatter( yaal::hcore::HStreamInterface* );
	template<typename T>
	HBinaryFormatter& operator << ( T const& v )
		{
		return ( binary( &v, sizeof ( T ) ) );
		}
	yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface::manipulator_t const& );
	HBinaryFormatter& binary( void const*, int );
	};

class HBinaryFormatterSeed
	{
public:
	HBinaryFormatter create( yaal::hcore::HStreamInterface* ) const;
	} extern bin;

HBinaryFormatter operator << ( yaal::hcore::HStreamInterface&, HBinaryFormatterSeed const& );

yaal::hcore::HString get_stream_id( yaal::hcore::HStreamInterface* );
yaal::hcore::HStreamInterface& ensure( yaal::hcore::HStreamInterface& );
yaal::hcore::HStreamInterface::ptr_t ensure( yaal::hcore::HStreamInterface::ptr_t );

template<typename container>
yaal::hcore::HStreamInterface& container_dump( yaal::hcore::HStreamInterface& out,
		container const& container_, char sep_, char const* const name_ )
	{
	out << ( name_ ? name_ : "" );
	char sep( '(' );
	for ( typename container::const_iterator it( container_.begin() ), end( container_.end() ); it != end; ++ it, sep = sep_ )
		out << sep << *it;
	out << ")" << yaal::hcore::flush;
	return ( out );
	}

template<typename container>
yaal::hcore::HStreamInterface& container_dump( yaal::hcore::HStreamInterface& out,
		container const& container_, char sep_ )
	{
	return ( container_dump( out, container_, sep_, NULL ) );
	}

template<typename container>
yaal::hcore::HStreamInterface& container_dump( yaal::hcore::HStreamInterface& out,
		container const& container_, char const* const name_  )
	{
	return ( container_dump( out, container_, ' ', name_ ) );
	}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::tools::HRing<tType> const& r_ )
	{
	return ( container_dump( out, r_, "ring" ) );
	}

}

namespace hcore
{

template<typename first_t, typename second_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& os, yaal::hcore::HPair<first_t, second_t> const& p )
	{
	os << "pair<" << p.first << "," << p.second << ">";
	return ( os );
	}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HArray<tType> const& a_ )
	{
	return ( container_dump( out, a_, "array" ) );
	}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HDeque<tType> const& d_ )
	{
	return ( container_dump( out, d_, "deque" ) );
	}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HList<tType> const& l_ )
	{
	return ( container_dump( out, l_, "list" ) );
	}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HSet<tType> const& s_ )
	{
	return ( container_dump( out, s_, "set" ) );
	}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HHashSet<tType> const& hs_ )
	{
	return ( container_dump( out, hs_, "hash_set" ) );
	}

template<typename T0>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", " << t_.template get<2>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3, typename T4>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << t_.template get<4>() << t_.template get<5>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
		<< t_.template get<6>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
		<< t_.template get<6>() << ", " << t_.template get<7>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
		<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
		<< t_.template get<8>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
		<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
		<< t_.template get<8>() << ", " << t_.template get<9>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7,
	typename T8, typename T9, typename T10>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out,
		yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
		<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
		<< t_.template get<8>() << ", " << t_.template get<9>() << ", "
		<< t_.template get<10>() << ">" << yaal::hcore::flush;
	return ( out );
	}

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7,
	typename T8, typename T9, typename T10, typename T11>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out,
		yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> const& t_ )
	{
	out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
		<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
		<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
		<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
		<< t_.template get<8>() << ", " << t_.template get<9>() << ", "
		<< t_.template get<10>() << ", " << t_.template get<11>() << ">" << yaal::hcore::flush;
	return ( out );
	}

class HNumber;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HNumber const& );

class HTime;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HTime const& );

}

}

#endif /* #ifndef YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED */

