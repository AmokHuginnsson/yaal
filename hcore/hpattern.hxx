/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.hxx - this file is integral part of `yaal' project.

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
	typedef HPattern self_t;
	bool		f_bInitialized;				/*!< is pattern initialized */
	bool		f_bIgnoreCaseDefault;	/*!< default value for ignore case switch */
	bool		f_bIgnoreCase;				/*!< self explanary */
	bool		f_bExtended;					/*!< simple strstr or extended RE */
	int			f_iSimpleMatchLength;	/*!< length of simple strstr pattern */
	HChunk	f_oCompiled;					/*!< compiled regular expression for
																	 search patterns */
	HString	f_oPatternInput;			/*!< current search pattern */
	HString	f_oPatternReal;				/*!< pattern with stripped switches */
	HString f_oError;							/*!< error message of last operation */
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
	char const* matches( HString const&,
			int* const = NULL /* match length */,
			int* const = NULL /* error code */ );
	int count( char const* const );
private:
	void prepare_error_message( int const, HString const& );
	bool set_switch( char const, pluggable_flags_t* );
	void save_state( void*, pluggable_flags_t* );
	void restore_state( void*, pluggable_flags_t* );
	};

class HPattern::HMatchIterator
	{
	};

class HPattern::HMatch
	{
	};

typedef HExceptionT<HPattern> HPatternException;

}

}

#endif /* not YAAL_HCORE_HPATTERN_HXX_INCLUDED */

