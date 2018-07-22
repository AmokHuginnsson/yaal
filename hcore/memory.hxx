/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/memory.hxx
 * \brief Declarations of basic memory functions.
 */

#ifndef YAAL_HCORE_MEMORY_HXX_INCLUDED
#define YAAL_HCORE_MEMORY_HXX_INCLUDED 1

#include <cstddef>
#include <new>

#include "hcore/hexception.hxx"

namespace yaal {

/*! \brief Memory handling routines.
 */
namespace memory {

template<typename T, typename aligner_type, int const size, int const tail>
struct aligned_storage {
	aligner_type _aligner;
	char _mem[trait::to_unsigned<int, tail>::value];
	aligned_storage( void )
		: _aligner()
		, _mem() {
	}
	T* mem( void ) {
		return ( static_cast<T*>( static_cast<void*>( this ) ) );
	}
	T const* mem( void ) const {
		return ( static_cast<T const*>( static_cast<void const*>( this ) ) );
	}
};

template<typename T, typename aligner_type, int const size>
struct aligned_storage<T, aligner_type, size, 0> {
	aligner_type _aligner;
	aligned_storage( void )
		: _aligner() {
	}
	T* mem( void ) {
		return ( static_cast<T*>( static_cast<void*>( this ) ) );
	}
	T const* mem( void ) const {
		return ( static_cast<T const*>( static_cast<void const*>( this ) ) );
	}
};

template<int const SIZE, typename T = void, bool const DOUBLE_LONG_ALIGNMENT = false>
struct aligned {
	typedef typename yaal::trait::ternary<
		DOUBLE_LONG_ALIGNMENT && ( SIZE > ALIGNOF_INT_LONG_LONG ),
		double long,
		typename yaal::trait::ternary<
			( SIZE > ALIGNOF_INT_LONG ),
			int long long,
			typename yaal::trait::ternary<
				( SIZE > ALIGNOF_INT ),
				int long,
				typename yaal::trait::ternary<
					( SIZE > ALIGNOF_INT_SHORT ),
					int,
					typename yaal::trait::ternary<
						( SIZE > static_cast<int>( sizeof ( char ) ) ),
						int short,
						char
					>::type
				>::type
			>::type
		>::type
	>::type aligner_type;
	static_assert ( static_cast<int>( sizeof ( aligner_type ) ) <= SIZE, "invalid aligner_type" );
	typedef aligned_storage<T, aligner_type, SIZE, static_cast<int>( SIZE - sizeof ( aligner_type ) )> type;
};

static void const* const INVALID_HANDLE __attribute__((used)) = reinterpret_cast<void const*>( static_cast<int_native_t>( -1 ) );

struct YaalNew {};

static YaalNew yaal __attribute__((used));

struct MemoryAllocation {};
typedef yaal::hcore::HExceptionT<MemoryAllocation> HMemoryAllocationException;

enum class ON_ALLOC_FAILURE {
	ABORT,
	THROW
};

void* alloc( int long );
void* calloc( int long );
void* realloc( void*, int long );
void free0( void* ) throw();

template<typename tType>
inline tType* alloc( int long count_ ) {
	return ( static_cast<tType*>( alloc( count_ * static_cast<int>( sizeof ( tType ) ) ) ) );
}

template<typename tType>
inline tType* calloc( int long count_ ) {
	return ( static_cast<tType*>( calloc( count_ * static_cast<int>( sizeof ( tType ) ) ) ) );
}

template<typename tType>
inline tType* realloc( void* pointer_, int long count_ ) {
	return ( static_cast<tType*>( realloc( pointer_, count_ * static_cast<int>( sizeof ( tType ) ) ) ) );
}

template<typename tType>
inline void free( tType& pointer_ ) throw() {
	free0( pointer_ );
	pointer_ = nullptr;
	return;
}

}

}

void* operator new ( std::size_t, yaal::memory::YaalNew const& );
void operator delete ( void*, yaal::memory::YaalNew const& ) throw ();
inline void operator delete ( void* ptr_, yaal::memory::YaalNew const& ) throw () {
	::operator delete ( ptr_ );
}
void* operator new[] ( std::size_t, yaal::memory::YaalNew const& );
void operator delete[] ( void*, yaal::memory::YaalNew const& ) throw ();
inline void operator delete[] ( void* ptr_, yaal::memory::YaalNew const& ) throw () {
	::operator delete[] ( ptr_ );
}

#endif /* #ifndef YAAL_HCORE_MEMORY_HXX_INCLUDED */

