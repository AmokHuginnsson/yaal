#ifndef YAAL_MSVCXX_SYNCHRONIZEDUNORDEREDSET_HXX_INCLUDED
#define YAAL_MSVCXX_SYNCHRONIZEDUNORDEREDSET_HXX_INCLUDED 1

#include "crit.hxx"

template<typename type_t>
class SynchronizedUnorderedSet {
	public:
		typedef std::unordered_set<type_t> data_t;
		typedef typename data_t::value_type value_type;
		typedef typename data_t::const_iterator const_iterator;
		typedef typename data_t::iterator iterator;
		typedef std::pair<iterator, bool> insert_result;
	private:
		data_t _data;
		mutable CMutex _mutex;
	public:
		SynchronizedUnorderedSet( void ) : _data(), _mutex() {}
		~SynchronizedUnorderedSet( void ) {}
		insert_result insert( value_type const& value_ ) {
			CLock l( _mutex );
			return ( _data.insert( value_ ) );
		}
		void erase( iterator it_ ) {
			CLock l( _mutex );
			_data.erase( it_ );
		}
		int long erase( value_type const& value_ ) {
			CLock l( _mutex );
			return ( _data.erase( value_ ) );
		}
		int long count( value_type const& value_ ) const {
			CLock l( _mutex );
			return ( _data.count( value_ ) );
		}
};

#endif /* #ifndef YAAL_MSVCXX_SYNCHRONIZEDUNORDEREDSET_HXX_INCLUDED */
