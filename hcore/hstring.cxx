/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdlib>
#include <climits>
#include <cwctype>
#include <cstdarg>
#include <cstdio>
#include <libintl.h>

#include "config.hxx"

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hstring.hxx"
#include "memory.hxx"
#include "hchunk.hxx"
#include "hcore.hxx"
#include "safe_int.hxx"
#include "unicode.hxx"
#include "htls.hxx"

namespace yaal {

namespace hcore {

/*! \brief HString class helpers, utility functions.
 */
namespace {

enum ERROR {
	OK = 0,
	NULL_PTR,
	UNINITIALIZED,
	INDEX_OOB,
	BAD_LENGTH,
	BAD_OFFSET,
	OVERFLOW
};

template<typename iterator_t, typename U>
inline void copy_n_safe_cast( iterator_t src_, HString::size_type size_, U* dst_ ) {
	for ( HString::size_type i( 0 ); i < size_; ++ i, ++ src_, ++ dst_ ) {
		*dst_ = safe_int::cast<U>( *src_ );
	}
	return;
}

template<typename iterator_t, typename U>
inline void copy_n_unpack( iterator_t src_, HString::size_type size_, U* dst_ ) {
	for ( HString::size_type i( 0 ); i < size_; ++ i, ++ src_, ++ dst_ ) {
		*dst_ = static_cast<U>( (*src_).get() );
	}
	return;
}

template<typename data_t, typename transform_t>
inline void transform_n_cast( data_t* data_, HString::size_type size_, transform_t transform_ ) {
	typedef typename trait::argument_type<transform_t, 0>::type arg_t;
	for ( HString::size_type i( 0 ); i < size_; ++ i, ++ data_ ) {
		*data_ = static_cast<data_t>( transform_( static_cast<arg_t>( *data_ ) ) );
	}
	return;
}

template<typename iterator_t, typename U>
inline void copy_n_cast_backward( iterator_t src_, HString::size_type size_, U* dst_ ) {
	src_ += ( size_ - 1 );
	dst_ += ( size_ - 1 );
	for ( HString::size_type i( 0 ); i < size_; ++ i, -- src_, -- dst_ ) {
		*dst_ = static_cast<U>( *src_ );
	}
	return;
}

#define KMP_SEARCH( direction_haystack, direction_needle, direction_result, transform ) \
	HChunk KMPnextCache( chunk_size<HString::size_type>( needleLength_ + 1 ) ); \
	HString::size_type* KMPnext( KMPnextCache.get<HString::size_type>() ); \
\
	KMPnext[ 0 ] = -1; \
	HString::size_type KMPnextIndex( 1 ); \
	HString::size_type candidate( 0 ); \
\
	while ( KMPnextIndex < needleLength_ ) { \
		if ( transform( needle_[ direction_needle KMPnextIndex ] ) == transform( needle_[ direction_needle candidate ] ) ) { \
			KMPnext[ KMPnextIndex ] = KMPnext[ candidate ]; \
		} else { \
			KMPnext[ KMPnextIndex ] = candidate; \
			candidate = KMPnext[ candidate ]; \
			while ( ( candidate >= 0 ) && ( transform( needle_[ direction_needle KMPnextIndex ] ) != transform( needle_[ direction_needle candidate ] ) ) ) { \
				candidate = KMPnext[ candidate ]; \
			} \
		} \
		++ KMPnextIndex; \
		++ candidate; \
	} \
	KMPnext[ KMPnextIndex ] = candidate; \
\
	HString::size_type matchStart( 0 ); /* index in string_ */ \
	HString::size_type consideredCharacterIndex( 0 ); /* index in needle_ */ \
	while ( ( matchStart + consideredCharacterIndex ) < haystackLength_ ) { \
		if ( transform( needle_[ direction_needle consideredCharacterIndex ] ) == transform( haystack_[ direction_haystack( matchStart + consideredCharacterIndex ) ] ) ) { \
			++ consideredCharacterIndex; \
			if ( consideredCharacterIndex == needleLength_ ) { \
				return ( direction_result matchStart ); \
			} \
		} else { \
			if ( KMPnext[ consideredCharacterIndex ] > -1 ) { \
				matchStart += ( consideredCharacterIndex - KMPnext[ consideredCharacterIndex ] ); \
				consideredCharacterIndex = KMPnext[ consideredCharacterIndex ]; \
			} else { \
				matchStart += ( consideredCharacterIndex + 1 ); \
				consideredCharacterIndex = 0; \
			} \
		} \
	} \
	return ( -1 );

template<typename haystack_t, typename needle_t>
inline HString::size_type kmpsearch_impl( haystack_t const* haystack_, HString::size_type haystackLength_, needle_t const* needle_, HString::size_type needleLength_ ) {
	KMP_SEARCH(,,,)
}

template<typename haystack_t, typename needle_t>
inline HString::size_type kmpsearch_last_impl( haystack_t const* haystack_, HString::size_type haystackLength_, needle_t const* needle_, HString::size_type needleLength_ ) {
	KMP_SEARCH( haystackLength_ - 1 -, needleLength_ - 1 -, haystackLength_ - needleLength_ -, )
}

template<typename str_ucs_t, typename value_t>
inline HString::size_type find_last_impl( str_ucs_t const* str_, HString::size_type size_, value_t v ) {
	for ( HString::size_type i( size_ - 1 ); i >= 0; -- i ) {
		if ( static_cast<code_point_t>( str_[i] ) == static_cast<code_point_t>( v ) ) {
			return ( i );
		}
	}
	return ( HString::npos );
}

template<typename str_ucs_t, typename set_ucs_t>
inline HString::size_type find_one_of_impl( str_ucs_t const* str_, HString::size_type size_, set_ucs_t const* set_, HString::size_type setSize_ ) {
	for ( HString::size_type i( 0 ); i < size_; ++ i ) {
		for ( HString::size_type s( 0 ); s < setSize_; ++ s ) {
			if ( static_cast<code_point_t>( str_[i] ) == static_cast<code_point_t>( set_[s] ) ) {
				return ( i );
			}
		}
	}
	return ( HString::npos );
}

template<typename str_ucs_t, typename set_ucs_t>
inline HString::size_type find_last_one_of_impl( str_ucs_t const* str_, HString::size_type size_, set_ucs_t const* set_, HString::size_type setSize_ ) {
	for ( HString::size_type i( size_ - 1 ); i >= 0; -- i ) {
		for ( HString::size_type s( 0 ); s < setSize_; ++ s ) {
			if ( static_cast<code_point_t>( str_[i] ) == static_cast<code_point_t>( set_[s] ) ) {
				return ( i );
			}
		}
	}
	return ( HString::npos );
}

template<typename str_ucs_t, typename set_ucs_t>
inline HString::size_type find_other_than_impl( str_ucs_t const* str_, HString::size_type size_, set_ucs_t const* set_, HString::size_type setSize_ ) {
	HString::size_type pos( HString::npos );
	for ( HString::size_type i( 0 ); i < size_; ++ i ) {
		HString::size_type s( 0 );
		for ( ; s < setSize_; ++ s ) {
			if ( static_cast<code_point_t>( str_[i] ) == static_cast<code_point_t>( set_[s] ) ) {
				break;
			}
		}
		if ( s == setSize_ ) {
			pos = i;
			break;
		}
	}
	return ( pos );
}

template<typename str_ucs_t, typename set_ucs_t>
inline HString::size_type find_last_other_than_impl( str_ucs_t const* str_, HString::size_type size_, set_ucs_t const* set_, HString::size_type setSize_ ) {
	HString::size_type pos( HString::npos );
	for ( HString::size_type i( size_ - 1 ); i >= 0; -- i ) {
		HString::size_type s( 0 );
		for ( ; s < setSize_; ++ s ) {
			if ( static_cast<code_point_t>( str_[i] ) == static_cast<code_point_t>( set_[s] ) ) {
				break;
			}
		}
		if ( s == setSize_ ) {
			pos = i;
			break;
		}
	}
	return ( pos );
}

namespace adaptive {

inline code_point_t get( void const* mem_, int rank_, HString::size_type index_ ) {
	code_point_t codePoint( 0 );
	if ( rank_ == 1 ) {
		codePoint = code_point_t( static_cast<yaal::u8_t const*>( mem_ )[index_] );
	} else if ( rank_ == 2 ) {
		codePoint = code_point_t( static_cast<yaal::u16_t const*>( mem_ )[index_] );
	} else {
		codePoint = code_point_t( static_cast<yaal::u32_t const*>( mem_ )[index_] );
	}
	return ( codePoint );
}

inline void set( void* mem_, int rank_, HString::size_type index_, code_point_t codePoint_ ) {
	if ( rank_ == 1 ) {
		static_cast<yaal::u8_t*>( mem_ )[index_] = static_cast<yaal::u8_t>( codePoint_.get() );
	} else if ( rank_ == 2 ) {
		static_cast<yaal::u16_t*>( mem_ )[index_] = static_cast<yaal::u16_t>( codePoint_.get() );
	} else {
		static_cast<yaal::u32_t*>( mem_ )[index_] = codePoint_.get();
	}
	return;
}

inline void fill( void* mem_, int rank_, HString::size_type offset_, HString::size_type size_, code_point_t codePoint_ ) {
	if ( rank_ == 1 ) {
		::memset( static_cast<char*>( mem_ ) + offset_, static_cast<int>( codePoint_.get() ), static_cast<size_t>( size_ ) );
	} else if ( rank_ == 2 ) {
		yaal::fill_n( static_cast<yaal::u16_t*>( mem_ ) + offset_, size_, static_cast<u16_t>( codePoint_.get() ) );
	} else {
		yaal::fill_n( static_cast<yaal::u32_t*>( mem_ ) + offset_, size_, codePoint_.get() );
	}
	return;
}

inline void copy( void* dest_, int destRank_, HString::size_type destOffset_, void const* src_, int srcRank_, HString::size_type srcOffset_, HString::size_type size_ ) {
	if ( destRank_ == srcRank_ ) {
		::memcpy( static_cast<char*>( dest_ ) + destOffset_ * destRank_, static_cast<char const*>( src_ ) + srcOffset_ * srcRank_ /* == destRank_ */, static_cast<size_t>( size_ * destRank_ ) );
	} else {
		switch ( 4 * destRank_ + srcRank_ ) {
			case ( 4 * 2 + 1 ): /* UCS-1 to UCS-2 */ {
				yaal::copy_n( static_cast<yaal::u8_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u16_t*>( dest_ ) + destOffset_ );
			} break;
			case ( 4 * 4 + 1 ): /* UCS-1 to UCS-4 */ {
				yaal::copy_n( static_cast<yaal::u8_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u32_t*>( dest_ ) + destOffset_ );
			} break;
			case ( 4 * 4 + 2 ): /* UCS-2 to UCS-4 */ {
				yaal::copy_n( static_cast<yaal::u16_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u32_t*>( dest_ ) + destOffset_ );
			} break;
			case ( 4 * 1 + 2 ): /* UCS-2 to UCS-1 (Used only in down-ranking through HString::reserve().) */ {
				copy_n_safe_cast( static_cast<yaal::u16_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u8_t*>( dest_ ) + destOffset_ );
			} break;
			case ( 4 * 1 + 4 ): /* UCS-4 to UCS-1 (Used only in down-ranking through HString::reserve().) */ {
				copy_n_safe_cast( static_cast<yaal::u32_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u8_t*>( dest_ ) + destOffset_ );
			} break;
			case ( 4 * 2 + 4 ): /* UCS-4 to UCS-2 (Used only in down-ranking through HString::reserve().) */ {
				copy_n_safe_cast( static_cast<yaal::u32_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u16_t*>( dest_ ) + destOffset_ );
			} break;
		}
	}
	return;
}

/* Used only in up-ranking through HString::reserve(). */
inline void copy_backward( void* dest_, int destRank_, HString::size_type destOffset_, void const* src_, int srcRank_, HString::size_type srcOffset_, HString::size_type size_ ) {
	switch ( 4 * destRank_ + srcRank_ ) {
		case ( 4 * 2 + 1 ): /* UCS-1 to UCS-2 */ {
			copy_n_cast_backward( static_cast<yaal::u8_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u16_t*>( dest_ ) + destOffset_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-1 to UCS-4 */ {
			copy_n_cast_backward( static_cast<yaal::u8_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u32_t*>( dest_ ) + destOffset_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-2 to UCS-4 */ {
			copy_n_cast_backward( static_cast<yaal::u16_t const*>( src_ ) + srcOffset_, size_, static_cast<yaal::u32_t*>( dest_ ) + destOffset_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-2 to UCS-1 */
		case ( 4 * 1 + 4 ): /* UCS-4 to UCS-1 */
		case ( 4 * 2 + 4 ): /* UCS-4 to UCS-2 */ {
			M_ASSERT( !"Bad code path - overwrite while shrinking!"[0] );
		} break;
		default: {
			M_ASSERT( !"Bad code path - backward copy instead of plain copy!"[0] );
		} break;
	}
	return;
}

inline void move( void* dest_, HString::size_type destOffset_, void const* src_, HString::size_type srcOffset_, int rank_, HString::size_type size_ ) {
	void* destStart( static_cast<char*>( dest_ ) + destOffset_ * rank_ );
	void const* srcStart( static_cast<char const*>( src_ ) + srcOffset_ * rank_ );
	::memmove( destStart, srcStart, static_cast<size_t>( rank_ * size_ ) );
	return;
}

template<typename left_ucs_t, typename right_ucs_t>
int compare_impl( left_ucs_t const* left_, HString::size_type leftOffset_, HString::size_type leftSize_, right_ucs_t const* right_, HString::size_type rightOffset_, HString::size_type rightSize_ ) {
	int res( 0 );
	left_ucs_t const* left( left_ + leftOffset_ );
	right_ucs_t const* right( right_ + rightOffset_ );
	HString::size_type len( min( leftSize_, rightSize_ ) );
	for ( HString::size_type i( 0 ); ( res == 0 ) && ( i < len ); ++ i ) {
		res = static_cast<int>( left[i] ) - static_cast<int>( right[i] );
	}
	return ( res != 0 ? res : ( ( leftSize_ < rightSize_ ) ? -1 : ( ( leftSize_ > rightSize_ ) ? 1 : 0 ) ) );
}

inline int compare( void const* left_, int leftRank_, HString::size_type leftOffset_, HString::size_type leftSize_, void const* right_, int rightRank_, HString::size_type rightOffset_, HString::size_type rightSize_ ) {
	int res( 0 );
	switch ( 4 * leftRank_ + rightRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			res = compare_impl(
				static_cast<yaal::u8_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u8_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			res = compare_impl(
				static_cast<yaal::u16_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u8_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			res = compare_impl(
				static_cast<yaal::u32_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u8_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			res = compare_impl(
				static_cast<yaal::u8_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u16_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			res = compare_impl(
				static_cast<yaal::u16_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u16_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			res = compare_impl(
				static_cast<yaal::u32_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u16_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			res = compare_impl(
				static_cast<yaal::u8_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u32_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			res = compare_impl(
				static_cast<yaal::u16_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u32_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			res = compare_impl(
				static_cast<yaal::u32_t const*>( left_ ), leftOffset_, leftSize_,
				static_cast<yaal::u32_t const*>( right_ ), rightOffset_, rightSize_
			);
		} break;
	}
	return ( res );
}

HString::size_type find( void const* mem_, int rank_, HString::size_type size_, HString::size_type after_, code_point_t value_ ) {
	int valueRank( unicode::rank( value_ ) );
	HString::size_type pos( HString::npos );
	switch ( 4 * rank_ + valueRank ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			char const* m( static_cast<char const*>( mem_ ) + after_ );
			char const* p( static_cast<char const*>( ::std::memchr( m, static_cast<yaal::u8_t>( value_.get() ), static_cast<size_t>( size_ - after_ ) ) ) );
			if ( p ) {
				pos = p - m;
			}
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			yaal::u16_t const* m( static_cast<yaal::u16_t const*>( mem_ ) + after_ );
			yaal::u16_t const* e( m + size_ - after_ );
			yaal::u16_t const* p( yaal::find( m, e, static_cast<yaal::u16_t>( value_.get() ) ) );
			if ( p != e ) {
				pos = p - m;
			}
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			yaal::u32_t const* m( static_cast<yaal::u32_t const*>( mem_ ) + after_ );
			yaal::u32_t const* e( m + size_ - after_ );
			yaal::u32_t const* p( yaal::find( m, e, value_.get() ) );
			if ( p != e ) {
				pos = p - m;
			}
		} break;
	}
	return ( pos != HString::npos ? pos + after_ : pos );
}

HString::size_type find_last( void const* mem_, int rank_, HString::size_type before_, code_point_t value_ ) {
	int valueRank( unicode::rank( value_ ) );
	HString::size_type pos( HString::npos );
	switch ( 4 * rank_ + valueRank ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			char const* m( static_cast<char const*>( mem_ ) );
			char const* p( static_cast<char const*>( ::memrchr( m, static_cast<yaal::u8_t>( value_.get() ), static_cast<size_t>( before_ ) ) ) );
			if ( p ) {
				pos = p - m;
			}
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = find_last_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u16_t>( value_.get() ) );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = find_last_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, value_.get() );
		} break;
	}
	return ( pos );
}

inline HString::size_type kmpsearch( void const* haystack_, int haystackRank_, HString::size_type haystackSize_, HString::size_type after_, void const* needle_, int needleRank_, HString::size_type needleSize_ ) {
	HString::size_type pos( HString::npos );
	switch ( 4 * haystackRank_ + needleRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			pos = kmpsearch_impl( static_cast<yaal::u8_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u8_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			pos = kmpsearch_impl( static_cast<yaal::u16_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u8_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			pos = kmpsearch_impl( static_cast<yaal::u32_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u8_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			pos = kmpsearch_impl( static_cast<yaal::u8_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u16_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = kmpsearch_impl( static_cast<yaal::u16_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u16_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			pos = kmpsearch_impl( static_cast<yaal::u32_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u16_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			pos = kmpsearch_impl( static_cast<yaal::u8_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u32_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			pos = kmpsearch_impl( static_cast<yaal::u16_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u32_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = kmpsearch_impl( static_cast<yaal::u32_t const*>( haystack_ ) + after_, haystackSize_ - after_, static_cast<yaal::u32_t const*>( needle_ ), needleSize_ );
		} break;
	}
	return ( pos != HString::npos ? pos + after_ : pos );
}

inline HString::size_type kmpsearch_last( void const* haystack_, int haystackRank_, HString::size_type before_, void const* needle_, int needleRank_, HString::size_type needleSize_ ) {
	HString::size_type pos( HString::npos );
	switch ( 4 * haystackRank_ + needleRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u8_t const*>( haystack_ ), before_, static_cast<yaal::u8_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u16_t const*>( haystack_ ), before_, static_cast<yaal::u8_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u32_t const*>( haystack_ ), before_, static_cast<yaal::u8_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u8_t const*>( haystack_ ), before_, static_cast<yaal::u16_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u16_t const*>( haystack_ ), before_, static_cast<yaal::u16_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u32_t const*>( haystack_ ), before_, static_cast<yaal::u16_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u8_t const*>( haystack_ ), before_, static_cast<yaal::u32_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u16_t const*>( haystack_ ), before_, static_cast<yaal::u32_t const*>( needle_ ), needleSize_ );
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = kmpsearch_last_impl( static_cast<yaal::u32_t const*>( haystack_ ), before_, static_cast<yaal::u32_t const*>( needle_ ), needleSize_ );
		} break;
	}
	return ( pos );
}

inline HString::size_type find_one_of( void const* mem_, int rank_, HString::size_type size_, HString::size_type after_, void const* set_, int setRank_, HString::size_type setSize_ ) {
	HString::size_type pos( HString::npos );
	switch ( 4 * rank_ + setRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			pos = find_one_of_impl( static_cast<yaal::u8_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			pos = find_one_of_impl( static_cast<yaal::u16_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			pos = find_one_of_impl( static_cast<yaal::u32_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			pos = find_one_of_impl( static_cast<yaal::u8_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = find_one_of_impl( static_cast<yaal::u16_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			pos = find_one_of_impl( static_cast<yaal::u32_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			pos = find_one_of_impl( static_cast<yaal::u8_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			pos = find_one_of_impl( static_cast<yaal::u16_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = find_one_of_impl( static_cast<yaal::u32_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
	}
	return ( pos != HString::npos ? pos + after_ : pos );
}

inline HString::size_type find_last_one_of( void const* mem_, int rank_, HString::size_type before_, void const* set_, int setRank_, HString::size_type setSize_ ) {
	HString::size_type pos( HString::npos );
	switch ( 4 * rank_ + setRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u8_t const*>( mem_ ), before_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u8_t const*>( mem_ ), before_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u8_t const*>( mem_ ), before_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = find_last_one_of_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
	}
	return ( pos );
}

inline HString::size_type find_other_than( void const* mem_, int rank_, HString::size_type size_, HString::size_type after_, void const* set_, int setRank_, HString::size_type setSize_ ) {
	HString::size_type pos( HString::npos );
	switch ( 4 * rank_ + setRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			pos = find_other_than_impl( static_cast<yaal::u8_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			pos = find_other_than_impl( static_cast<yaal::u16_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			pos = find_other_than_impl( static_cast<yaal::u32_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			pos = find_other_than_impl( static_cast<yaal::u8_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = find_other_than_impl( static_cast<yaal::u16_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			pos = find_other_than_impl( static_cast<yaal::u32_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			pos = find_other_than_impl( static_cast<yaal::u8_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			pos = find_other_than_impl( static_cast<yaal::u16_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = find_other_than_impl( static_cast<yaal::u32_t const*>( mem_ ) + after_, size_ - after_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
	}
	return ( pos != HString::npos ? pos + after_ : pos );
}

inline HString::size_type find_last_other_than( void const* mem_, int rank_, HString::size_type before_, void const* set_, int setRank_, HString::size_type setSize_ ) {
	HString::size_type pos( HString::npos );
	switch ( 4 * rank_ + setRank_ ) {
		case ( 4 * 1 + 1 ): /* UCS-1 and UCS-1 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u8_t const*>( mem_ ), before_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 1 ): /* UCS-2 and UCS-1 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 1 ): /* UCS-4 and UCS-1 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, static_cast<yaal::u8_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 2 ): /* UCS-1 and UCS-2 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u8_t const*>( mem_ ), before_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 2 ): /* UCS-2 and UCS-2 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 2 ): /* UCS-4 and UCS-2 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, static_cast<yaal::u16_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 1 + 4 ): /* UCS-1 and UCS-4 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u8_t const*>( mem_ ), before_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 2 + 4 ): /* UCS-2 and UCS-4 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u16_t const*>( mem_ ), before_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
		case ( 4 * 4 + 4 ): /* UCS-4 and UCS-4 */ {
			pos = find_last_other_than_impl( static_cast<yaal::u32_t const*>( mem_ ), before_, static_cast<yaal::u32_t const*>( set_ ), setSize_ );
		} break;
	}
	return ( pos );
}

typedef wint_t ( *transformer_t )( wint_t );
inline void transform( void* mem_, int rank_, HString::size_type size_, transformer_t mod_ ) {
	switch ( rank_ ) {
		case ( 1 ): {
			transform_n_cast( static_cast<yaal::u8_t*>( mem_ ), size_, mod_ );
		} break;
		case ( 2 ): {
			transform_n_cast( static_cast<yaal::u16_t*>( mem_ ), size_, mod_ );
		} break;
		case ( 4 ): {
			transform_n_cast( static_cast<yaal::u32_t*>( mem_ ), size_, mod_ );
		} break;
	}
	return;
}

}

namespace utf8 {

struct OUTF8StringStats {
	HString::size_type _characterCount;
	HString::size_type _byteCount;
	int _rank;
};

OUTF8StringStats get_string_stats( char const* str_, HString::size_type size_ ) {
	OUTF8StringStats s{ 0, 0, 0 };
	int byteCount( 0 );
	int characterCount( 0 );
	int charBytesLeft( 0 );
	int rank( 1 );
	int tryRank( 0 );
	u32_t cp( 0 );
	char const* p( str_ );
	while ( ( byteCount < size_ ) && *p ) {
		if ( charBytesLeft == 0 ) {
			if ( ! ( *p & unicode::ENC_1_BYTES_MARK_MASK ) ) {
				++ characterCount;
			} else if ( ( *p & unicode::ENC_2_BYTES_MARK_MASK ) == unicode::ENC_2_BYTES_MARK_VALUE ) {
				charBytesLeft = 1;
				tryRank = 0; /* 3 byte UTF-8 sequence can have either rank == 2 or rank == 1 */
				cp = ( *p & unicode::ENC_2_BYTES_VALUE_MASK );
			} else if ( ( *p & unicode::ENC_3_BYTES_MARK_MASK ) == unicode::ENC_3_BYTES_MARK_VALUE ) {
				charBytesLeft = 2;
				tryRank = 1; /* 3 byte UTF-8 sequence can have either rank == 4 or rank == 2 */
				cp = ( *p & unicode::ENC_3_BYTES_VALUE_MASK );
			} else if ( ( *p & unicode::ENC_4_BYTES_MARK_MASK ) == unicode::ENC_4_BYTES_MARK_VALUE ) {
				charBytesLeft = 3;
				tryRank = 2;
				cp = ( *p & unicode::ENC_4_BYTES_VALUE_MASK );
			} else {
				throw HUTF8StringException( "Invalid UTF-8 head sequence at: "_ys.append( byteCount ) );
			}
		} else if ( ( *p & unicode::TAIL_BYTES_MARK_MASK ) == unicode::TAIL_BYTES_MARK_VALUE ) {
			-- charBytesLeft;
			cp <<= 6;
			cp |= ( *p & unicode::TAIL_BYTES_VALUE_MASK );
			if ( ! charBytesLeft ) {
				static u32_t const UCS_RANK_CUTOFF[] = {
					unicode::UCS_MAX_1_BYTE_CODE_POINT,
					unicode::UCS_MAX_2_BYTE_CODE_POINT,
					0
				};
				static int const RANKS[][3] = { { 1, 2, 4 }, { 2, 4, 4 } };

				++ characterCount;
				rank = max( RANKS[ cp > UCS_RANK_CUTOFF[tryRank] ][tryRank], rank );
			}
		} else {
			throw HUTF8StringException( "Invalid UTF-8 tail sequence at: "_ys.append( byteCount ) );
		}
		++ p;
		++ byteCount;
	}
	if ( charBytesLeft > 0 ) {
		throw HUTF8StringException( "Truncated UTF-8 tail sequence at: "_ys.append( byteCount ) );
	}
	s._byteCount = byteCount;
	s._characterCount = characterCount;
	s._rank = rank;
	return ( s );
}

void encode( code_point_t codePoint_, char*& dest_ ) {
	int r( unicode::utf8_length( codePoint_ ) );
	dest_ += r;
	char* p( dest_ - 1 );
	u8_t head[] = { 0, 0, unicode::ENC_2_BYTES_MARK_VALUE, unicode::ENC_3_BYTES_MARK_VALUE, unicode::ENC_4_BYTES_MARK_VALUE };
	switch ( r ) {
		case ( 4 ): { *p = static_cast<char>( unicode::TAIL_BYTES_MARK_VALUE | ( unicode::TAIL_BYTES_VALUE_MASK & codePoint_.get() ) ); -- p; codePoint_ >>= 6; } /* fall through */
		case ( 3 ): { *p = static_cast<char>( unicode::TAIL_BYTES_MARK_VALUE | ( unicode::TAIL_BYTES_VALUE_MASK & codePoint_.get() ) ); -- p; codePoint_ >>= 6; } /* fall through */
		case ( 2 ): { *p = static_cast<char>( unicode::TAIL_BYTES_MARK_VALUE | ( unicode::TAIL_BYTES_VALUE_MASK & codePoint_.get() ) ); -- p; codePoint_ >>= 6; } /* fall through */
		case ( 1 ): { *p = static_cast<char>( head[r] | codePoint_.get() ); } /* no break - fall through */
	}
}

code_point_t decode_forward( char const*& ptr_ ) {
	code_point_t character( static_cast<u8_t>( *ptr_ ) );
	static u8_t const mask[] = { 0xff, unicode::ENC_2_BYTES_VALUE_MASK, unicode::ENC_3_BYTES_VALUE_MASK, unicode::ENC_4_BYTES_VALUE_MASK };
	int tailLength( unicode::utf8_declared_length( *ptr_ ) - 1 );
	character &= mask[tailLength];
	for ( int i( 0 ); i < tailLength; ++ i ) {
		character <<= 6;
		++ ptr_;
		character |= ( static_cast<u8_t>( *ptr_ ) & unicode::TAIL_BYTES_VALUE_MASK );
	}
	++ ptr_;
	return ( character );
}

template<typename T>
void decode( T* dst_, char const* str_, HString::size_type size_ ) {
	for ( HString::size_type i( 0 ); i < size_; ++ i, ++ dst_ ) {
		*dst_ = static_cast<T>( decode_forward( str_ ).get() );
	}
	return;
}

}

}

template<>
bool HCharacterClass<char>::has( code_point_t char_ ) const {
	return (
		( char_.get() <= unicode::UCS_MAX_1_BYTE_CODE_POINT )
		&& ( ::memchr( _data, static_cast<u8_t>( char_.get() ), static_cast<size_t>( _size ) ) != nullptr )
	);
}

template<>
bool HCharacterClass<char>::hasz( code_point_t char_ ) const {
	return (
		( char_.get() <= unicode::UCS_MAX_1_BYTE_CODE_POINT )
		&& (
			( ::memchr( _data, static_cast<u8_t>( char_.get() ), static_cast<size_t>( _size ) ) != nullptr )
			|| ( char_.get() == 0 )
		)
	);
}

template<>
bool HCharacterClass<char16_t>::has( code_point_t char_ ) const {
	return (
		( char_.get() <= unicode::UCS_MAX_2_BYTE_CODE_POINT )
		&& ( find( _data, _data + _size, static_cast<u16_t>( char_.get() ) ) != ( _data + _size ) )
	);
}

template<>
bool HCharacterClass<char16_t>::hasz( code_point_t char_ ) const {
	return (
		( char_.get() <= unicode::UCS_MAX_2_BYTE_CODE_POINT )
		&& (
			( find( _data, _data + _size, static_cast<u16_t>( char_.get() ) ) != ( _data + _size ) )
			|| ( char_.get() == 0 )
		)
	);
}

template<>
HCharacterClass<char> const& character_class( CHARACTER_CLASS characterClass_ ) {
#undef D_WHITE_SPACE
#define D_WHITE_SPACE "\a\b \t\v\f\r\n"
	static character_class_t const whiteSpace = character_class_t( D_WHITE_SPACE, static_cast<int>( sizeof ( D_WHITE_SPACE ) - 1 ) );
#undef D_WHITE_SPACE
#undef D_LOWER_CASE_LETTER
#undef D_UPPER_CASE_LETTER
#undef D_LETTER
#define D_LOWER_CASE_LETTER "abcdefghijklmnopqrstuvwxyz"
#define D_UPPER_CASE_LETTER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define D_VOWEL "aAeEiIoOuUyY"
#define D_LETTER D_LOWER_CASE_LETTER D_UPPER_CASE_LETTER
#undef D_BIN_DIGIT
#define D_BIN_DIGIT "01"
#undef D_OCT_DIGIT
#define D_OCT_DIGIT D_BIN_DIGIT "234567"
#undef D_DEC_DIGIT
#define D_DEC_DIGIT D_OCT_DIGIT "89"
#undef D_HEX_DIGIT
#define D_HEX_DIGIT D_DEC_DIGIT "aAbBcCdDeEfF"
	static character_class_t const binDigit = character_class_t( D_BIN_DIGIT, static_cast<int>( sizeof ( D_BIN_DIGIT ) - 1 ) );
	static character_class_t const octDigit = character_class_t( D_OCT_DIGIT, static_cast<int>( sizeof ( D_OCT_DIGIT ) - 1 ) );
	static character_class_t const digit = character_class_t( D_DEC_DIGIT, static_cast<int>( sizeof ( D_DEC_DIGIT ) - 1 ) );
	static character_class_t const hexDigit = character_class_t( D_HEX_DIGIT, static_cast<int>( sizeof ( D_HEX_DIGIT ) - 1 ) );
	static character_class_t const letter = character_class_t( D_LETTER, static_cast<int>( sizeof ( D_LETTER ) - 1 ) );
	static character_class_t const lowerCaseLetter = character_class_t( D_LOWER_CASE_LETTER, static_cast<int>( sizeof ( D_LOWER_CASE_LETTER ) - 1 ) );
	static character_class_t const upperCaseLetter = character_class_t( D_UPPER_CASE_LETTER, static_cast<int>( sizeof ( D_UPPER_CASE_LETTER ) - 1 ) );
	static character_class_t const word = character_class_t( D_LETTER D_DEC_DIGIT "_", static_cast<int>( sizeof ( D_LETTER D_DEC_DIGIT "_" ) - 1 ) );
	static character_class_t const vowel = character_class_t( D_VOWEL, static_cast<int>( sizeof ( D_VOWEL ) - 1 ) );
#undef D_HEX_DIGIT
#undef D_DEC_DIGIT
#undef D_OCT_DIGIT
#undef D_BIN_DIGIT
#undef D_LETTER
#undef D_VOWEL
#undef D_LOWER_CASE_LETTER
#undef D_UPPER_CASE_LETTER
	static character_class_t const* characterClass[] = {
		&whiteSpace,
		&binDigit,
		&octDigit,
		&digit,
		&hexDigit,
		&letter,
		&lowerCaseLetter,
		&upperCaseLetter,
		&word,
		&vowel
	};
	return ( *characterClass[static_cast<int>( characterClass_ )] );
}

template<>
HCharacterClass<char16_t> const& character_class( CHARACTER_CLASS characterClass_ ) {
#undef D_LOWER_CASE_GREEK
#undef D_UPPER_CASE_GREEK
#undef D_GREEK
#define D_LOWER_CASE_GREEK u"αβγδεζηθικλμνξοπρστυφχψω"
#define D_UPPER_CASE_GREEK u"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩ"
#define D_GREEK D_LOWER_CASE_GREEK D_UPPER_CASE_GREEK
#undef D_SUBSCRIPT_DIGIT
#undef D_SUBSCRIPT_LOWER_CASE_LETTER
#undef D_SUBSCRIPT_UPPER_CASE_LETTER
#undef D_SUBSCRIPT_LETTER
#undef D_SUBSCRIPT
#define D_SUBSCRIPT_DIGIT u"₀₁₂₃₄₅₆₇₈₉"
#define D_SUBSCRIPT_LOWER_CASE_LETTER u"ₐₑₕᵢⱼₖₗₘₙₒₚᵣₛₜᵤᵥₓ"
#define D_SUBSCRIPT_UPPER_CASE_LETTER u""
#define D_SUBSCRIPT_LETTER D_SUBSCRIPT_LOWER_CASE_LETTER D_SUBSCRIPT_UPPER_CASE_LETTER
#define D_SUBSCRIPT D_SUBSCRIPT_DIGIT D_SUBSCRIPT_LETTER
#undef D_SUPERSCRIPT_DIGIT
#undef D_SUPERSCRIPT_LOWER_CASE_LETTER
#undef D_SUPERSCRIPT_UPPER_CASE_LETTER
#undef D_SUPERSCRIPT_LETTER
#undef D_SUPERSCRIPT
#define D_SUPERSCRIPT_DIGIT u"⁰¹²³⁴⁵⁶⁷⁸⁹"
#define D_SUPERSCRIPT_LOWER_CASE_LETTER u"ᵃᵇᶜᵈᵉᶠᵍʰⁱʲᵏˡᵐⁿᵒᵖʳˢᵗᵘᵛʷˣʸᶻ"
#define D_SUPERSCRIPT_UPPER_CASE_LETTER u"ᴬᴮᴰᴱᴳᴴᴵᴶᴷᴸᴹᴺᴼᴾᴿᵀᵁⱽᵂ"
#define D_SUPERSCRIPT_LETTER D_SUPERSCRIPT_LOWER_CASE_LETTER D_SUPERSCRIPT_UPPER_CASE_LETTER
#define D_SUPERSCRIPT D_SUPERSCRIPT_DIGIT D_SUPERSCRIPT_LETTER
	typedef HCharacterClass<char16_t> cc_t;
	static cc_t const greek = cc_t( D_GREEK, size( D_GREEK ) - 1 );
	static cc_t const lowerCaseGreek = cc_t( D_LOWER_CASE_GREEK, size( D_LOWER_CASE_GREEK ) - 1 );
	static cc_t const upperCaseGreek = cc_t( D_UPPER_CASE_GREEK, size( D_UPPER_CASE_GREEK ) - 1 );
	static cc_t const subscript = cc_t( D_SUBSCRIPT, size( D_SUBSCRIPT ) - 1 );
	static cc_t const subscriptDigit = cc_t( D_SUBSCRIPT_DIGIT, size( D_SUBSCRIPT_DIGIT ) - 1 );
	static cc_t const subscriptLetter = cc_t( D_SUBSCRIPT_LETTER, size( D_SUBSCRIPT_LETTER ) - 1 );
	static cc_t const subscriptLowerCaseLetter = cc_t( D_SUBSCRIPT_LOWER_CASE_LETTER, size( D_SUBSCRIPT_LOWER_CASE_LETTER ) - 1 );
	static cc_t const subscriptUpperCaseLetter = cc_t( D_SUBSCRIPT_UPPER_CASE_LETTER, size( D_SUBSCRIPT_UPPER_CASE_LETTER ) - 1 );
	static cc_t const superscript = cc_t( D_SUPERSCRIPT, size( D_SUPERSCRIPT ) - 1 );
	static cc_t const superscriptDigit = cc_t( D_SUPERSCRIPT_DIGIT, size( D_SUPERSCRIPT_DIGIT ) - 1 );
	static cc_t const superscriptLetter = cc_t( D_SUPERSCRIPT_LETTER, size( D_SUPERSCRIPT_LETTER ) - 1 );
	static cc_t const superscriptLowerCaseLetter = cc_t( D_SUPERSCRIPT_LOWER_CASE_LETTER, size( D_SUPERSCRIPT_LOWER_CASE_LETTER ) - 1 );
	static cc_t const superscriptUpperCaseLetter = cc_t( D_SUPERSCRIPT_UPPER_CASE_LETTER, size( D_SUPERSCRIPT_UPPER_CASE_LETTER ) - 1 );
#undef D_SUPERSCRIPT_DIGIT
#undef D_SUPERSCRIPT_LOWER_CASE_LETTER
#undef D_SUPERSCRIPT_UPPER_CASE_LETTER
#undef D_SUPERSCRIPT_LETTER
#undef D_SUPERSCRIPT
#undef D_SUBSCRIPT_DIGIT
#undef D_SUBSCRIPT_LOWER_CASE_LETTER
#undef D_SUBSCRIPT_UPPER_CASE_LETTER
#undef D_SUBSCRIPT_LETTER
#undef D_SUBSCRIPT
#undef D_GREEK
#undef D_UPPER_CASE_GREEK
#undef D_LOWER_CASE_GREEK
	static cc_t const* characterClass[] = {
		&greek,
		&lowerCaseGreek,
		&upperCaseGreek,
		&subscript,
		&subscriptDigit,
		&subscriptLetter,
		&subscriptLowerCaseLetter,
		&subscriptUpperCaseLetter,
		&superscript,
		&superscriptDigit,
		&superscriptLetter,
		&superscriptLowerCaseLetter,
		&superscriptUpperCaseLetter
	};
	return ( *characterClass[static_cast<int>( characterClass_ ) - static_cast<int>( CHARACTER_CLASS::GREEK )] );
}

static yaal::u8_t const ALLOC_BIT_MASK = meta::obinary<010000000>::value;
static yaal::u8_t const RANK_BIT_MASK =  meta::obinary<001100000>::value;
#undef IS_INPLACE
#define IS_INPLACE ( ! ( this->_mem[ ALLOC_FLAG_INDEX ] & ALLOC_BIT_MASK ) )
#undef GET_RANK
#define GET_RANK ( ( ( this->_mem[ ALLOC_FLAG_INDEX ] & RANK_BIT_MASK ) >> 5 ) + 1 )
#undef EXT_IS_INPLACE
#define EXT_IS_INPLACE( base ) ( ! ( base._mem[ ALLOC_FLAG_INDEX ] & ALLOC_BIT_MASK ) )
#undef EXT_GET_RANK
#define EXT_GET_RANK( base ) ( ( ( base._mem[ ALLOC_FLAG_INDEX ] & RANK_BIT_MASK ) >> 5 ) + 1 )
#undef MEM
#define MEM ( IS_INPLACE ? this->_mem : this->_ptr )
#undef EXT_MEM
#define EXT_MEM( base ) ( EXT_IS_INPLACE( base ) ? base._mem : base._ptr )
#undef GET_SIZE
#define GET_SIZE ( IS_INPLACE ? ( this->_mem[ ALLOC_FLAG_INDEX ] & ~RANK_BIT_MASK ) : static_cast<size_type>( this->_len[ 1 ] ) )
#undef EXT_GET_SIZE
#define EXT_GET_SIZE( base ) ( EXT_IS_INPLACE( base ) ? ( base._mem[ ALLOC_FLAG_INDEX ] & ~RANK_BIT_MASK ) : static_cast<size_type>( base._len[ 1 ] ) )
#undef SET_RANK
#define SET_RANK( rank ) ( this->_mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( ( this->_mem[ ALLOC_FLAG_INDEX ] & ~RANK_BIT_MASK ) | ( ( ( rank ) - 1 ) << 5 ) ) )
#undef SET_SIZE
#define SET_SIZE( size ) \
	do { \
		( IS_INPLACE ? this->_mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( ( this->_mem[ ALLOC_FLAG_INDEX ] & RANK_BIT_MASK ) | static_cast<char>( size ) ) : this->_len[ 1 ] = ( size ) ); \
		if ( ( size ) == 0 ) { \
			SET_RANK( 1 ); \
		} \
	} while ( 0 )
#undef GET_ALLOC_BYTES
#undef SET_ALLOC_BYTES
#if TARGET_CPU_BITS >= 64
# define GET_ALLOC_BYTES ( IS_INPLACE ? MAX_INPLACE_CAPACITY : this->_len[ 2 ] & static_cast<size_type>( static_cast<int long unsigned>( -1 ) >> 3 ) )
# define SET_ALLOC_BYTES( capacity ) \
	do { \
		this->_len[ 2 ] = ( capacity ); this->_mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( this->_mem[ ALLOC_FLAG_INDEX ] | ALLOC_BIT_MASK ); \
	} while ( 0 )
#else /* #if TARGET_CPU_BITS >= 64 */
# define GET_ALLOC_BYTES ( IS_INPLACE ? MAX_INPLACE_CAPACITY : ( this->_len[ 2 ] & static_cast<size_type>( static_cast<int long unsigned>( -1 ) >> 3 ) ) << 3 )
# define SET_ALLOC_BYTES( capacity ) \
	do { \
		this->_len[ 2 ] = ( ( capacity ) >> 3 ); this->_mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( this->_mem[ ALLOC_FLAG_INDEX ] | ALLOC_BIT_MASK ); \
	} while ( 0 )
#endif /* #else #if TARGET_CPU_BITS >= 64 */

namespace {

namespace hidden {
char const* _errMsgHString_[ 7 ] = {
	( "ok" ),
	( "nullptr pointer used for string operations" ),
	( "use of uninitialized string" ),
	( "index out of bound" ),
	( "bad length" ),
	( "bad offset" ),
	( "overflow" )
};
}

char const* err_msg( int no_ ) {
	char const* em( _( hidden::_errMsgHString_[no_] ) );
	return ( em ? em : hidden::_errMsgHString_[no_] );
}

}

#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002 */

HString::HString( void )
	: _len() {
	M_PROLOG
	return;
	M_EPILOG
}

HString::~HString( void ) {
	M_PROLOG
	if ( ! IS_INPLACE ) {
		memory::free( _ptr );
	}
	return;
	M_DESTRUCTOR_EPILOG
}

HString::HString( HString const& string_ )
	: _len() {
	M_PROLOG
	assign( string_ );
	return;
	M_EPILOG
}

HString::HString( HString&& string_ ) noexcept
	: _len() {
	::memcpy( _mem, string_._mem, INPLACE_BUFFER_SIZE );
	::memset( string_._mem, 0, INPLACE_BUFFER_SIZE );
}

HString::HString( size_type preallocate_, code_point_t fill_ )
	: _len() {
	M_PROLOG
	assign( preallocate_, fill_ );
	return;
	M_EPILOG
}

/*
 * What are possible scenarios that should be tested?
 *
 * S(s, r)[x] - a string of size s and rank r, x tells shows true rank per character (or none if does not matter)
 * r(s, r) - request for size s and rank r
 *
 * S(old) -> oper(req) = S(res)
 *
 * S(0, 2) - is impossible by definition, empty string must have a rank == 1
 *
 * S(0, 1) -> reserve(0, 1) = S(0, 1)
 * S(0, 1) -> reserve(0, 2) = S(0, 1)
 * S(0, 1) -> reserve(1, 2) = S(1, 2)
 * S(2, 1) -> resize(0, 1) = S(0, 1)
 * S(2, 2) -> resize(0, 1) = S(0, 1)
 * S(2, 2) -> resize(0, 2) = S(0, 1)
 * S(2, 2)[22] -> reserve(2, 1) = ex
 * S(2, 2)[22] -> reserve(1, 1) = ex
 * S(2, 2)[12] -> reserve(1, 1) = S(2, 1) // needs down-ranking with trimming
 * S(2, 2)[11] -> reserve(2, 1) = S(2, 1)
 * S(2, 2)[11) -> reserve(1, 1) = S(2, 1)
 */
void HString::reserve( size_type const preallocate_, int const rank_ ) {
	M_PROLOG
	M_ASSERT( ( rank_ >= 0 ) && ( rank_ <= 4 ) && ( rank_ != 3 ) );
	if ( ( preallocate_ < 0 ) || ( preallocate_ > MAX_STRING_LENGTH ) ) {
		M_THROW( _( "bad new buffer size requested" ), preallocate_ );
	}
	/* Increase requested buffer size to accommodate space terminating NIL. */
	size_type oldAllocBytes( GET_ALLOC_BYTES );
	int oldRank( GET_RANK );
#if 0
	size_type reqAllocBytes( safe_int::mul( preallocate_, rank_ ) );
#endif
	size_type reqAllocBytes( preallocate_ * rank_ );
	if ( reqAllocBytes > MAX_STRING_LENGTH ) {
		M_THROW( _( "requested allocation size is too big" ), preallocate_ );
	}
	if ( reqAllocBytes > oldAllocBytes ) {
		size_type newAllocBytes = 1;
		while ( newAllocBytes < reqAllocBytes ) {
			newAllocBytes <<= 1;
		}
#if TARGET_CPU_BITS < 64
		/*
		 * 7 because we drop 3 youngest bits in SET_ALLOC_BYTES.
		 * We drop those bits to make room for `rank` bits
		 * and `IN_PLACE` mark bit.
		 */
		M_ASSERT( ! ( newAllocBytes & 7 ) );
#endif /* #if TARGET_CPU_BITS < 64 */
		if ( ! IS_INPLACE ) {
			_ptr = memory::realloc<char>( _ptr, newAllocBytes );
			SET_ALLOC_BYTES( newAllocBytes );
			SET_RANK( rank_ );
			::std::memset( static_cast<char*>( _ptr ) + oldAllocBytes, 0, static_cast<size_t>( newAllocBytes - oldAllocBytes ) );
		} else {
			char* newMem( memory::calloc<char>( newAllocBytes ) );
			size_type origSize( GET_SIZE );
			::std::memcpy( newMem, _mem, static_cast<size_t>( origSize * oldRank ) );
			_ptr = newMem;
			SET_ALLOC_BYTES( newAllocBytes );
			SET_SIZE( origSize );
			SET_RANK( rank_ );
		}
	} else {
		SET_RANK( ( preallocate_ > 0 ) ? rank_ : 1 );
	}
	if ( rank_ > oldRank ) {
		adaptive::copy_backward( MEM, rank_, 0, MEM, oldRank, 0, GET_SIZE );
	} else if ( rank_ < oldRank ) {
		adaptive::copy( MEM, rank_, 0, MEM, oldRank, 0, GET_SIZE );
	}
	return;
	M_EPILOG
}

void HString::reserve( size_type preallocate_ ) {
	M_PROLOG
	reserve( preallocate_, GET_RANK );
	return;
	M_EPILOG
}

void HString::resize( size_type const preallocate_, int const rank_ ) {
	M_PROLOG
	if ( preallocate_ > 0 ) {
		reserve( max( preallocate_, GET_SIZE ), rank_ );
	}
	SET_SIZE( preallocate_ );
	return;
	M_EPILOG
}

HString::HString( HUTF8String const& str_ )
	: _len() {
	M_PROLOG
	size_type newSize( str_.character_count() );
	int rank( str_.rank() );
	resize( newSize, rank ? rank : 1 );
	switch ( rank ) {
		case ( 0 ): { /* ASCII */
			::memcpy( MEM, str_.raw(), static_cast<size_t>( newSize ) );
		} break;
		case ( 1 ): { /* code point value: 128 - 256 */
			copy_n_unpack( str_.begin(), newSize, static_cast<yaal::u8_t*>( MEM ) );
		} break;
		case ( 2 ): {
			copy_n_unpack( str_.begin(), newSize, static_cast<yaal::u16_t*>( MEM ) );
		} break;
		case ( 4 ): {
			copy_n_unpack( str_.begin(), newSize, static_cast<yaal::u32_t*>( MEM ) );
		} break;
	}
	return;
	M_EPILOG
}

void HString::from_utf8( size_type offset_, size_type onto_, const char* str_, size_type size_ ) {
	M_PROLOG
	size_type oldSize( GET_SIZE );
	M_ASSERT( offset_ <= oldSize );
	try {
		utf8::OUTF8StringStats s( utf8::get_string_stats( str_, size_ ) );
		int rank( max( GET_RANK, s._rank ) );
		size_type newSize( ( oldSize - onto_ ) + s._characterCount );
		resize( newSize, rank );
		adaptive::move( MEM, offset_ + s._characterCount, MEM, offset_ + onto_, rank, ( oldSize - offset_ ) - onto_ );
		switch ( rank ) {
			case ( 1 ): {
				utf8::decode( static_cast<yaal::u8_t*>( MEM ) + offset_, str_, s._characterCount );
			} break;
			case ( 2 ): {
				utf8::decode( static_cast<yaal::u16_t*>( MEM ) + offset_, str_, s._characterCount );
			} break;
			case ( 4 ): {
				utf8::decode( static_cast<yaal::u32_t*>( MEM ) + offset_, str_, s._characterCount );
			} break;
		}
	} catch ( HUTF8StringException const& e ) {
		throw HStringException( e.what() );
	}
	return;
	M_EPILOG
}

HString::HString( char const* str_ )
	: _len() {
	M_PROLOG
	size_type len( str_ ? static_cast<size_type>( ::std::strlen( str_ ) ) : 0 );
	assign( str_, len );
	return;
	M_EPILOG
}

HString::HString( HConstIterator first_, HConstIterator last_ )
	: _len() {
	M_PROLOG
	size_type newSize( last_ - first_ );
	int rank( EXT_GET_RANK( (*first_._owner) ) );
	resize( newSize, rank );
	adaptive::copy( MEM, rank, 0, EXT_MEM( (*first_._owner) ), rank, first_._index, newSize );
	return;
	M_EPILOG
}

HString::HString( char const* array_, size_type size_ )
	: _len() {
	M_PROLOG
	assign( array_, size_ );
	return;
	M_EPILOG
}

HString::HString( char16_t const* array_, size_type size_ )
	: _len() {
	M_PROLOG
	assign( array_, size_ );
	return;
	M_EPILOG
}

HString::HString( char32_t const* array_, size_type size_ )
	: _len() {
	M_PROLOG
	assign( array_, size_ );
	return;
	M_EPILOG
}

HString::HString( code_point_t codePoint_ )
	: _len() {
	M_PROLOG
	if ( codePoint_.get() ) {
		int rank( unicode::rank( codePoint_ ) );
		resize( 1, rank );
		adaptive::set( MEM, rank, 0, codePoint_ );
	}
	return;
	M_EPILOG
}

HString::HString( char char_ )
	: _len() {
	M_PROLOG
	if ( char_ ) {
		resize( 1, 1 );
		_mem[ 0 ] = char_;
	}
	return;
	M_EPILOG
}

HString::HString( char unsigned charUnsigned_ )
	: _len() {
	M_PROLOG
	if ( charUnsigned_ ) {
		resize( 1, 1 );
		_mem[ 0 ] = static_cast<char>( charUnsigned_ );
	}
	return;
	M_EPILOG
}

HString::HString( int short shortInt_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%hd", shortInt_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%hd", shortInt_ ) == newSize );
	SET_SIZE( newSize ); /* explicit SET_SIZE here is required because we use reserve and not resize */
	return;
	M_EPILOG
}

HString::HString( int short unsigned unsignedShortInt_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%hu", unsignedShortInt_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%hu", unsignedShortInt_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int int_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%d", int_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%d", int_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int unsigned int_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%u", int_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%u", int_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long long_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%ld", long_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%ld", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long unsigned long_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%lu", long_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%lu", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long long longLong_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%lld", longLong_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%lld", longLong_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long long unsigned longLong_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%llu", longLong_ ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "%llu", longLong_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( double double_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%f", double_ ) );
	reserve( newSize + 1, 1 );
	char* buf( static_cast<char*>( MEM ) );
	M_ENSURE( ::snprintf( buf, static_cast<size_t>( newSize + 1 ), "%f", double_ ) == newSize );
	if ( ( newSize >= 3 /* "0.0" */ ) && ( strchr( buf, '.' ) != nullptr ) ) {
		while ( ( buf[newSize - 1] == '0' ) && ( buf[newSize - 2] != '.' ) ) {
			-- newSize;
		}
		buf[newSize] = 0;
	}
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( double long double_ )
	: _len() {
	M_PROLOG
	size_type newSize( ::snprintf( nullptr, 0, "%.12Lf", double_ ) );
	reserve( newSize + 1, 1 );
	char* buf( static_cast<char*>( MEM ) );
	M_ENSURE( ::snprintf( buf, static_cast<size_t>( newSize + 1 ), "%.12Lf", double_ ) == newSize );
	if ( ( newSize >= 3 /* "0.0" */ ) && ( strchr( buf, '.' ) != nullptr ) ) {
		while ( ( buf[newSize - 1] == '0' ) && ( buf[newSize - 2] != '.' ) ) {
			-- newSize;
		}
		buf[newSize] = 0;
	}
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( void const* ptrVoid_ )
	: _len() {
	M_PROLOG
	/*
	 * Solaris libc omits 0x in %p conversion.
	 * Well, that sucks.
	 */
	size_type newSize( ::snprintf( nullptr, 0, "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) );
	reserve( newSize + 1, 1 );
	M_ENSURE( ::snprintf( static_cast<char*>( MEM ), static_cast<size_t>( newSize + 1 ), "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002
# pragma GCC diagnostic pop
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002 */

HString& HString::operator = ( HString const& string_ ) {
	M_PROLOG
	return ( assign( string_ ) );
	M_EPILOG
}

HString& HString::operator = ( HString&& string_ ) {
	M_PROLOG
	if ( &string_ != this ) {
		if ( ! IS_INPLACE ) {
			memory::free( _ptr );
		}
		::memcpy( _mem, string_._mem, INPLACE_BUFFER_SIZE );
		::memset( string_._mem, 0, INPLACE_BUFFER_SIZE );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::operator += ( HString const& string_ ) {
	M_PROLOG
	append( string_ );
	return ( *this );
	M_EPILOG
}

HString HString::operator + ( HString const& other_ ) const {
	M_PROLOG
	HString str( *this );
	return ( str += other_ );
	M_EPILOG
}

code_point_t const HString::operator[] ( size_type index_ ) const {
	M_PROLOG
	if ( ( index_ < 0 ) || ( index_ >= GET_SIZE ) ) {
		M_THROW( err_msg( ERROR::INDEX_OOB ), index_ );
	}
	return ( adaptive::get( MEM, GET_RANK, index_ ) );
	M_EPILOG
}

void HString::set_at( size_type index_, code_point_t char_ ) {
	M_PROLOG
	size_type curSize( GET_SIZE );
	if ( ( index_ < 0 ) || ( index_ >= curSize ) ) {
		M_THROW( err_msg( ERROR::INDEX_OOB ), index_ );
	}
	int rank( GET_RANK );
	int charRank( unicode::rank( char_ ) );
	if ( charRank > rank ) {
		resize( curSize, rank = charRank );
	}
	adaptive::set( MEM, rank, index_, char_ );
	if ( ! char_ ) {
		SET_SIZE( index_ );
	}
	return;
	M_EPILOG
}

bool HString::operator == ( HString const& other_ ) const {
	M_PROLOG
	size_type thisSize( GET_SIZE );
	size_type otherSize( EXT_GET_SIZE( other_ ) );
	return (
		( this == &other_ )
		|| (
			( thisSize == otherSize )
			&& ( adaptive::compare( MEM, GET_RANK, 0, thisSize, EXT_MEM( other_ ), EXT_GET_RANK( other_ ), 0, otherSize ) == 0 )
		)
	);
	M_EPILOG
}

bool HString::operator != (  HString const& other_ ) const {
	M_PROLOG
	return ( ! operator == ( other_ ) );
	M_EPILOG
}

bool HString::operator > ( HString const& other_ ) const {
	M_PROLOG
	return (
		adaptive::compare( MEM, GET_RANK, 0, GET_SIZE, EXT_MEM( other_ ), EXT_GET_RANK( other_ ), 0, EXT_GET_SIZE( other_ ) ) > 0
	);
	M_EPILOG
}

bool HString::operator < ( HString const& other_ ) const {
	M_PROLOG
	return (
		adaptive::compare( MEM, GET_RANK, 0, GET_SIZE, EXT_MEM( other_ ), EXT_GET_RANK( other_ ), 0, EXT_GET_SIZE( other_ ) ) < 0
	);
	M_EPILOG
}

bool HString::operator >= ( HString const& other_ ) const {
	M_PROLOG
	return (
		adaptive::compare( MEM, GET_RANK, 0, GET_SIZE, EXT_MEM( other_ ), EXT_GET_RANK( other_ ), 0, EXT_GET_SIZE( other_ ) ) >= 0
	);
	M_EPILOG
}

bool HString::operator <= ( HString const& other_ ) const {
	M_PROLOG
	return (
		adaptive::compare( MEM, GET_RANK, 0, GET_SIZE, EXT_MEM( other_ ), EXT_GET_RANK( other_ ), 0, EXT_GET_SIZE( other_ ) ) <= 0
	);
	M_EPILOG
}

HString operator + ( char const* left_, HString const& right_ ) {
	M_PROLOG
	HString tmp( left_ );
	tmp.operator += ( right_ );
	return ( tmp );
	M_EPILOG
}

bool operator == ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator == ( left_ ) );
	M_EPILOG
}

bool operator != ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator != ( left_ ) );
	M_EPILOG
}

bool operator > ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator < ( left_ ) );
	M_EPILOG
}

bool operator < ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator > ( left_ ) );
	M_EPILOG
}

bool operator >= ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator <= ( left_ ) );
	M_EPILOG
}

bool operator <= ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator >= ( left_ ) );
	M_EPILOG
}

int HString::compare( HString const& other_, size_type from_, size_type len_ ) const {
	M_PROLOG
	return ( compare( 0, GET_SIZE, other_, from_, len_ ) );
	M_EPILOG
}

int HString::compare( size_type thisFrom_, size_type thisLen_, HString const& other_, size_type from_, size_type len_ ) const {
	M_PROLOG
	size_type s( GET_SIZE );
	size_type os( EXT_GET_SIZE( other_ ) );
	if ( ( thisFrom_ < 0 ) || ( thisFrom_ > s ) ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), thisFrom_ );
	}
	if ( ( thisLen_ < 0 ) || ( ( thisFrom_ + thisLen_ ) > s ) ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), thisLen_ );
	}
	if ( len_ == npos ) {
		len_ = os - from_;
	}
	if ( ( from_ < 0 ) || ( from_ > os ) ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), from_ );
	}
	if ( ( len_ < 0 ) || ( ( from_ + len_ ) > os ) ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), len_ );
	}
	return ( adaptive::compare( MEM, GET_RANK, thisFrom_, thisLen_, EXT_MEM( other_ ), EXT_GET_RANK( other_ ), from_, len_ ) );
	M_EPILOG
}

bool HString::starts_with( HString const& other_ ) const {
	size_type os( EXT_GET_SIZE( other_ ) );
	return ( ( os <= GET_SIZE ) && ( compare( 0, os, other_, 0 ) == 0 ) );
}

bool HString::ends_with( HString const& other_ ) const {
	size_type s( GET_SIZE );
	size_type os( EXT_GET_SIZE( other_ ) );
	return ( ( os <= s ) && ( compare( s - os, os, other_ ) == 0 ) );
}

code_point_t HString::front( void ) const {
	if ( GET_SIZE == 0 ) {
		M_THROW( err_msg( ERROR::INDEX_OOB ), 0 );
	}
	return ( adaptive::get( MEM, GET_RANK, 0 ) );
}

code_point_t HString::back( void ) const {
	size_type s( GET_SIZE );
	if ( s == 0 ) {
		M_THROW( err_msg( ERROR::INDEX_OOB ), s - 1 );
	}
	return ( adaptive::get( MEM, GET_RANK, s - 1 ) );
}

HString::const_iterator HString::begin( void ) const {
	return ( HConstIterator( this, 0 ) );
}

HString::const_iterator HString::cbegin( void ) const {
	return ( HConstIterator( this, 0 ) );
}

HString::const_iterator HString::end( void ) const {
	return ( HConstIterator( this, get_length() ) );
}

HString::const_iterator HString::cend( void ) const {
	return ( HConstIterator( this, get_length() ) );
}

HString::iterator HString::begin( void ) {
	return ( iterator( this, 0 ) );
}

HString::iterator HString::end( void ) {
	return ( iterator( this, get_length() ) );
}

bool HString::empty( void ) const {
	return ( is_empty() );
}

bool HString::is_empty( void ) const {
	return ( ! GET_SIZE );
}

HString::operator bool ( void ) const {
	return ( ! is_empty() );
}

void HString::clear( void ) {
	SET_SIZE( 0 );
	return;
}

HString::size_type HString::get_length( void ) const {
	M_PROLOG
	M_ASSERT( ! GET_SIZE || ( GET_SIZE <= GET_ALLOC_BYTES ) );
	return ( GET_SIZE );
	M_EPILOG
}

HString::size_type HString::length( void ) const {
	M_PROLOG
	return ( get_length() );
	M_EPILOG
}

HString::size_type HString::get_size( void ) const {
	M_PROLOG
	return ( get_length() );
	M_EPILOG
}

HString::size_type HString::size( void ) const {
	M_PROLOG
	return ( get_length() );
	M_EPILOG
}

HString::size_type HString::capacity( void ) const {
	M_PROLOG
	return ( get_capacity() );
	M_EPILOG
}

HString::size_type HString::get_capacity( void ) const {
	M_PROLOG
	return ( GET_ALLOC_BYTES );
	M_EPILOG
}

HString::size_type HString::get_max_size( void ) const {
	return ( MAX_STRING_LENGTH );
}

HString::size_type HString::max_size( void ) const {
	return ( MAX_STRING_LENGTH );
}

void HString::swap( HString& other ) {
	using yaal::swap;
	swap( _len, other._len );
	return;
}

HString& HString::assign( HString const& str_, size_type offset_, size_type length_ ) {
	M_PROLOG
	if ( length_ == npos ) {
		length_ = MAX_STRING_LENGTH;
	}
	if ( length_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), length_ );
	}
	if ( offset_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), offset_ );
	}
	size_type s( EXT_GET_SIZE( str_ ) );
	if ( offset_ > s ) {
		offset_ = s;
	}
	size_type newSize( min( length_, s - offset_ ) );
	int rank( EXT_GET_RANK( str_ ) );
	clear();
	resize( newSize, rank );
	adaptive::copy( MEM, rank, 0, EXT_MEM( str_ ), rank, offset_, newSize );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( char const* data_, size_type length_ ) {
	M_PROLOG
	if ( ! data_ ) {
		M_THROW( err_msg(  ERROR::NULL_PTR  ), errno );
	}
	if ( length_ < 0 ) {
		M_THROW( err_msg(  ERROR::BAD_LENGTH  ), length_ );
	}
	size_type len( static_cast<size_type>( ::strnlen( data_, static_cast<size_t>( length_ ) ) ) );
	clear();
	from_utf8( 0, GET_SIZE, data_, len );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( void const* data_, size_type size_ ) {
	M_PROLOG
	if ( ! data_ ) {
		M_THROW( err_msg(  ERROR::NULL_PTR  ), errno );
	}
	if ( size_ < 0 ) {
		M_THROW( err_msg(  ERROR::BAD_LENGTH  ), size_ );
	}
	clear();
	int rank( 1 );
	resize( size_, rank );
	adaptive::copy( MEM, rank, 0, data_, rank, 0, size_ );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( char16_t const* data_, size_type size_ ) {
	M_PROLOG
	if ( ! data_ ) {
		M_THROW( err_msg(  ERROR::NULL_PTR  ), errno );
	}
	if ( size_ < 0 ) {
		M_THROW( err_msg(  ERROR::BAD_LENGTH  ), size_ );
	}
	clear();
	int rank( 2 );
	resize( size_, rank );
	adaptive::copy( MEM, rank, 0, data_, rank, 0, size_ );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( char32_t const* data_, size_type size_ ) {
	M_PROLOG
	if ( ! data_ ) {
		M_THROW( err_msg(  ERROR::NULL_PTR  ), errno );
	}
	if ( size_ < 0 ) {
		M_THROW( err_msg(  ERROR::BAD_LENGTH  ), size_ );
	}
	clear();
	int rank( 4 );
	resize( size_, rank );
	adaptive::copy( MEM, rank, 0, data_, rank, 0, size_ );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( const_iterator first_, const_iterator last_ ) {
	M_PROLOG
	if ( ! ( first_._owner && last_._owner ) ) {
		M_THROW( err_msg(  ERROR::NULL_PTR  ), errno );
	}
	if ( last_ < first_ ) {
		M_THROW( err_msg(  ERROR::BAD_LENGTH  ), last_ - first_ );
	}
	return ( assign( *first_._owner, first_._index, last_._index - first_._index ) );
	M_EPILOG
}

HString& HString::assign( size_type size_, code_point_t fill_ ) {
	M_PROLOG
	if ( size_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), size_ );
	}
	int rank( unicode::rank( fill_ ) );
	clear();
	resize( size_, rank );
	adaptive::fill( MEM, rank, 0, size_, fill_ );
	if ( ! fill_ ) {
		SET_SIZE( 0 );
	}
	return ( *this );
	M_EPILOG
}

HString::size_type HString::find( code_point_t char_, size_type after_ ) const {
	M_PROLOG
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	return ( adaptive::find( MEM, GET_RANK, GET_SIZE, after_, char_ ) );
	M_EPILOG
}

HString::size_type HString::find( HString const& pattern_, size_type after_ ) const {
	M_PROLOG
	if ( pattern_.is_empty() ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	size_type patternLength( EXT_GET_SIZE( pattern_ ) );
	if ( ( ! patternLength )
			|| ( ( after_ + patternLength ) > GET_SIZE ) ) {
		return ( npos );
	}
	return (
		adaptive::kmpsearch( MEM, GET_RANK, GET_SIZE, after_, EXT_MEM( pattern_ ), EXT_GET_RANK( pattern_ ), patternLength )
	);
	M_EPILOG
}

HString::size_type HString::find_one_of( HString const& set_, size_type after_ ) const {
	M_PROLOG
	if ( set_.is_empty() ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	return ( adaptive::find_one_of( MEM, GET_RANK, GET_SIZE, after_, EXT_MEM( set_ ), EXT_GET_RANK( set_ ), EXT_GET_SIZE( set_ ) ) );
	M_EPILOG
}

HString::size_type HString::reverse_find_one_of( HString const& set_, size_type before_ ) const {
	M_PROLOG
	if ( set_.is_empty() ) {
		return ( npos );
	}
	if ( before_ < 0 ) {
		before_ = 0;
	}
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	size_type pos( find_last_one_of( set_, ( GET_SIZE - before_ ) - 1 ) );
	return ( pos != npos ? ( GET_SIZE - 1 ) - pos : pos );
	M_EPILOG
}

HString::size_type HString::find_last_one_of( HString const& set_, size_type before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( ( before_ < 0 ) || set_.is_empty() ) {
		return ( npos );
	}
	return ( adaptive::find_last_one_of( MEM, GET_RANK, before_ + 1, EXT_MEM( set_ ), EXT_GET_RANK( set_ ), EXT_GET_SIZE( set_ ) ) );
	M_EPILOG
}

HString::size_type HString::find_other_than( HString const& set_, size_type after_ ) const {
	M_PROLOG
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( set_.is_empty() ) {
		return ( after_ );
	}
	return ( adaptive::find_other_than( MEM, GET_RANK, GET_SIZE, after_, EXT_MEM( set_ ), EXT_GET_RANK( set_ ), EXT_GET_SIZE( set_ ) ) );
	M_EPILOG
}

HString::size_type HString::reverse_find_other_than( HString const& set_, size_type before_ ) const {
	M_PROLOG
	if ( before_ < 0 ) {
		before_ = 0;
	}
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( set_.is_empty() ) {
		return ( before_ );
	}
	size_type pos( find_last_other_than( set_, ( GET_SIZE - before_ ) - 1 ) );
	return ( pos != npos ? ( GET_SIZE - 1 ) - pos : pos );
	M_EPILOG
}

HString::size_type HString::find_last_other_than( HString const& set_, size_type before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	if ( set_.is_empty() ) {
		return ( before_ );
	}
	return ( adaptive::find_last_other_than( MEM, GET_RANK, before_ + 1, EXT_MEM( set_ ), EXT_GET_RANK( set_ ), EXT_GET_SIZE( set_ ) ) );
	M_EPILOG
}

HString::size_type HString::reverse_find( code_point_t char_, size_type before_ ) const {
	M_PROLOG
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( before_ < 0 ) {
		before_ = 0;
	}
	size_type pos( find_last( char_, ( GET_SIZE - before_ ) - 1 ) );
	return ( pos != npos ? ( GET_SIZE - 1 ) - pos : pos );
	M_EPILOG
}

HString::size_type HString::find_last( code_point_t char_, size_type before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	return ( adaptive::find_last( MEM, GET_RANK, before_ + 1, char_ ) );
	M_EPILOG
}

HString::size_type HString::find_last( HString const& pattern_, size_type before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	size_type patternLength( EXT_GET_SIZE( pattern_ ) );
	if ( ( ! patternLength ) || ( patternLength > before_ ) ) {
		return ( npos );
	}
	return (
		adaptive::kmpsearch_last( MEM, GET_RANK, before_ + 1, EXT_MEM( pattern_ ), EXT_GET_RANK( pattern_ ), patternLength )
	);
	M_EPILOG
}

HString& HString::replace( HString const& pattern_, HString const& with_ ) {
	M_PROLOG
	if ( pattern_.is_empty() ) {
		return ( *this );
	}
	typedef yaal::hcore::HArray<size_type> pattern_offsets_t;
	pattern_offsets_t patternOffsets;
	size_type lenPattern( pattern_.get_length() );
	size_type patPos( 0 );
	while ( ( patPos = find( pattern_, patPos ) ) != npos ) {
		patternOffsets.push_back( patPos );
		patPos += lenPattern;
	}
	size_type lenWith( with_.get_length() );
	size_type subWP( lenWith - lenPattern );
	size_type oldSize( GET_SIZE );
	size_type newSize( oldSize + subWP * patternOffsets.get_size() );
	int withRank( EXT_GET_RANK( with_ ) );
	int rank( max( GET_RANK, withRank ) );
	resize( newSize, rank );
	void* dst( MEM );
	void const* src( EXT_MEM( with_ ) );
	size_type extraOffset( 0 );
	if ( newSize > oldSize ) {
		extraOffset = newSize - oldSize;
		adaptive::move( dst, extraOffset, dst, 0, rank, oldSize );
	}
	size_type srcOffset( 0 );
	size_type dstOffset( 0 );
	for ( size_type offset : patternOffsets ) {
		size_type unchangedLength( offset - srcOffset );
		adaptive::move( dst, dstOffset, dst, srcOffset + extraOffset, rank, unchangedLength );
		dstOffset += unchangedLength;
		srcOffset += unchangedLength;
		adaptive::copy( dst, rank, dstOffset, src, withRank, 0, lenWith );
		dstOffset += lenWith;
		srcOffset += lenPattern;
	}
	adaptive::move( dst, dstOffset, dst, srcOffset + extraOffset, rank, oldSize - srcOffset );
	return ( *this );
	M_EPILOG
}

HString& HString::replace( size_type pos_, size_type size_, HString const& replacement_ ) {
	M_PROLOG
	return ( replace( pos_, size_, replacement_, 0, replacement_.get_length() ) );
	M_EPILOG
}

void HString::replace_check( size_type thisOffset_, size_type onto_, size_type srcOffset_, size_type srcUsedSize_, size_type srcRealSize_ ) {
	M_PROLOG
	if ( srcOffset_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), srcOffset_ );
	}
	if ( onto_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), onto_ );
	}
	if ( srcUsedSize_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), srcUsedSize_ );
	}
	if ( thisOffset_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), thisOffset_ );
	}
	size_type oldSize( GET_SIZE );
	if ( ( thisOffset_ + onto_ ) > oldSize ) {
		M_THROW( err_msg( ERROR::OVERFLOW ), thisOffset_ + onto_ );
	}
	if ( srcOffset_ > srcRealSize_ ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), srcOffset_ );
	}
	return;
	M_EPILOG
}

HString& HString::replace( size_type thisOffset_, size_type onto_, HString const& replacement_, size_type srcOffset_, size_type srcUsedSize_ ) {
	M_PROLOG
	if ( srcUsedSize_ == npos ) {
		srcUsedSize_ = MAX_STRING_LENGTH;
	}
	replace_check( thisOffset_, onto_, srcOffset_, srcUsedSize_, replacement_.get_length() );
	if ( ( srcOffset_ + srcUsedSize_ ) > replacement_.get_length() ) {
		srcUsedSize_ = replacement_.get_length() - srcOffset_;
	}
	size_type oldSize( GET_SIZE );
	int withRank( EXT_GET_RANK( replacement_ ) );
	int rank( GET_RANK );
	size_type newSize( oldSize + ( srcUsedSize_ - onto_ ) );
	resize( newSize, rank = max( rank, withRank ) );
	adaptive::move( MEM, thisOffset_ + srcUsedSize_, MEM, thisOffset_ + onto_, rank, oldSize - ( thisOffset_ + onto_ ) );
	adaptive::copy( MEM, rank, thisOffset_, EXT_MEM( replacement_ ), withRank, srcOffset_, srcUsedSize_ );
	return ( *this );
	M_EPILOG
}

HString& HString::replace( size_type pos_, size_type onto_, size_type count_, code_point_t value_ ) {
	M_PROLOG
	replace_check( pos_, onto_, 0, 0, MAX_STRING_LENGTH );
	size_type oldSize( GET_SIZE );
	int withRank( unicode::rank( value_ ) );
	int rank( GET_RANK );
	size_type newSize( oldSize + ( count_ - onto_ ) );
	resize( newSize, rank = max( rank, withRank ) );
	adaptive::move( MEM, pos_ + count_, MEM, pos_ + onto_, rank, oldSize - ( pos_ + onto_ ) );
	adaptive::fill( MEM, rank, pos_, count_, value_ );
	return ( *this );
	M_EPILOG
}

HString& HString::replace( size_type pos_, size_type onto_, char const* buffer_, size_type len_ ) {
	M_PROLOG
	replace_check( pos_, onto_, 0, len_, len_ );
	from_utf8( pos_, onto_, buffer_, len_ );
	return ( *this );
	M_EPILOG
}

HString& HString::replace( iterator first_, iterator last_, HString const& replacement_ ) {
	M_PROLOG
	M_ENSURE( first_._owner == this );
	M_ENSURE( last_._owner == this );
	return ( replace( first_._index, last_ - first_, replacement_ ) );
	M_EPILOG
}

HString& HString::upper( void ) {
	M_PROLOG
	adaptive::transform( MEM, GET_RANK, GET_SIZE, &::std::towupper );
	return ( *this );
	M_EPILOG
}

HString& HString::lower( void ) {
	M_PROLOG
	adaptive::transform( MEM, GET_RANK, GET_SIZE, &::std::towlower );
	return ( *this );
	M_EPILOG
}

namespace {
wint_t case_swap( wint_t char_ ) {
	wint_t c( 0 );
	wint_t u( towupper( char_ ) );
	if ( u != char_ ) {
		c = u;
	} else {
		c = towlower( char_ );
	}
	return ( c );
}
}

HString& HString::reverse( void ) {
	M_PROLOG
	adaptive::transform( MEM, GET_RANK, GET_SIZE, &case_swap );
	return ( *this );
	M_EPILOG
}

void HString::substr( HString& dest_, size_type from_, size_type length_ ) const {
	M_PROLOG
	if ( length_ == npos ) {
		length_ = MAX_STRING_LENGTH;
	}
	if ( from_ < 0 ) {
		length_ += from_;
		from_ = 0;
	}
	if ( ( length_ > 0 ) && ( from_ < GET_SIZE ) ) {
		size_type newSize( min( length_, GET_SIZE ) );
		if ( ( newSize + from_ ) > GET_SIZE ) {
			newSize = GET_SIZE - from_;
		}
		int rank( GET_RANK );
		dest_.resize( newSize, rank );
		adaptive::copy( EXT_MEM( dest_ ), rank, 0, MEM, rank, from_, newSize );
	}
	return;
	M_EPILOG
}

HString HString::substr( size_type from_, size_type length_ ) const {
	M_PROLOG
	HString str;
	substr( str, from_, length_ );
	return ( str );
	M_EPILOG
}

HString HString::left( size_type to_ ) const {
	M_PROLOG
	HString str;
	if ( to_ < 1 ) {
		return ( str );
	}
	size_type newSize( min( to_, GET_SIZE ) );
	int rank( GET_RANK );
	str.resize( newSize, rank );
	adaptive::copy( EXT_MEM( str ), rank, 0, MEM, rank, 0, newSize );
	return ( str );
	M_EPILOG
}

HString HString::mid( size_type from_, size_type length_ ) const {
	M_PROLOG
	HString str;
	substr( str, from_, length_ );
	return ( str );
	M_EPILOG
}

HString HString::right( size_type fromEnd_ ) const {
	M_PROLOG
	HString str;
	if ( fromEnd_ < 1 ) {
		return ( str );
	}
	size_type newSize( min( fromEnd_, GET_SIZE ) );
	int rank( GET_RANK );
	str.resize( newSize, rank );
	adaptive::copy( EXT_MEM( str ), rank, 0, MEM, rank, GET_SIZE - newSize, newSize );
	return ( str );
	M_EPILOG
}

HString& HString::trim_left( HString const& set_ ) {
	M_PROLOG
	size_type cut( find_other_than( set_ ) );
	if ( cut != npos ) {
		shift_left( cut );
	} else {
		clear();
	}
	return ( *this );
	M_EPILOG
}

HString& HString::trim_right( HString const& set_ ) {
	M_PROLOG
	size_type cut( find_last_other_than( set_ ) );
	if ( cut != npos ) {
		SET_SIZE( cut + 1 );
	} else {
		clear();
	}
	return ( *this );
	M_EPILOG
}

HString& HString::trim( HString const& set_ ) {
	M_PROLOG
	trim_left( set_ );
	trim_right( set_ );
	return ( *this );
	M_EPILOG
}

HString& HString::shift_left( size_type shift_ ) {
	M_PROLOG
	if ( shift_ < 0 ) {
		M_THROW( "bad left shift length", shift_ );
	}
	if ( shift_ ) {
		size_type oldSize( GET_SIZE );
		if ( shift_ < oldSize ) {
			int rank( GET_RANK );
			size_type newSize( oldSize - shift_ );
			adaptive::move( MEM, 0, MEM, shift_, rank, newSize );
			SET_SIZE( newSize );
		} else {
			clear();
		}
	}
	return ( *this );
	M_EPILOG
}

HString& HString::shift_right( size_type shift_, code_point_t filler_ ) {
	M_PROLOG
	if ( shift_ < 0 ) {
		M_THROW( "bad right shift length", shift_ );
	}
	if ( shift_ ) {
		size_type oldSize( GET_SIZE );
		size_type newSize( oldSize + shift_ );
		int rank( max( GET_RANK, unicode::rank( filler_ ) ) );
		resize( newSize, rank );
		adaptive::move( MEM, shift_, MEM, 0, rank, oldSize );
		/* using SET_SIZE twice is not a bug or mistake,
		 * fill() depends on size and modifies it
		 * so it must have new size before and we need to
		 * undo fill()'s size modification after fill() is done.
		 */
		fill( filler_, 0, shift_ );
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::fill( code_point_t filler_, size_type offset_, size_type count_ ) {
	M_PROLOG
	if ( count_ == npos ) {
		count_ = ( GET_SIZE - offset_ );
	}
	if ( count_ < 0 ) {
		M_THROW( err_msg( ERROR::BAD_LENGTH ), count_ );
	}
	if ( ( offset_ < 0 ) || ( offset_ > GET_SIZE ) ) {
		M_THROW( err_msg( ERROR::BAD_OFFSET ), offset_ );
	}
	if ( filler_.get() ) {
		int rank( max( GET_RANK, unicode::rank( filler_ ) ) );
		size_type newSize( max( count_ + offset_, GET_SIZE ) );
		resize( newSize, rank );
		adaptive::fill( MEM, rank, offset_, count_, filler_ );
	} else {
		SET_SIZE( offset_ );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::fillz( code_point_t filler_, size_type offset_, size_type count_ ) {
	M_PROLOG
	fill( filler_, offset_, count_ );
	if ( count_ == npos ) {
		count_ = ( GET_SIZE - offset_ );
	}
	SET_SIZE( count_ + offset_ );
	return ( *this );
	M_EPILOG
}

HString& HString::erase( size_type from_, size_type length_ ) {
	M_PROLOG
	if ( length_ == npos ) {
		length_ = GET_SIZE;
	}
	if ( from_ < 0 ) {
		length_ += from_;
		from_ = 0;
	}
	if ( ( from_ + length_ ) > GET_SIZE ) {
		length_ = GET_SIZE - from_;
	}
	if ( ( length_ > 0 ) && ( from_ < GET_SIZE ) ) {
		int rank( GET_RANK );
		size_type oldSize( GET_SIZE );
		adaptive::move( MEM, from_, MEM, from_ + length_, rank, oldSize - ( from_ + length_ ) );
		SET_SIZE( oldSize - length_ );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::insert( size_type from_, HString const& str_, size_type offset_, size_type length_ ) {
	M_PROLOG
	return ( replace( from_, 0, str_, offset_, length_ ) );
	M_EPILOG
}

HString& HString::insert( size_type from_, HString const& str_ ) {
	M_PROLOG
	return ( insert( from_, str_, 0, str_.get_length() ) );
	M_EPILOG
}

HString& HString::insert( size_type from_, char const* chunk_, size_type length_ ) {
	M_PROLOG
	return ( replace( from_, 0, chunk_, length_ ) );
	M_EPILOG
}

HString& HString::insert( size_type from_, char const* chunk_ ) {
	M_PROLOG
	return ( insert( from_, chunk_, static_cast<size_type>( ::strlen( chunk_ ) ) ) );
	M_EPILOG
}

HString& HString::insert( size_type from_, size_type length_, code_point_t char_ ) {
	M_PROLOG
	return ( replace( from_, 0, length_, char_ ) );
	M_EPILOG
}

HString& HString::append( HString const& string_ ) {
	M_PROLOG
	return ( append( string_, 0 ) );
	M_EPILOG
}

HString& HString::append( HString const& string_, size_type from_, size_type len_ ) {
	M_PROLOG
	return ( replace( GET_SIZE, 0, string_, from_, len_ ) );
	M_EPILOG
}

HString& HString::append( size_type count_, code_point_t val_ ) {
	M_PROLOG
	return ( replace( GET_SIZE, 0, count_, val_ ) );
	M_EPILOG
}

HString& HString::append( char const* buf_, size_type len_ ) {
	M_PROLOG
	return ( replace( GET_SIZE, 0, buf_, len_ ) );
	M_EPILOG
}

HString& HString::append( const_iterator first_, const_iterator last_ ) {
	M_PROLOG
	return ( append( (*first_._owner), first_._index, last_ - first_ ) );
	M_EPILOG
}

void HString::push_back( code_point_t character_ ) {
	M_PROLOG
	append( 1, character_ );
	return;
	M_EPILOG
}

void HString::pop_back( void ) {
	M_PROLOG
	erase( GET_SIZE - 1, 1 );
	return;
	M_EPILOG
}

HString::HCharRef& HString::HCharRef::operator = ( code_point_t ch_ ) {
	M_PROLOG
	_string.set_at( _index, ch_ );
	return ( *this );
	M_EPILOG
}

void HString::HCharRef::swap( HCharRef& charRef_ ) {
	M_PROLOG
	if ( &charRef_ != this ) {
		code_point_t l( _string[_index] );
		code_point_t r( charRef_._string[charRef_._index] );
		_string.set_at( _index, r );
		charRef_._string.set_at( charRef_._index, l );
	}
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( void )
	: _characterCount( 0 )
	, _offset( 0 )
	, _byteCount( 0 )
	, _ptr( nullptr ) {
	return;
}

HUTF8String::HUTF8String( HString const& str_ )
	: HUTF8String( str_.begin(), str_.end() ) {
}

HUTF8String::HUTF8String( HString::const_iterator it_, HString::const_iterator end_ )
	: HUTF8String() {
	M_PROLOG
	assign( it_, end_ );
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( char const* str_ )
	: HUTF8String() {
	M_PROLOG
	utf8::OUTF8StringStats s( utf8::get_string_stats( str_, static_cast<size_type>( strlen( str_ ) ) ) );
	if ( s._byteCount > 0 ) {
		alloc( s._byteCount );
		_meta->_used = safe_int::cast<int>( s._byteCount );
		_meta->_rank = static_cast<i8_t>( s._rank );
		_byteCount = safe_int::cast<int>( s._byteCount );
		_characterCount = safe_int::cast<int>( s._characterCount );
		::strncpy( _ptr + sizeof ( OBufferMeta ), str_, static_cast<size_t>( s._byteCount ) );
	}
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( const_iterator beg_, const_iterator end_ )
	: HUTF8String() {
	M_PROLOG
	M_ASSERT( beg_._ptr == end_._ptr );
	if ( beg_._ptr ) {
		_ptr = beg_._ptr;
		++ _meta->_refCount;
		_characterCount = end_._characterIndex - beg_._characterIndex;
		_offset = beg_._byteIndex;
		_byteCount = end_._byteIndex - beg_._byteIndex;
	}
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( HUTF8String const& str_ )
	: _characterCount( str_._characterCount )
	, _offset( str_._offset )
	, _byteCount( str_._byteCount )
	, _ptr( str_._ptr ) {
	if ( _ptr ) {
		++ _meta->_refCount;
	}
	return;
}

HUTF8String::HUTF8String( HUTF8String&& str_ ) noexcept
	: _characterCount( yaal::move( str_._characterCount ) )
	, _offset( yaal::move( str_._offset ) )
	, _byteCount( yaal::move( str_._byteCount ) )
	, _ptr( yaal::move( str_._ptr ) ) {
	str_._ptr = nullptr;
	str_.reset();
	return;
}

HUTF8String::~HUTF8String( void ) {
	M_PROLOG
	reset();
	return;
	M_DESTRUCTOR_EPILOG
}

void HUTF8String::reset( void ) {
	M_PROLOG
	M_ASSERT( ( _ptr == nullptr ) || ( ( _ptr != nullptr ) && ( _meta->_refCount > 0 ) ) );
	if ( _ptr != nullptr ) {
		-- _meta->_refCount;
		if ( _meta->_refCount == 0 ) {
			memory::free( _ptr );
		}
	}
	_ptr = nullptr;
	_byteCount = 0;
	_offset = 0;
	_characterCount = 0;
	return;
	M_EPILOG
}

void HUTF8String::swap( HUTF8String& other_ ) {
	M_PROLOG
	using yaal::swap;
	swap( _characterCount, other_._characterCount );
	swap( _offset, other_._offset );
	swap( _byteCount, other_._byteCount );
	swap( _ptr, other_._ptr );
	return;
	M_EPILOG
}

HUTF8String& HUTF8String::operator = ( HString const& str_ ) {
	M_PROLOG
	assign( str_ );
	return ( *this );
	M_EPILOG
}

HUTF8String& HUTF8String::operator = ( HUTF8String const& str_ ) {
	M_PROLOG
	reset();
	if ( str_._ptr ) {
		HUTF8String tmp( str_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HUTF8String& HUTF8String::operator = ( HUTF8String&& other_ ) {
	swap( other_ );
	other_.reset();
	return ( *this );
}

bool HUTF8String::operator == ( yaal::hcore::HUTF8String const& other_ ) const {
	bool same( false );
	if ( ( _characterCount == other_._characterCount ) && ( _byteCount == other_._byteCount ) ) {
		if ( _ptr ) {
			same = ( !! other_._ptr ) && ( ::strncmp( _ptr + sizeof ( OBufferMeta ) + _offset, other_._ptr + sizeof ( OBufferMeta ) + other_._offset, static_cast<size_t>( _byteCount ) ) == 0 );
		} else {
			same = ! other_._ptr;
		}
	}
	return ( same );
}

bool HUTF8String::operator != ( yaal::hcore::HUTF8String const& other_ ) const {
	return ( ! operator == ( other_ ) );
}

bool HUTF8String::operator == ( char const* other_ ) const {
	size_type byteCount( static_cast<size_type>( ::strlen( other_ ) ) );
	bool same( false );
	if ( byteCount == _byteCount ) {
		if ( _ptr ) {
			same = other_ && ( ::strncmp( _ptr + sizeof ( OBufferMeta ) + _offset, other_, static_cast<size_t>( _byteCount ) ) == 0 );
		} else {
			same = ! other_;
		}
	}
	return ( same );
}

bool HUTF8String::operator != ( char const* other_ ) const {
	return ( ! operator == ( other_ ) );
}

void HUTF8String::alloc( size_type size_ ) {
	M_PROLOG
	M_ASSERT( size_ > 0 );
	if ( _ptr && ( _meta->_refCount > 1 ) ) {
		reset();
	}
	int oldSize( _ptr ? _meta->_allocated : 0 );
	if ( size_ > oldSize ) {
		size_type newSize( oldSize ? oldSize : 1 );
		while ( newSize < size_ ) {
			newSize *= 2;
		}
		int size( safe_int::cast<int>( newSize + 1 + static_cast<int>( sizeof ( OBufferMeta ) ) ) );
		_ptr = memory::realloc<char>( _ptr, size );
		::memset( _ptr + static_cast<int>( sizeof ( OBufferMeta ) ) + oldSize, 0, static_cast<size_t>( newSize - oldSize ) );
		_meta->_refCount = 1;
		_meta->_allocated = static_cast<int>( newSize );
	}
	_ptr[ static_cast<int>( sizeof ( OBufferMeta ) ) + size_ ] = 0;
	return;
	M_EPILOG
}

void HUTF8String::assign( HString::const_iterator it_, HString::const_iterator end_ ) {
	M_PROLOG
	code_point_t maxCodePoint( 0 );
	int byteCount( 0 );
	for ( HString::const_iterator it( it_ ); it != end_; ++ it ) {
		byteCount += unicode::utf8_length( *it );
		maxCodePoint = max( *it, maxCodePoint );
	}
	if ( byteCount > 0 ) {
		alloc( byteCount );
		char* p( _ptr + sizeof ( OBufferMeta ) );
		for ( HString::const_iterator it( it_ ); it != end_; ++ it ) {
			utf8::encode( *it, p );
		}
	}
	if ( _ptr ) {
		_meta->_used = byteCount;
		int r( unicode::rank( maxCodePoint ) );
		int utf8Length( unicode::utf8_length( maxCodePoint ) );
		_meta->_rank = static_cast<i8_t>( utf8Length > 1 ? r : 0 );
		char* p( _ptr + sizeof ( OBufferMeta ) );
		p[byteCount] = 0;
	}
	_characterCount = static_cast<int>( end_ - it_ );
	_offset = 0;
	_byteCount = byteCount;
	return;
	M_EPILOG
}

void HUTF8String::assign( HString const& str_ ) {
	M_PROLOG
	assign( str_.begin(), str_.end() );
	return;
	M_EPILOG
}

bool HUTF8String::is_empty( void ) const {
	return ( _characterCount == 0 );
}

bool HUTF8String::empty( void ) const {
	return ( _characterCount == 0 );
}

char const* HUTF8String::c_str( void ) const {
	M_ASSERT( ! _ptr || ( ( _offset + _byteCount ) == _meta->_used ) );
	return ( _ptr ? _ptr + sizeof ( OBufferMeta ) + _offset : "" );
}

char const* HUTF8String::raw( void ) const {
	return ( _ptr ? _ptr + sizeof ( OBufferMeta ) + _offset : "" );
}

HUTF8String::size_type HUTF8String::byte_count( void ) const {
	return ( _byteCount );
}

HUTF8String::size_type HUTF8String::character_count( void ) const {
	return ( _characterCount );
}

int HUTF8String::rank( void ) const {
	return ( _ptr ? _meta->_rank : 0 );
}

HUTF8String::HIterator HUTF8String::begin( void ) const {
	return ( HIterator( _ptr, _offset, 0 ) );
}

HUTF8String::HIterator HUTF8String::end( void ) const {
	return ( HIterator( _ptr, _offset + _byteCount, _characterCount ) );
}

HUTF8String::HIterator HUTF8String::cbegin( void ) const {
	return ( HIterator( _ptr, _offset, 0 ) );
}

HUTF8String::HIterator HUTF8String::cend( void ) const {
	return ( HIterator( _ptr, _offset + _byteCount, _characterCount ) );
}

HUTF8String::reverse_iterator HUTF8String::rbegin( void ) const {
	return ( end() );
}

HUTF8String::reverse_iterator HUTF8String::rend( void ) const {
	return ( begin() );
}

HUTF8String::reverse_iterator HUTF8String::crbegin( void ) const {
	return ( end() );
}

HUTF8String::reverse_iterator HUTF8String::crend( void ) const {
	return ( begin() );
}

HUTF8String HUTF8String::substr( size_type from_, size_type len_ ) const {
	M_PROLOG
	if ( len_ == HString::npos ) {
		len_ = HString::MAX_STRING_LENGTH;
	}
	if ( from_ < 0 ) {
		from_ = 0;
	}
	if ( ( len_ + from_ ) > _characterCount ) {
		len_ = _characterCount - from_;
	}
	HIterator it( begin() );
	if ( from_ >= _characterCount ) {
		from_ = 0;
		len_ = 0;
		it = end();
	} else {
		it += from_;
	}
	HIterator endIt( len_ < ( _characterCount - from_ ) ? it + len_ : end() );
	HUTF8String s( *this );
	s._offset = it._byteIndex;
	s._byteCount = endIt._byteIndex - it._byteIndex;
	s._characterCount = safe_int::cast<int>( len_ );
	return ( s );
	M_EPILOG
}

code_point_t HUTF8String::front( void ) const {
	M_PROLOG
	M_ASSERT( _ptr );
	if ( _characterCount < 1 ) {
		M_THROW( err_msg( ERROR::INDEX_OOB ), 0 );
	}
	char const* ptr( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _offset );
	return ( utf8::decode_forward( ptr ) );
	M_EPILOG
}

bool operator == ( char const* left_, HUTF8String const& right_ ) {
	M_PROLOG
	return ( right_.operator == ( left_ ) );
	M_EPILOG
}

bool operator != ( char const* left_, HUTF8String const& right_ ) {
	M_PROLOG
	return ( right_.operator != ( left_ ) );
	M_EPILOG
}

HUTF8String::HIterator::HIterator( void )
	: _ptr( nullptr )
	, _characterIndex( -1 )
	, _byteIndex( 0 ) {
	return;
}

HUTF8String::HIterator::HIterator( HIterator const& other_ )
	: _ptr( other_._ptr )
	, _characterIndex( other_._characterIndex )
	, _byteIndex( other_._byteIndex ) {
	if ( _ptr ) {
		++ _meta->_refCount;
	}
	return;
}

HUTF8String::HIterator::HIterator( HIterator&& other_ ) noexcept
	: _ptr( yaal::move( other_._ptr ) )
	, _characterIndex( yaal::move( other_._characterIndex ) )
	, _byteIndex( yaal::move( other_._byteIndex ) ) {
	other_._ptr = nullptr;
	other_.reset();
	return;
}

HUTF8String::HIterator::HIterator( char* data_, int byteIndex_, int characterIndex_ )
	: _ptr( data_ )
	, _characterIndex( characterIndex_ )
	, _byteIndex( byteIndex_ ) {
	M_PROLOG
	if ( _ptr ) {
		++ _meta->_refCount;
	}
	return;
	M_EPILOG
}

HUTF8String::HIterator::~HIterator( void ) {
	M_PROLOG
	reset();
	return;
	M_DESTRUCTOR_EPILOG
}

HUTF8String::HIterator& HUTF8String::HIterator::operator = ( HIterator const& other_ ) {
	M_PROLOG
	reset();
	if ( other_._ptr ) {
		HIterator tmp( other_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HUTF8String::HIterator& HUTF8String::HIterator::operator = ( HIterator&& other_ ) {
	M_PROLOG
	swap( other_ );
	other_.reset();
	return ( *this );
	M_EPILOG
}

void HUTF8String::HIterator::swap( HIterator& other_ ) {
	using yaal::swap;
	swap( _ptr, other_._ptr );
	swap( _characterIndex, other_._characterIndex );
	swap( _byteIndex, other_._byteIndex );
	return;
}

void HUTF8String::HIterator::reset( void ) {
	M_PROLOG
	M_ASSERT( ( _ptr == nullptr ) || ( ( _ptr != nullptr ) && ( _meta->_refCount > 0 ) ) );
	if ( _ptr ) {
		-- _meta->_refCount;
		if ( _meta->_refCount == 0 ) {
			memory::free( _ptr );
		}
	}
	_ptr = nullptr;
	_byteIndex = 0;
	_characterIndex = 0;
	return;
	M_EPILOG
}

HUTF8String::HIterator& HUTF8String::HIterator::operator ++ ( void ) {
	M_ASSERT( _ptr );
	char const* p( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _byteIndex );
	int inc( 1 );
	if ( ! ( *p & unicode::ENC_1_BYTES_MARK_MASK ) ) {
	} else if ( ( *p & unicode::ENC_2_BYTES_MARK_MASK ) == unicode::ENC_2_BYTES_MARK_VALUE ) {
		inc = 2;
	} else if ( ( *p & unicode::ENC_3_BYTES_MARK_MASK ) == unicode::ENC_3_BYTES_MARK_VALUE ) {
		inc = 3;
	} else {
		inc = 4;
	}
	_byteIndex += inc;
	++ _characterIndex;
	return ( *this );
}

HUTF8String::HIterator& HUTF8String::HIterator::operator -- ( void ) {
	M_ASSERT( _ptr );
	-- _byteIndex;
	char const* p( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _byteIndex );
	while ( ( *p & unicode::TAIL_BYTES_MARK_MASK ) == unicode::TAIL_BYTES_MARK_VALUE ) {
		-- _byteIndex;
		-- p;
	}
	-- _characterIndex;
	return ( *this );
}

HUTF8String::HIterator& HUTF8String::HIterator::operator += ( size_type by_ ) {
	if ( by_ > 0 ) {
		while ( by_ -- ) {
			operator ++ ();
		}
	} else {
		while ( by_ ++ ) {
			operator -- ();
		}
	}
	return ( *this );
}

HUTF8String::HIterator& HUTF8String::HIterator::operator -= ( size_type by_ ) {
	if ( by_ > 0 ) {
		while ( by_ -- ) {
			operator -- ();
		}
	} else {
		while ( by_ ++ ) {
			operator ++ ();
		}
	}
	return ( *this );
}

code_point_t HUTF8String::HIterator::operator * ( void ) const {
	M_ASSERT( _ptr );
	char const* ptr( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _byteIndex );
	return ( utf8::decode_forward( ptr ) );
}

#undef SET_ALLOC_BYTES
#undef GET_ALLOC_BYTES
#undef SET_SIZE
#undef GET_SIZE
#undef EXT_MEM_off
#undef MEM_off
#undef EXT_IS_INPLACE
#undef IS_INPLACE

HString operator "" _ys ( char const* str_, size_t len_ ) {
	return ( HString( str_, static_cast<int>( len_ ) ) );
}

HString to_string( char val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}
HString to_string( char unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int short val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int short unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long long val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long long unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( float val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( double val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( double long val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( char const* val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( void const* val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

namespace {

void copy_ascii_impl( HString::const_iterator it_, HString::const_iterator end_, char* buffer_, int size_, bool all_ ) {
	for (
		int i( 0 ), LIMIT( min( size_ - 1, static_cast<int>( end_ - it_ ) ) );
		( i < LIMIT ) && ( all_ || is_ascii( *it_ ) );
		++ i, ++ it_, ++ buffer_
	) {
		*buffer_ = safe_int::cast<char>( (*it_).get() );
	}
	*buffer_ = 0;
	return;
}

}

void copy_ascii( HString const& string_, char* buffer_, int size_ ) {
	M_PROLOG
	copy_ascii( string_.begin(), string_.end(), buffer_, size_ );
	return;
	M_EPILOG
}

void copy_ascii( HString::const_iterator it_, HString::const_iterator end_, char* buffer_, int size_ ) {
	M_PROLOG
	copy_ascii_impl( it_, end_, buffer_, size_, true );
	return;
	M_EPILOG
}

void copy_all_to_ascii( HString const& string_, char* buffer_, int size_ ) {
	M_PROLOG
	copy_all_to_ascii( string_.begin(), string_.end(), buffer_, size_ );
	return;
	M_EPILOG
}

void copy_all_to_ascii( HString::const_iterator it_, HString::const_iterator end_, char* buffer_, int size_ ) {
	M_PROLOG
	copy_ascii_impl( it_, end_, buffer_, size_, false );
	return;
	M_EPILOG
}

int stoi( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_int::cast<int>( stoll( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

int long stol( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_int::cast<int long>( stoll( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

int long unsigned stoul( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_int::cast<int long unsigned>( stoull( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

namespace hidden {

int copy_digits( HString const&, char*, int );

}

namespace {

static int const MAX_INTEGER_DIGIT_COUNT( 160 );

}

int long long stoll_impl( char const*, int*, int );
int long long stoll_impl( char const* str_, int* endIdx_, int base_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	char* end( nullptr );
	int long long val( ::strtoll( str_, &end, base_ ) );
	if ( ! val || ( val == LLONG_MIN ) || ( val == LLONG_MAX ) ) {
		if ( ( errno == EINVAL ) || ( end == str_ ) ) {
			throw HInvalidArgumentException( "Invalid argument in conversion: "_ys.append( str_ ) );
		} else if ( errno == ERANGE ) {
			throw HOutOfRangeException( "Out of range value in conversion: "_ys.append( str_ ) );
		}
	}
	if ( endIdx_ ) {
		*endIdx_ = static_cast<int>( end - str_ );
	}
	return ( val );
	M_EPILOG
}

int long long stoll( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	char buf[MAX_INTEGER_DIGIT_COUNT];
	int skip( hcore::hidden::copy_digits( str_, buf, MAX_INTEGER_DIGIT_COUNT ) );
	int long long result( stoll_impl( buf, endIdx_, base_ ) );
	if ( endIdx_ ) {
		*endIdx_ += skip;
	}
	return ( result );
	M_EPILOG
}

int long long unsigned stoull_impl( char const*, int*, int );
int long long unsigned stoull_impl( char const* str_, int* endIdx_, int base_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	char* end( nullptr );
	int long long unsigned val( ::strtoull( str_, &end, base_ ) );
	if ( ! val || ( val == ULLONG_MAX ) ) {
		if ( ( errno == EINVAL ) || ( end == str_ ) ) {
			throw HInvalidArgumentException( "Invalid argument in conversion: "_ys.append( str_ ) );
		} else if ( errno == ERANGE ) {
			throw HOutOfRangeException( "Out of range value in conversion: "_ys.append( str_ ) );
		}
	}
	if ( endIdx_ ) {
		*endIdx_ = static_cast<int>( end - str_ );
	}
	return ( val );
	M_EPILOG
}

int long long unsigned stoull( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	char buf[MAX_INTEGER_DIGIT_COUNT];
	int skip( hcore::hidden::copy_digits( str_, buf, MAX_INTEGER_DIGIT_COUNT ) );
	int long long unsigned result( stoull_impl( buf, endIdx_, base_ ) );
	if ( endIdx_ ) {
		*endIdx_ += skip;
	}
	return ( result );
	M_EPILOG
}

float stof( HString const& str_, int* endIdx_ ) {
	return ( static_cast<float>( stold( str_, endIdx_ ) ) );
}

double stod( HString const& str_, int* endIdx_ ) {
	return ( static_cast<double>( stold( str_, endIdx_ ) ) );
}

double long stold( HString const& str_, int* endIdx_ ) {
	/*
	 * Try to call smart version of float parser if it is available.
	 *
	 * hcore::strtold can redirect call to either:
	 *
	 * ::strtold() - standard C library float parser
	 *
	 * - or -
	 *
	 * yaal::atof_ex - a full fledged expression parser (if it is available by linking with yaal::tools library)
	 */
	return ( hcore::strtold( str_, endIdx_ ) );
}

bool is_whitespace( code_point_t char_ ) {
	return ( ( char_.get() <= ' ' ) && character_class<CHARACTER_CLASS::WHITESPACE>().has( char_ ) );
}

bool is_digit( code_point_t char_ ) {
	return ( ( char_.get() >= '0' ) && ( char_.get() <= '9' ) );
}

bool is_dec_digit( code_point_t char_ ) {
	return ( ( char_.get() >= '0' ) && ( char_.get() <= '9' ) );
}

bool is_hex_digit( code_point_t char_ ) {
	return ( character_class<CHARACTER_CLASS::HEX_DIGIT>().has( char_ ) );
}

bool is_oct_digit( code_point_t char_ ) {
	return ( ( char_.get() >= '0' ) && ( char_.get() <= '7' ) );
}

bool is_bin_digit( code_point_t char_ ) {
	return ( ( char_.get() >= '0' ) && ( char_.get() <= '1' ) );
}

bool is_letter( code_point_t char_ ) {
	code_point_t::value_type ch( char_.get() );
	return ( ( ( ch >= 'a' ) && ( ch <= 'z' ) ) || ( ( ch >= 'A' ) && ( ch <= 'Z' ) ) );
}

bool is_upper( code_point_t char_ ) {
	return ( ::std::iswupper( static_cast<wint_t>( char_.get() ) ) ? true : false );
}

bool is_lower( code_point_t char_ ) {
	return ( ::std::iswlower( static_cast<wint_t>( char_.get() ) ) ? true : false );
}

bool is_alpha( code_point_t char_ ) {
	return ( ::std::iswalpha( static_cast<wint_t>( char_.get() ) ) ? true : false );
}

bool is_alnum( code_point_t char_ ) {
	return ( ::std::iswalnum( static_cast<wint_t>( char_.get() ) ) ? true : false );
}

bool is_ascii( code_point_t char_ ) {
	return ( char_ <= unicode::UTF8_MAX_1_BYTE_CODE_POINT );
}

bool is_greek( code_point_t char_ ) {
	code_point_t::value_type cp( char_.get() );
	return (
		( cp >= unicode::CODE_POINT::GREEK_CAPITAL_LETTER_ALPHA )
		&& ( cp <= unicode::CODE_POINT::GREEK_SMALL_LETTER_OMEGA )
		&& ( cp != 0x03a2u )
	);
}

bool is_upper_greek( code_point_t char_ ) {
	code_point_t::value_type cp( char_.get() );
	return (
		( cp >= unicode::CODE_POINT::GREEK_CAPITAL_LETTER_ALPHA )
		&& ( cp <= unicode::CODE_POINT::GREEK_CAPITAL_LETTER_OMEGA )
		&& ( cp != 0x03a2u )
	);
}

bool is_lower_greek( code_point_t char_ ) {
	code_point_t::value_type cp( char_.get() );
	return (
		( cp >= unicode::CODE_POINT::GREEK_SMALL_LETTER_ALPHA )
		&& ( cp <= unicode::CODE_POINT::GREEK_SMALL_LETTER_OMEGA )
	);
}

bool is_subscript( code_point_t char_ ) {
	return ( character_class<CHARACTER_CLASS::SUBSCRIPT>().has( char_ ) );
}

bool is_superscript( code_point_t char_ ) {
	return ( character_class<CHARACTER_CLASS::SUPERSCRIPT>().has( char_ ) );
}

inline wint_t to_lower( code_point_t codePoint_ ) {
	return ( ::std::towlower( static_cast<wint_t>( codePoint_.get() ) ) );
}

HString::size_type icasesearch( HString const& haystack_, HString const& needle_ ) {
	HString::size_type haystackLength_( haystack_.get_length() );
	HString::size_type needleLength_( needle_.get_length() );
	KMP_SEARCH(,,, to_lower )
}

int stricasecmp( HString const& left_, HString const& right_ ) {
	HString::size_type len( min( left_.get_length(), right_.get_length() ) );
	int diff( 0 );
	for ( HString::size_type i( 0 ); ( diff == 0 ) && ( i < len ); ++ i ) {
		diff = static_cast<int>( towlower( static_cast<wint_t>( left_[i].get() ) ) ) - static_cast<int>( towlower( static_cast<wint_t>( right_[i].get() ) ) );
	}
	return ( diff ? diff : ( left_.get_size() == right_.get_size() ? 0 : ( left_.get_size() < right_.get_size() ? -1 : 1 ) ) );
}

}

}

