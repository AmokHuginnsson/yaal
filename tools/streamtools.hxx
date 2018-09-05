/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/streamtools.hxx
 * \brief Various yaal::hcore::HStreamInterface utilities.
 *
 * HBinaryFormatter - `bin' manipulator for yaal::hcore::HStreamInterface.
 */

#ifndef YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED
#define YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "hcore/harray.hxx"
#include "hcore/hstaticarray.hxx"
#include "hcore/hdeque.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hset.hxx"
#include "hcore/hhashset.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/htuple.hxx"
#include "hcore/hbitset.hxx"
#include "tools/hring.hxx"
#include "tools/htwowaymap.hxx"
#include "tools/hbitmap.hxx"

namespace yaal {

namespace tools {

/*! \brief New manipulator for yaal::hcore::HStreamInterface, `bin'.
 *
 * Output PODs as in binary number format.
 */
class HBinaryFormatter {
	yaal::hcore::HStreamInterface* _stream;
public:
	HBinaryFormatter( yaal::hcore::HStreamInterface* );
	HBinaryFormatter& operator << ( int short );
	HBinaryFormatter& operator << ( int short unsigned );
	HBinaryFormatter& operator << ( int );
	HBinaryFormatter& operator << ( int unsigned );
	HBinaryFormatter& operator << ( int long );
	HBinaryFormatter& operator << ( int long unsigned );
	HBinaryFormatter& operator << ( int long long );
	HBinaryFormatter& operator << ( int long long unsigned );
	HBinaryFormatter& operator << ( float );
	HBinaryFormatter& operator << ( double );
	HBinaryFormatter& operator << ( double long );
	template<typename T>
	HBinaryFormatter& operator << ( T const& v ) {
		return ( binary( &v, sizeof ( T ) ) );
	}
	yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface::manipulator_t );
	HBinaryFormatter& binary( void const*, int );
};

class HBinaryFormatterSeed {
public:
	HBinaryFormatter create( yaal::hcore::HStreamInterface* ) const;
} extern bin;

HBinaryFormatter operator << ( yaal::hcore::HStreamInterface&, HBinaryFormatterSeed const& );

yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::tools::HBitmap const& );

yaal::hcore::HString get_stream_id( yaal::hcore::HStreamInterface* );
yaal::hcore::HStreamInterface& ensure( yaal::hcore::HStreamInterface& );
yaal::hcore::HStreamInterface::ptr_t ensure( yaal::hcore::HStreamInterface::ptr_t );

namespace stream {

template<typename T>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& stream_, T const& value_  ) {
	return ( stream_.write( value_ ) );
}

template<typename T>
yaal::hcore::HStreamInterface& read( yaal::hcore::HStreamInterface& stream_, T& output_  ) {
	return ( stream_.read( output_ ) );
}

template<typename first_t, typename second_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& os, yaal::hcore::HPair<first_t, second_t> const& p ) {
	M_PROLOG
	if ( os.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		os << "pair<" << p.first << "," << p.second << ">";
	} else {
		os << p.first << p.second;
	}
	return ( os );
	M_EPILOG
}

template<typename first_t, typename second_t>
yaal::hcore::HStreamInterface& read( yaal::hcore::HStreamInterface& is, yaal::hcore::HPair<first_t, second_t>& p ) {
	M_PROLOG
	if ( is.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		read( read( is.consume( "pair<" ), p.first ).consume( "," ), p.second ).consume( ">" );
	} else {
		is >> p.first >> p.second;
	}
	return ( is );
	M_EPILOG
}

template<typename tag_t, typename T>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& os, yaal::hcore::HTaggedPOD<tag_t, T> const& v ) {
	M_PROLOG
	return ( os.write( v.get() ) );
	M_EPILOG
}

template<typename container>
yaal::hcore::HStreamInterface& container_dump( yaal::hcore::HStreamInterface& out,
		container const& container_, char sep_, char const* name_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << ( name_ ? name_ : "" );
		char sep( '(' );
		for ( typename container::const_iterator it( container_.begin() ), end( container_.end() ); it != end; ++ it, sep = sep_ ) {
			out << sep << *it;
		}
		if ( container_.is_empty() ) {
			out << sep;
		}
		out << ")" << yaal::hcore::flush;
	} else {
		write( out, container_.get_size() );
		for ( typename container::value_type const& e : container_ ) {
			write( out, e );
		}
	}
	return ( out );
	M_EPILOG
}

template<typename container>
yaal::hcore::HStreamInterface& container_dump( yaal::hcore::HStreamInterface& out,
		container const& container_, char sep_ ) {
	M_PROLOG
	return ( container_dump( out, container_, sep_, nullptr ) );
	M_EPILOG
}

template<typename container>
yaal::hcore::HStreamInterface& container_dump( yaal::hcore::HStreamInterface& out,
		container const& container_, char const* name_  ) {
	M_PROLOG
	return ( container_dump( out, container_, ' ', name_ ) );
	M_EPILOG
}

template<typename container_t, typename method_t>
yaal::hcore::HStreamInterface& container_scan(
	yaal::hcore::HStreamInterface& in,
	container_t& container_, method_t method_,
	char sep_, char const* name_
) {
	M_PROLOG
	container_t container;
	do {
		if ( in.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
			if ( name_ ) {
				in.consume( name_ );
				if ( ! in.good() ) {
					break;
				}
			}
			in.consume( "(" );
			if ( ! in.good() ) {
				break;
			}
			typename container_t::value_type e;
			while ( in.good() ) {
				int next( in.peek() );
				if ( ( next < 0 ) || ( next == ')' ) ) {
					break;
				}
				if ( next == sep_ ) {
					in.consume( sep_ );
				}
				read( in, e );
				( container.*method_ )( e );
			}
			in.consume( ")" );
		} else {
			typename container_t::size_type size( 0 );
			read( in, size );
			typename container_t::value_type e;
			for ( typename container_t::size_type i( 0 ); in.good() && ( i < size ); ++ i ) {
				read( in, e );
				( container.*method_ )( e );
			}
		}
		container_ = yaal::move( container );
	} while ( false );
	return ( in );
	M_EPILOG
}

template<typename container_t, typename method_t>
yaal::hcore::HStreamInterface& container_scan(
	yaal::hcore::HStreamInterface& in,
	container_t& container_, method_t method_, char sep_
) {
	M_PROLOG
	return ( container_scan( in, container_, method_, sep_, nullptr ) );
	M_EPILOG
}

template<typename container_t, typename method_t>
yaal::hcore::HStreamInterface& container_scan(
	yaal::hcore::HStreamInterface& in,
	container_t& container_, method_t method_, char const* name_
) {
	M_PROLOG
	return ( container_scan( in, container_, method_, ' ', name_ ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HArray<tType, allocator_t> const& a_ ) {
	M_PROLOG
	return ( container_dump( out, a_, "array" ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& read( yaal::hcore::HStreamInterface& out, yaal::hcore::HArray<tType, allocator_t>& a_ ) {
	M_PROLOG
	typedef yaal::hcore::HArray<tType, allocator_t> array_t;
	return ( container_scan( out, a_, static_cast<void ( array_t::* )( typename array_t::value_type const& )>( &array_t::push_back ), "array" ) );
	M_EPILOG
}

template<typename tType, int const N>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HStaticArray<tType, N> const& sa_ ) {
	M_PROLOG
	typedef yaal::hcore::HStaticArray<tType, N> container_t;
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "staticarray";
		char sep( '(' );
		for ( typename container_t::const_iterator it( sa_.begin() ), end( sa_.end() ); it != end; ++ it, sep = ' ' ) {
			out << sep << *it;
		}
		if ( sa_.is_empty() ) {
			out << sep;
		}
		out << ")" << yaal::hcore::flush;
	} else {
		for ( typename container_t::value_type const& e : sa_ ) {
			write( out, e );
		}
	}
	return ( out );
	M_EPILOG
}

template<typename tType, int const N>
yaal::hcore::HStreamInterface& read( yaal::hcore::HStreamInterface& in, yaal::hcore::HStaticArray<tType, N>& sa_ ) {
	M_PROLOG
	typedef yaal::hcore::HStaticArray<tType, N> container_t;
	container_t container;
	do {
		if ( in.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
			in.consume( "staticarray(" );
			if ( ! in.good() ) {
				break;
			}
			for ( typename container_t::size_type i( 0 ); in.good() && ( i < N ); ++ i ) {
				int next( in.peek() );
				if ( ( next < 0 ) || ( next == ')' ) ) {
					break;
				}
				if ( next == ' ' ) {
					in.consume( " " );
				}
				read( in, container[i] );
			}
			in.consume( ")" );
		} else {
			for ( typename container_t::size_type i( 0 ); in.good() && ( i < N ); ++ i ) {
				read( in, container[i] );
			}
		}
		sa_ = yaal::move( container );
	} while ( false );
	return ( in );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HDeque<tType, allocator_t> const& d_ ) {
	M_PROLOG
	return ( container_dump( out, d_, "deque" ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& read( yaal::hcore::HStreamInterface& out, yaal::hcore::HDeque<tType, allocator_t>& d_ ) {
	M_PROLOG
	typedef yaal::hcore::HDeque<tType, allocator_t> array_t;
	return ( container_scan( out, d_, static_cast<void ( array_t::* )( typename array_t::value_type const& )>( &array_t::push_back ), "deque" ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HList<tType, allocator_t> const& l_ ) {
	M_PROLOG
	return ( container_dump( out, l_, "list" ) );
	M_EPILOG
}

template<typename tType, typename compare_t, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HSet<tType, compare_t, allocator_t> const& s_ ) {
	M_PROLOG
	return ( container_dump( out, s_, "set" ) );
	M_EPILOG
}

template<typename key_t, typename value_t, typename compare_t, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HMap<key_t, value_t, compare_t, allocator_t, yaal::hcore::HSBBSTree<yaal::hcore::HPair<key_t const, value_t>, compare_t, yaal::hcore::map_helper<key_t, value_t>, allocator_t>> const& m_ ) {
	M_PROLOG
	return ( container_dump( out, m_, "map" ) );
	M_EPILOG
}

template<typename key_t, typename value_t, typename compare_t, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HMap<key_t, value_t, compare_t, allocator_t, yaal::hcore::HLookup<yaal::hcore::HPair<key_t const, value_t>, compare_t, yaal::hcore::map_helper<key_t, value_t>, yaal::trait::no_type>> const& m_ ) {
	M_PROLOG
	return ( container_dump( out, m_, "lookup" ) );
	M_EPILOG
}

template<typename value_t, typename hasher_t, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HHashSet<value_t, hasher_t, allocator_t> const& hs_ ) {
	M_PROLOG
	return ( container_dump( out, hs_, "hash_set" ) );
	M_EPILOG
}

template<typename key_t, typename value_t, typename hasher_t, typename allocator_t>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HHashMap<key_t, value_t, hasher_t, allocator_t> const& hs_ ) {
	M_PROLOG
	return ( container_dump( out, hs_, "hash_map" ) );
	M_EPILOG
}

template<typename T0>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", " << t_.template get<2>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>() << t_.template get<2>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>() << t_.template get<2>() << t_.template get<3>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>() << t_.template get<2>() << t_.template get<3>() << t_.template get<4>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
			<< t_.template get<6>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>()
			<< t_.template get<6>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
			<< t_.template get<6>() << ", " << t_.template get<7>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>()
			<< t_.template get<6>() << t_.template get<7>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
			<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
			<< t_.template get<8>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>()
			<< t_.template get<6>() << t_.template get<7>()
			<< t_.template get<8>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
			<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
			<< t_.template get<8>() << ", " << t_.template get<9>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>()
			<< t_.template get<6>() << t_.template get<7>()
			<< t_.template get<8>() << t_.template get<9>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7,
	typename T8, typename T9, typename T10>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out,
		yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
			<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
			<< t_.template get<8>() << ", " << t_.template get<9>() << ", "
			<< t_.template get<10>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>()
			<< t_.template get<6>() << t_.template get<7>()
			<< t_.template get<8>() << t_.template get<9>()
			<< t_.template get<10>();
	}
	return ( out );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7,
	typename T8, typename T9, typename T10, typename T11>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out,
		yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> const& t_ ) {
	M_PROLOG
	if ( out.get_mode() == yaal::hcore::HStreamInterface::MODE::TEXT ) {
		out << "tuple<" << t_.template get<0>() << ", " << t_.template get<1>() << ", "
			<< t_.template get<2>() << ", " << t_.template get<3>() << ", "
			<< t_.template get<4>() << ", " << t_.template get<5>() << ", "
			<< t_.template get<6>() << ", " << t_.template get<7>() << ", "
			<< t_.template get<8>() << ", " << t_.template get<9>() << ", "
			<< t_.template get<10>() << ", " << t_.template get<11>() << ">" << yaal::hcore::flush;
	} else {
		out << t_.template get<0>() << t_.template get<1>()
			<< t_.template get<2>() << t_.template get<3>()
			<< t_.template get<4>() << t_.template get<5>()
			<< t_.template get<6>() << t_.template get<7>()
			<< t_.template get<8>() << t_.template get<9>()
			<< t_.template get<10>() << t_.template get<11>();
	}
	return ( out );
	M_EPILOG
}

template<int const SIZE>
yaal::hcore::HStreamInterface& write( yaal::hcore::HStreamInterface& out, yaal::hcore::HBitset<SIZE> const& bs_ ) {
	M_PROLOG
	out << "bitset<" << bs_.to_string() << ">" << yaal::hcore::flush;
	return ( out );
	M_EPILOG
}

}

template<typename tType>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::tools::HRing<tType> const& r_ ) {
	M_PROLOG
	return ( stream::container_dump( out, r_, "ring" ) );
	M_EPILOG
}

template<typename left_t, typename right_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::tools::HTwoWayMap<left_t, right_t> const& twm_ ) {
	M_PROLOG
	return ( stream::container_dump( out, twm_, "twowaymap" ) );
	M_EPILOG
}

class HTee : public yaal::hcore::HStreamInterface {
public:
	typedef HTee this_type;
	typedef yaal::hcore::HStreamInterface base_type;
private:
	yaal::hcore::HStreamInterface& _stream1;
	yaal::hcore::HStreamInterface& _stream2;
public:
	HTee( yaal::hcore::HStreamInterface& stream1_, yaal::hcore::HStreamInterface& stream2_ )
		: _stream1( stream1_ )
		, _stream2( stream2_ ) {
	}
	HTee( HTee const& );
protected:
	virtual int long do_write( void const*, int long ) override;
	virtual int long do_read( void*, int long ) override M_DEBUG_CODE( __attribute__((noreturn)) );
	virtual void do_flush( void ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
	virtual bool do_good( void ) const override;
	virtual bool do_fail( void ) const override;
	virtual bool do_bad( void ) const override;
};

HTee tee( yaal::hcore::HStreamInterface::ptr_t, yaal::hcore::HStreamInterface::ptr_t  );
HTee tee( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface::ptr_t  );
HTee tee( yaal::hcore::HStreamInterface::ptr_t, yaal::hcore::HStreamInterface& );
HTee tee( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface& );

}

namespace hcore {

template<typename first_t, typename second_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& os, yaal::hcore::HPair<first_t, second_t> const& p ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( os, p ) );
	M_EPILOG
}

template<typename first_t, typename second_t>
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& is, yaal::hcore::HPair<first_t, second_t>& p ) {
	M_PROLOG
	using tools::stream::read;
	return ( read( is, p ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HArray<tType, allocator_t> const& a_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, a_ ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& in, yaal::hcore::HArray<tType, allocator_t>& a_ ) {
	M_PROLOG
	using tools::stream::read;
	return ( read( in, a_ ) );
	M_EPILOG
}

template<typename tType, int const N>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HStaticArray<tType, N> const& sa_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, sa_ ) );
	M_EPILOG
}

template<typename tType, int const N>
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& in, yaal::hcore::HStaticArray<tType, N>& sa_ ) {
	M_PROLOG
	using tools::stream::read;
	return ( read( in, sa_ ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HDeque<tType, allocator_t> const& d_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, d_ ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface& in, yaal::hcore::HDeque<tType, allocator_t>& d_ ) {
	M_PROLOG
	using tools::stream::read;
	return ( read( in, d_ ) );
	M_EPILOG
}

template<typename tType, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HList<tType, allocator_t> const& l_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, l_ ) );
	M_EPILOG
}

template<typename tType, typename compare_t, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HSet<tType, compare_t, allocator_t> const& s_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, s_ ) );
	M_EPILOG
}

template<typename key_t, typename value_t, typename compare_t, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HMap<key_t, value_t, compare_t, allocator_t, yaal::hcore::HSBBSTree<yaal::hcore::HPair<key_t const, value_t>, compare_t, yaal::hcore::map_helper<key_t, value_t>, allocator_t>> const& m_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, m_ ) );
	M_EPILOG
}

template<typename key_t, typename value_t, typename compare_t, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HMap<key_t, value_t, compare_t, allocator_t, yaal::hcore::HLookup<yaal::hcore::HPair<key_t const, value_t>, compare_t, yaal::hcore::map_helper<key_t, value_t>, yaal::trait::no_type>> const& m_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, m_ ) );
	M_EPILOG
}

template<typename value_t, typename hasher_t, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HHashSet<value_t, hasher_t, allocator_t> const& hs_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, hs_ ) );
	M_EPILOG
}

template<typename key_t, typename value_t, typename hasher_t, typename allocator_t>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HHashMap<key_t, value_t, hasher_t, allocator_t> const& hs_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, hs_ ) );
	M_EPILOG
}

template<typename T0>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7,
	typename T8, typename T9, typename T10>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out,
		yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<typename T0, typename T1, typename T2, typename T3,
	typename T4, typename T5, typename T6, typename T7,
	typename T8, typename T9, typename T10, typename T11>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out,
		yaal::hcore::HTuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> const& t_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, t_ ) );
	M_EPILOG
}

template<int const SIZE>
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface& out, yaal::hcore::HBitset<SIZE> const& bs_ ) {
	M_PROLOG
	using tools::stream::write;
	return ( write( out, bs_ ) );
	M_EPILOG
}

class HNumber;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HNumber const& );
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface&, yaal::hcore::HNumber& );

class HTime;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HTime const& );
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface&, yaal::hcore::HTime& );

class HComplex;
yaal::hcore::HStreamInterface& operator << ( yaal::hcore::HStreamInterface&, yaal::hcore::HComplex const& );
yaal::hcore::HStreamInterface& operator >> ( yaal::hcore::HStreamInterface&, yaal::hcore::HComplex& );

}

}

#endif /* #ifndef YAAL_TOOLS_STREAMTOOLS_HXX_INCLUDED */

