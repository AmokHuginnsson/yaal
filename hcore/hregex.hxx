/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hregex.hxx
 * \brief HRegex class related declarations.
 *
 * HRegex::HMatchIterator and HRegex::HMatch classes are included here.
 */

#ifndef YAAL_HCORE_HREGEX_HXX_INCLUDED
#define YAAL_HCORE_HREGEX_HXX_INCLUDED 1

#include "config.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hbitflag.hxx"
#include "hcore/harray.hxx"
#include "hcore/hboundcall.hxx"

namespace yaal {

namespace hcore {

/*! \brief Regular expression string searcher.
 */
class HRegex final {
public:
	struct COMPILE;
	typedef HBitFlag<COMPILE> compile_t;
	struct COMPILE {
		static M_YAAL_HCORE_PUBLIC_API compile_t const NONE;
		static M_YAAL_HCORE_PUBLIC_API compile_t const IGNORE_CASE;
#undef NEWLINE
		static M_YAAL_HCORE_PUBLIC_API compile_t const NEWLINE;
		static M_YAAL_HCORE_PUBLIC_API compile_t const NO_EXCEPTION;
		static M_YAAL_HCORE_PUBLIC_API compile_t const DEFAULT;
	};
	struct MATCH;
	typedef HBitFlag<MATCH> match_t;
	struct MATCH {
		static M_YAAL_HCORE_PUBLIC_API match_t const NONE;
		static M_YAAL_HCORE_PUBLIC_API match_t const NOT_BEGINNING_OF_LINE;
		static M_YAAL_HCORE_PUBLIC_API match_t const NOT_END_OF_LINE;
		static M_YAAL_HCORE_PUBLIC_API match_t const OVERLAPPING;
		static M_YAAL_HCORE_PUBLIC_API match_t const DEFAULT;
	};
#undef NO_MATCH
	static int const NO_MATCH = -1;
	class HMatchResult;
	class HMatch;
	class HMatchIterator;
	typedef HMatchIterator iterator;
	typedef HRegex this_type;
	typedef yaal::hcore::HArray<HMatch> groups_t;
	typedef yaal::hcore::HBoundCall<yaal::hcore::HString ( yaal::hcore::HString const& )> replacer_t;
private:
	HString _pattern;       /*!< original regex pattern */
	compile_t _flags;       /*!< flags used to compile this regex */
	void*   _impl;          /*!< compiled regular expression for search patterns */
	void*   _extra;         /*!< extra implementation data */
	mutable int _lastError;
	/* All fields below are conceptually a memory cache. */
	HString _errorMessage;  /*!< error message of last operation */
	mutable HUTF8String _utf8ConversionCache;
public:
	/*! \brief Create uninitialized regex object.
	 */
	HRegex( void );
	HRegex( HRegex&& );
	HRegex& operator = ( HRegex&& );

	/*! \brief Construct regex with regex rule.
	 *
	 * \param regex - regex pattern to construct regex object.
	 * \param flags - configure various aspects of regex behavior.
	 */
	HRegex( HString const& regex, compile_t flags = COMPILE::DEFAULT );

	/*! \brief Destroy regex object and free all resources.
	 */
	~HRegex( void );

	/*! \brief Parse regular expression pattern with configuration and prepare for matches retrieval.
	 *
	 * \param regex - regular expression pattern to match against.
	 * \param flags - configure various aspects of regex behavior.
	 */
	bool compile( HString const& regex, compile_t flags = COMPILE::DEFAULT );

	/*! \brief Test if this regex object is a valid regex object.
	 *
	 * \return True iff this regex object is valid.
	 */
	bool is_valid( void ) const;
	/*! \brief Get original regex patter for this regex object.
	 *
	 * \return Original regex pattern.
	 */
	HString const& pattern( void ) const;

	HString const& error( void ) const;
	int error_code( void ) const;

	/*! \brief Find substring matching this patter in given string.
	 *
	 * \param string_ - string to search through.
	 * \return HMatchIterator pointing to first match or end of matches HMatchIterator.
	 *
	 * Parameter must be raw memory pointer for HMatchIterator returns
	 * raw memory pointers to input string while dereferenced.
	 */
	HMatchIterator find( HUTF8String const& string_, match_t = MATCH::DEFAULT ) const;
	HMatchIterator end( void ) const;
	/*! \brief Tell if given string matches regex.
	 *
	 * \param string_ - string to check against the regex.
	 * \return True iff given string matches this regex.
	 */
	HMatchResult matches( HUTF8String const& string_, match_t = MATCH::DEFAULT ) const;
	groups_t groups( HUTF8String const& string_, match_t = MATCH::DEFAULT ) const;
	yaal::hcore::HString replace( yaal::hcore::HString const&, yaal::hcore::HString const&, match_t = MATCH::DEFAULT ) const;
	yaal::hcore::HString replace( yaal::hcore::HString const&, replacer_t const&, match_t = MATCH::DEFAULT ) const;
	void swap( HRegex& );
	void clear( void );
	HRegex copy( void ) const;
private:
	HRegex( HRegex const& ) = delete;
	HRegex& operator = ( HRegex const& ) = delete;
	void error_clear( void );
	void reset( void );
	HUTF8String matches_impl( HUTF8String const&, int&, int&, match_t ) const;
	groups_t groups_impl( HUTF8String const&, match_t ) const;
};

/*! \brief Instance of single match for given regex.
 */
class HRegex::HMatch {
	int _start;
	int _size;
public:
	HMatch( int, int );
	int start() const;
	int size() const;
private:
	friend class HRegex::HMatchIterator;
};

/*! \brief Pattern matches iterator.
 */
class HRegex::HMatchIterator : public iterator_interface<HMatch, iterator_category::forward> {
	HRegex const* _owner;
	HRegex::match_t _flags;
	HUTF8String _string;
	HRegex::HMatch _match;
public:
	typedef iterator_interface<HMatch, iterator_category::forward> base_type;
	HMatch const* operator->( void ) const;
	HMatch const& operator*( void ) const;
	bool operator != ( HMatchIterator const& ) const;
	bool operator == ( HMatchIterator const& ) const;
	HMatchIterator& operator ++ ( void );
	HMatchIterator( HMatchIterator const& );
	HMatchIterator& operator = ( HMatchIterator const& );
private:
	HMatchIterator( HRegex const*, match_t, HUTF8String const&, int, int );
	friend class HRegex;
};

/*! \brief Abstraction of HRegex::matches() result.
 */
class HRegex::HMatchResult {
	struct SemanticContext { SemanticContext const& member( SemanticContext& ) const { return ( *this ); } };
	typedef SemanticContext const& ( SemanticContext::* safe_bool_t )( SemanticContext& ) const;
	HRegex::HMatchIterator _it;
	HRegex::HMatchIterator _end;
public:
	HRegex::HMatchIterator begin( void ) const;
	HRegex::HMatchIterator end( void ) const;
	HRegex::HMatchIterator cbegin( void ) const;
	HRegex::HMatchIterator cend( void ) const;
	operator safe_bool_t() const;
private:
	HMatchResult( HRegex::HMatchIterator const&, HRegex::HMatchIterator const& );
	friend class HRegex;
};

typedef HExceptionT<HRegex> HRegexException;

HRegex operator "" _yr ( char const*, size_t );

inline void swap( HRegex& a, HRegex& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HREGEX_HXX_INCLUDED */

