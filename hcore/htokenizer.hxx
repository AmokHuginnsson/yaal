/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/htokenizer.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/htokenizer.hxx
 * \brief HTokenizer class related declarations.
 *
 * HTokenizer::HIterator class is included here.
 */

#ifndef YAAL_HCORE_HTOKENIZER_HXX_INCLUDED
#define YAAL_HCORE_HTOKENIZER_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/hstring.hxx"
#include "hcore/hbitflag.hxx"

namespace yaal {

namespace hcore {

/*! \brief Convenient interface for splitting strings into tokens.
 */
class HTokenizer final {
	typedef HTokenizer this_type;
public:
	typedef HBitFlag<HTokenizer> behavior_t;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const DEFAULT;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const SKIP_EMPTY;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const INCLUDE_EMPTY;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const HANDLE_EMPTY_MASK;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const DELIMITED_BY_ANY_OF;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const DELIMITED_BY_WHOLE_STRING;
	static M_YAAL_HCORE_PUBLIC_API behavior_t const DELIMITED_BY_MASK;
private:
	behavior_t _behavior;
	HString _string;
	HString _delimiter;
	mutable HString _buffer;
public:
	class HIterator;
	typedef HIterator iterator;
	typedef HIterator const_iterator;
public:
	HTokenizer( HString const&, HString const&, behavior_t const& = DEFAULT );
	HTokenizer( HString const&, behavior_t const& = DEFAULT );
	void assign( HString const& );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HString const& operator[] ( int long ) const;
	HString tail( HIterator ) const;
private:
	int long skip_empty( int long ) const;
	friend class HIterator;
};

/*! \brief Interface for iteration through tokens from given string.
 */
class HTokenizer::HIterator {
	typedef HTokenizer::HIterator this_type;
	HTokenizer const* _owner;
	int long _start;
	mutable HString _buffer;
public:
	HIterator( HIterator const& );
	HIterator& operator = ( HIterator const& );
	HString const& operator*( void ) const;
	HString const* operator->( void ) const;
	bool operator != ( HIterator const& ) const;
	bool operator == ( HIterator const& ) const;
	HIterator& operator ++ ( void );
private:
	HIterator( HTokenizer const*, int long );
	friend class HTokenizer;
};

typedef HExceptionT<HTokenizer> HTokenizerException;

}

}

#endif /* #ifndef YAAL_HCORE_HTOKENIZER_HXX_INCLUDED */

