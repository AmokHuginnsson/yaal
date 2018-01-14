/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hconsole/hpattern.hxx
 * \brief HPattern class related declarations.
 */

#ifndef YAAL_HCONSOLE_HPATTERN_HXX_INCLUDED
#define YAAL_HCONSOLE_HPATTERN_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hregex.hxx"
#include "hcore/hpair.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace hconsole {

/*! \brief String pattern searcher helper.
 */
class HPattern {
public:
	class HMatch;
	class HMatchIterator;
	typedef HMatchIterator iterator;
	typedef yaal::hcore::HPair<char, bool*> flag_desc_t;
	typedef yaal::hcore::HArray<flag_desc_t> pluggable_flags_t;
	typedef HPattern this_type;
#undef NO_MATCH
	static int const NO_MATCH = yaal::hcore::HRegex::NO_MATCH;
private:
	bool _initialized;             /*!< is pattern initialized */
	bool _ignoreCaseDefault;       /*!< default value for ignore case switch */
	bool _ignoreCase;              /*!< self explanatory */
	bool _extended;                /*!< simple strstr or extended RE */
	int  _simpleMatchLength;       /*!< length of simple strstr pattern */
	yaal::hcore::HRegex  _regex;   /*!< regular expression for search patterns */
	yaal::hcore::HString _pattern; /*!< pattern with stripped switches */
	mutable yaal::hcore::HString _errorCause; /*!< cause of last error */
public:
	HPattern( void );
	virtual ~HPattern( void );
	/*! \brief Parse regular expression pattern with configuration and prepare for matches retrieval.
	 *
	 * pattern - regular expression pattern to match against.
	 * externalFlags - set of external flags that may by set by pattern extra arguments.
	 */
	bool parse( yaal::hcore::HString const& pattern, pluggable_flags_t* externalFlags = nullptr );
	/*! \brief Parse naked regular expression pattern and prepare for matches retrieval.
	 *
	 * pattern - regular expression pattern to match against.
	 */
	bool parse_re( yaal::hcore::HString const& pattern );
	yaal::hcore::HString const& error( void ) const;
	int error_code( void ) const;
	/*! \brief Find substring matching this patter in given string.
	 *
	 * \param string_ - string to search through.
	 * \return HMatchIterator pointing to first match or end of matches HMatchIterator.
	 *
	 * Parameter must be raw memory pointer for HMatchIterator returns
	 * raw memory pointers to input string while dereferenced.
	 */
	HMatchIterator find( yaal::hcore::HString const& string_ ) const;
	HMatchIterator end( void ) const;
	/*! \brief Tell if given string matches pattern.
	 *
	 * \param string_ - string to check against the pattern.
	 * \return True iff given string matches this pattern.
	 */
	bool matches( yaal::hcore::HString const& string_ ) const;
private:
	int matches( yaal::hcore::HString const&, int& /* match length */ ) const;
	bool set_switch( char const, pluggable_flags_t* );
	void save_state( void*, pluggable_flags_t* );
	void restore_state( void*, pluggable_flags_t* );
};

/*! \brief Instance of single match for given pattern.
 */
class HPattern::HMatch {
	int _start;
	int _size;
public:
	int start() const;
	int size() const;
private:
	HMatch( int, int );
	friend class HPattern::HMatchIterator;
};

/*! \brief Pattern matches iterator.
 */
class HPattern::HMatchIterator : public yaal::hcore::iterator_interface<HMatch, yaal::hcore::iterator_category::forward> {
	HPattern const* _owner;
	HPattern::HMatch _match;
public:
	typedef yaal::hcore::iterator_interface<HMatch, yaal::hcore::iterator_category::forward> base_type;
	HMatch const* operator->( void ) const;
	HMatch const& operator*( void ) const;
	bool operator != ( HMatchIterator const& ) const;
	bool operator == ( HMatchIterator const& ) const;
	HMatchIterator& operator ++ ( void );
	HMatchIterator( HMatchIterator const& );
	HMatchIterator& operator = ( HMatchIterator const& );
private:
	HMatchIterator( HPattern const*, int, int );
	friend class HPattern;
};

typedef yaal::hcore::HExceptionT<HPattern> HPatternException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HPATTERN_HXX_INCLUDED */

