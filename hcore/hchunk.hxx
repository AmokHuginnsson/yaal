/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hchunk.hxx
 * \brief Declaration of HChunk class.
 */

#ifndef YAAL_HCORE_HCHUNK_HXX_INCLUDED
#define YAAL_HCORE_HCHUNK_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"

namespace yaal {

namespace hcore {

template<typename T>
int long chunk_size( int long count_ ) {
	return ( static_cast<int>( sizeof ( T ) ) * count_ );
}

/*! \brief Simplest raw memory provisioner.
 */
class HChunk final {
public:
	typedef HChunk this_type;
private:
	int long _size;
	void* _data;
public:
	enum class STRATEGY {
		GEOMETRIC,
		EXACT
	};
	typedef yaal::hcore::HPointer<HChunk> ptr_t;
	/*! \brief Create empty memory holder.
	 */
	HChunk( void );
	/*! \brief Create memory chunk guard and allocate memory for it.
	 *
	 * \param size - requested memory chunk size.
	 */
	explicit HChunk( int long size, STRATEGY = STRATEGY::EXACT );
	HChunk( HChunk&& );
	HChunk& operator = ( HChunk&& );
	~HChunk( void );
	template<typename tType>
	tType const* get() const {
		return ( static_cast<tType const*>( _data ) );
	}
	template<typename tType>
	tType* get() {
		return ( static_cast<tType*>( _data ) );
	}
	void const* raw( void ) const {
		return ( static_cast<char const*>( _data ) );
	}
	void* raw( void ) {
		return ( static_cast<char*>( _data ) );
	}
	void free( void );
	void* realloc( int long, STRATEGY = STRATEGY::GEOMETRIC );
	template<typename T>
	void* realloc( int long count_, STRATEGY strategy_ = STRATEGY::GEOMETRIC ) {
		return ( realloc( chunk_size<T>( count_ ), strategy_ ) );
	}
	void swap( HChunk& );
	int long get_size( void ) const {
		return ( _size );
	}
	int long size( void ) const {
		return ( _size );
	}
	template<typename T>
	int long count_of( void ) const {
		return ( _size / static_cast<int>( sizeof ( T ) ) );
	}
private:
	HChunk( HChunk const& ) = delete;
	HChunk& operator = ( HChunk const& ) = delete;
};

inline void swap( yaal::hcore::HChunk& a, yaal::hcore::HChunk& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HCHUNK_HXX_INCLUDED */

