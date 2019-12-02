/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
	code_point_t _escape;
	mutable HString _buffer;
public:
	class HIterator;
	typedef HIterator iterator;
	typedef HIterator const_iterator;
public:
	HTokenizer( HString const&, HString const&, behavior_t const& = DEFAULT, code_point_t = code_point_t( 0 ) );
	HTokenizer( HString const&, behavior_t const& = DEFAULT, code_point_t = code_point_t( 0 ) );
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
	HString::size_type _start;
	HString::size_type _end;
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
	void find_end( void );
	HIterator( HTokenizer const*, int long );
	friend class HTokenizer;
};

typedef HExceptionT<HTokenizer> HTokenizerException;

}

}

#endif /* #ifndef YAAL_HCORE_HTOKENIZER_HXX_INCLUDED */

