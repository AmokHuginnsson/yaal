/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hpattern.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hpattern.hxx
 * \brief HPattern class related declarations.
 *
 * HPattern::HMatchIterator and HPattern::HMatch classes are included here.
 */

#ifndef YAAL_HCORE_HPATTERN_HXX_INCLUDED
#define YAAL_HCORE_HPATTERN_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hpair.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace hcore {

/*! \brief Regular expression string searcher.
 */
class HPattern {
public:
	class HMatch;
	class HMatchIterator;
	typedef HMatchIterator iterator;
	typedef HPair<char, bool*> flag_desc_t;
	typedef HArray<flag_desc_t> pluggable_flags_t;
	typedef HPattern this_type;
private:
	bool		_initialized;          /*!< is pattern initialized */
	bool		_ignoreCaseDefault;    /*!< default value for ignore case switch */
	bool		_ignoreCase;           /*!< self explanary */
	bool		_extended;             /*!< simple strstr or extended RE */
	int			_simpleMatchLength;    /*!< length of simple strstr pattern */
	HChunk	_compiled;             /*!< compiled regular expression for
																      search patterns */
	HString	_patternInput;         /*!< current search pattern */
	HString	_patternReal;          /*!< pattern with stripped switches */
	mutable int _lastError;
	mutable HChunk _errorBuffer;   /*!< buffer for error message */
	mutable HString _errorCause;   /*!< cause of last error */
	mutable HString _errorMessage; /*!< error message of last operation */
public:
	HPattern( void );
	HPattern( char const* const );
	HPattern( HString const& );
	/*! \brief Construct pattern with regex rule.
	 *
	 * \param pattern - regex rule to construct pattern object.
	 * \param ignoreCase - shall defualt match policy be: case matters, default ignore case state
	 */
	HPattern( HString const& pattern, bool ignoreCase );
	virtual ~HPattern( void );
	/*! \brief Parse regular expression pattern with configuration and prepare for matches retrieval.
	 *
	 * pattern - regular expression pattern to match against.
	 * externalFlags - set of external flags that may by set by pattern extra arguments.
	 */
	int parse( HString const& pattern, pluggable_flags_t* externalFlags = NULL );
	/*! \brief Parse naked regular expression pattern and prepare for matches retrieval.
	 *
	 * pattern - regular expression pattern to match against.
	 */
	int parse_re( HString const& pattern );
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
	HMatchIterator find( char const* const string_ ) const;
	HMatchIterator find( HString const& string_ ) const;
	HMatchIterator end( void ) const;
	/*! \brief Tell if given string matches pattern.
	 *
	 * \param string_ - string to check against the pattern.
	 * \return True iff given string matches this pattern.
	 */
	bool matches( HString const& string_ ) const;
private:
	char const* error_message( int = 0 ) const;
	void error_clear( void ) const;
	char const* matches( char const* const,
			int long* const = NULL /* match length */ ) const;
	bool set_switch( char const, pluggable_flags_t* );
	void save_state( void*, pluggable_flags_t* );
	void restore_state( void*, pluggable_flags_t* );
};

/*! \brief Instance of single match for given pattern.
 */
class HPattern::HMatch {
	int long _size;
	char const* _start;
public:
	int long size() const;
	char const* raw() const;
private:
	HMatch( char const*, int long );
	friend class HPattern::HMatchIterator;
};

/*! \brief Pattern matches iterator.
 */
class HPattern::HMatchIterator : public iterator_interface<HMatch, iterator_category::forward> {
	HPattern const* _owner;
	HPattern::HMatch _match;
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
	HMatchIterator( HPattern const*, char const*, int long );
	friend class HPattern;
};

typedef HExceptionT<HPattern> HPatternException;

}

}

#endif /* #ifndef YAAL_HCORE_HPATTERN_HXX_INCLUDED */

