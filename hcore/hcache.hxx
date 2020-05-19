/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcache.hxx
 * \brief Declaration HCache class template.
 */

#ifndef YAAL_HCORE_HCACHE_HXX_INCLUDED
#define YAAL_HCORE_HCACHE_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hboundcall.hxx"

namespace yaal {

namespace hcore {

template<typename key_t, typename value_t>
class HCache final {
public:
	typedef key_t key_type;
	typedef value_t value_type;
	class HEntry {
		key_type _key;
		value_type _value;
	public:
		HEntry( key_type const& key_, value_type const& value_ )
			: _key( key_ )
			, _value( value_ ) {
		}
		key_type const& key( void ) const {
			return ( _key );
		}
		value_type const& value( void ) const {
			return ( _value );
		}
	};
	typedef HCache<key_type, value_type> this_type;
	typedef yaal::hcore::HList<HEntry> store_t;
	typedef yaal::hcore::HHashMap<key_t, typename store_t::iterator> lookup_t;
	typedef yaal::hcore::HBoundCall<value_type ( key_type const& )> value_fetcher_t;
private:
	store_t _store;
	lookup_t _lookup;
	value_fetcher_t _valueFetcher;
	int long _maxEntriesCount;
public:
	HCache( value_fetcher_t valueFetcher_, int long maxEntriesCount_ )
		: _store()
		, _lookup()
		, _valueFetcher( valueFetcher_ )
		, _maxEntriesCount( maxEntriesCount_ ) {
		M_ASSERT( !! valueFetcher_ );
		M_ASSERT( maxEntriesCount_ > 0 );
	}
	value_type const& obtain( key_type const& key_ ) {
		M_PROLOG
		typename lookup_t::iterator it( _lookup.find( key_ ) );
		if ( it != _lookup.end() ) {
			_store.splice( _store.begin(), _store, it->second );
			it->second = _store.begin();
			return ( it->second->value() );
		}
		_store.emplace_front( key_, _valueFetcher( key_ ) );
		_lookup.insert( make_pair( key_, _store.begin() ) );
		if ( _store.get_size() > _maxEntriesCount ) {
			_lookup.erase( _store.back().key() );
			_store.pop_back();
		}
		return ( _store.front().value() );
		M_EPILOG
	}

};

}

}

#endif /* #ifndef YAAL_HCORE_HCACHE_HXX_INCLUDED */

