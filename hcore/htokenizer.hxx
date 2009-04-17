/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/htokenizer.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/htokenizer.hxx
 * \brief HTokenizer class related declarations.
 *
 * HTokenizer::HIterator class is included here.
 */

#ifndef YAAL_HCORE_HTOKENIZER_HXX_INCLUDED
#define YAAL_HCORE_HTOKENIZER_HXX_INCLUDED

#include "hcore/hstring.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Convenient interface for splitting strings into tokens.
 */
class HTokenizer
	{
	typedef HTokenizer self_t;
public:
	typedef enum
		{
		D_SKIP_EMPTY,
		D_INCLUDE_EMPTY
		} behavoir_t;
private:
	behavoir_t _behavior;
	HString _string;
	HString _delimiter;
	mutable HString _buffer;
public:
	class HIterator;
public:
	HTokenizer( HString const&, HString const&, behavoir_t const& = D_INCLUDE_EMPTY );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HString const& operator[] ( int long const& ) const;
private:
	friend class HIterator;
	};

/*! \brief Interface for itertion thru tokens from given string.
 */
class HTokenizer::HIterator
	{
	typedef HTokenizer::HIterator self_t;
	HTokenizer const* _owner;
	int long _start;
	mutable HString _buffer;
public:
	HIterator( HIterator const& );
	HIterator& operator = ( HIterator const& );
	HString const& operator*( void ) const;
	bool operator != ( HIterator const& ) const;
	HIterator& operator ++ ( void );
private:
	HIterator( HTokenizer const*, int long const& );
	friend class HTokenizer;
	};

typedef HExceptionT<HTokenizer> HTokenizerException;

}

}

#endif /* not YAAL_HCORE_HTOKENIZER_HXX_INCLUDED */

