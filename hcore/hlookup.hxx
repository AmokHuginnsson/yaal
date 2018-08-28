/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hlookup.hxx
 * \brief Declaration of HLookup related classes.
 */

#ifndef YAAL_HCORE_HLOOKUP_HXX_INCLUDED
#define YAAL_HCORE_HLOOKUP_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/harray.hxx"
#include "hcore/algorithm.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHLookup_[];

/*! \brief HLookup - a sorted array.
 *
 * HLookup is implementation of associative array based on array type
 * for efficiency.
 * Implementation of alternative engine for HMap<> and HSet<> classes.
 * HLookup is not meant to be used directly.
 */
template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
class HLookup {
public:
	typedef HLookup<key_value_t, compare_t, key_get_t, allocator_t> this_type;
	typedef key_value_t key_value_type;
	typedef compare_t compare_type;
	typedef key_get_t key_get_type;
	typedef allocator_t allocator_type;
	typedef int long size_type;
	typedef trait::delayed_sizeof<key_value_type> node_size;
	typedef key_value_type node_type;
	typedef typename key_get_type::key_type key_type;
	typedef HArray<key_value_type> data_type;
	typedef typename data_type::iterator iterator_type;
	/*! \brief HSBBSTreeBase operation error types.
	 */
	struct ERROR {
		typedef enum {
			OK = 0,
			NON_EXISTING_KEY,
			NIL_ITERATOR
		} error_t;
	};
	struct comparator_adaptor {
		compare_type const& _compare;
		comparator_adaptor( compare_type const& compare_ )
			: _compare( compare_ ) {
			return;
		}
		comparator_adaptor( comparator_adaptor const& ) = default;
		comparator_adaptor& operator = ( comparator_adaptor const& ) = default;
		bool operator () ( key_type const& left_, key_type const& right_ ) const {
			return ( _compare( left_, right_ ) );
		}
		bool operator () ( key_type const& left_, key_value_type const& right_ ) const {
			return ( _compare( left_, key_get_type::key( right_ ) ) );
		}
		bool operator () ( key_value_type const& left_, key_type const& right_ ) const {
			return ( _compare( key_get_type::key( left_ ), right_ ) );
		}
	};
private:
	data_type _data;
	compare_type _compare;
public:
	HLookup( compare_type const& compare_, allocator_type const& )
		: _data()
		, _compare( compare_ ) {
		return;
	}
	~HLookup( void ) {
		M_PROLOG
		clear();
		return;
		M_DESTRUCTOR_EPILOG
	}
	HPair<iterator_type, bool> insert( key_value_type const& key_ ) {
		M_PROLOG
		iterator_type it(
			yaal::lower_bound(
				_data.begin(),
				_data.end(),
				key_get_type::key( key_ ),
				comparator_adaptor( _compare )
			)
		);
		bool existed( false );
		if ( ! ( it != _data.end() ) || ( key_get_type::key( *it ) != key_get_type::key( key_ ) ) ) {
			it = _data.insert( it, key_ );
		} else {
			existed = true;
		}
		return ( make_pair( it, ! existed ) );
		M_EPILOG
	}
	HPair<iterator_type, bool> insert( key_value_type&& key_ ) {
		M_PROLOG
		iterator_type it(
			yaal::lower_bound(
				_data.begin(),
				_data.end(),
				key_get_type::key( key_ ),
				comparator_adaptor( _compare )
			)
		);
		bool existed( false );
		if ( ! ( it != _data.end() ) || ( key_get_type::key( *it ) != key_get_type::key( key_ ) ) ) {
			it = _data.insert( it, yaal::move( key_ ) );
		} else {
			existed = true;
		}
		return ( make_pair( it, ! existed ) );
		M_EPILOG
	}
	iterator_type remove( iterator_type const& it_ ) {
		M_PROLOG
		_data.erase( it_ );
		return ( it_ );
		M_EPILOG
	}
	iterator_type find( key_type const& key_ ) const {
		M_PROLOG
		iterator_type it(
			yaal::lower_bound(
				const_cast<data_type&>( _data ).begin(),
				const_cast<data_type&>( _data ).end(),
				key_,
				comparator_adaptor( _compare )
			)
		);
		if ( ( it != _data.end() ) && ( key_get_type::key( *it ) != key_ ) ) {
			it = const_cast<data_type&>( _data ).end();
		}
		return ( it );
		M_EPILOG
	}
	iterator_type lower_bound( key_type const& key_ ) const {
		return (
			yaal::lower_bound(
				const_cast<data_type&>( _data ).begin(),
				const_cast<data_type&>( _data ).end(),
				key_,
				comparator_adaptor( _compare )
			)
		);
	}
	iterator_type upper_bound( key_type const& key_ ) const {
		return (
			yaal::upper_bound(
				const_cast<data_type&>( _data ).begin(),
				const_cast<data_type&>( _data ).end(),
				key_,
				comparator_adaptor( _compare )
			)
		);
	}
	iterator_type begin( void ) const {
		return ( const_cast<data_type&>( _data ).begin() );
	}
	iterator_type end( void ) const {
		return ( const_cast<data_type&>( _data ).end() );
	}
	void clear( void ) {
		M_PROLOG
		_data.clear();
		return;
		M_EPILOG
	}
	size_type get_size( void ) const {
		return ( _data.get_size() );
	}
	bool is_empty( void ) const {
		return ( _data.is_empty() );
	}
	void swap( HLookup& other ) {
		if ( &other != this ) {
			using yaal::swap;
			swap( _data, other._data );
			swap( _compare, other._compare );
		}
		return;
	}
	void copy_from( HLookup const& source ) {
		M_PROLOG
		if ( &source != this ) {
			_data.assign( source._data.begin(), source._data.end() );
			_compare = source._compare;
		}
		return;
		M_EPILOG
	}
	compare_type const& compare( void ) const {
		return ( _compare );
	}
	allocator_type const& get_allocator( void ) const {
		static allocator_type allocator;
		return ( allocator );
	}
};

template<typename key_value_t, typename compare_t, typename key_get_t, typename allocator_t>
inline void swap(
	yaal::hcore::HLookup<key_value_t, compare_t, key_get_t, allocator_t>& a,
	yaal::hcore::HLookup<key_value_t, compare_t, key_get_t, allocator_t>& b
) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HLOOKUP_HXX_INCLUDED */

