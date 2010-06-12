/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hpattern.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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
#define YAAL_HCORE_HPATTERN_HXX_INCLUDED

#include "hcore/hchunk.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hpair.hxx"
#include "hcore/harray.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Regular expression string searcher.
 */
class HPattern
	{
public:
	class HMatch;
	class HMatchIterator;
	typedef HMatchIterator iterator;
	typedef HPair<char, bool*> flag_desc_t;
	typedef HArray<flag_desc_t> pluggable_flags_t;
private:
	typedef HPattern this_type;
	bool		_initialized;				/*!< is pattern initialized */
	bool		_ignoreCaseDefault;	/*!< default value for ignore case switch */
	bool		_ignoreCase;				/*!< self explanary */
	bool		_extended;					/*!< simple strstr or extended RE */
	int			_simpleMatchLength;	/*!< length of simple strstr pattern */
	HChunk	_compiled;					/*!< compiled regular expression for
																	 search patterns */
	HString	_patternInput;			/*!< current search pattern */
	HString	_patternReal;				/*!< pattern with stripped switches */
	mutable int _lastError;
	mutable HString _varTmpBuffer;	/*!< error message of last operation */
public:
	/*! \brief Construct pattern.
	 *
	 * caseSentitive - shall defualt match policy be: case matters, default ignore case state
	 */
	HPattern( bool caseSentitive = false );
	virtual ~HPattern( void );
	/*! \brief Parse regular expression patten and prepare for matches retrieval.
	 *
	 * pattern - regular expression pattern to match against.
	 * externalFlags - set of external flags that may by set by pattern extra arguments.
	 */
	int parse( HString const& pattern, pluggable_flags_t* externalFlags = NULL );
	int parse_re( char const* const );
	HString const& error( void ) const;
	int error_code( void ) const;
	HMatchIterator find( char const* const ) const;
	HMatchIterator end( void ) const;
private:
	char const* matches( char const* const,
			int long* const = NULL /* match length */ ) const;
	void prepare_error_message( HString const& ) const;
	bool set_switch( char const, pluggable_flags_t* );
	void save_state( void*, pluggable_flags_t* );
	void restore_state( void*, pluggable_flags_t* );
	};

/*! \brief Instance of single match for given pattern.
 */
class HPattern::HMatch
	{
	int long _size;
	char const* _start;
public:
	int long size() const;
	char const* raw() const;
private:
	HMatch( char const* const&, int long const& );
	friend class HPattern::HMatchIterator;
	};

/*! \brief Pattern matches iterator.
 */
class HPattern::HMatchIterator
	{
	HPattern const* _owner;
	HPattern::HMatch _match;
public:
	HMatch const* operator->( void ) const;
	HMatch const& operator*( void ) const;
	bool operator != ( HMatchIterator const& ) const;
	bool operator == ( HMatchIterator const& ) const;
	HMatchIterator& operator ++ ( void );
private:
	HMatchIterator( HPattern const*, char const*, int long const& );
	friend class HPattern;
	};

typedef HExceptionT<HPattern> HPatternException;

}

}

#endif /* not YAAL_HCORE_HPATTERN_HXX_INCLUDED */

