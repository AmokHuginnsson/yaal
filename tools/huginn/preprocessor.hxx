/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_PREPROCESSOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_PREPROCESSOR_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/iterator.hxx"
#include "source.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace {
static code_point_t const NEWLINE = '\n'_ycp;
}

class HPrepocessor final {
public:
	typedef HPrepocessor this_type;
	class HIterator;
private:
	yaal::hcore::HString::const_iterator _beg;
	yaal::hcore::HString::const_iterator _end;
	yaal::hcore::HString _comment;
public:
	HPrepocessor( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ )
		: _beg( first_ )
		, _end( last_ )
		, _comment() {
		return;
	}
	HIterator begin( void );
	HIterator end( void );
	yaal::hcore::HString const& comment( void ) const {
		return ( _comment );
	}
private:
	HPrepocessor( HPrepocessor const& ) = delete;
	HPrepocessor& operator = ( HPrepocessor const& ) = delete;
};

class HPrepocessor::HIterator : public yaal::hcore::iterator_interface<code_point_t const, yaal::hcore::iterator_category::forward> {
public:
	typedef HIterator this_type;
	typedef yaal::hcore::iterator_interface<code_point_t const, yaal::hcore::iterator_category::forward> base_type;
private:
	static char const COMMENT_START_CHAR1 = '/';
	static char const COMMENT_START_CHAR2 = '*';
	static char const COMMENT_START_CHAR2ALT = '/';
	static char const COMMENT_STOP_CHAR1 = '*';
	static char const COMMENT_STOP_CHAR2 = '/';
	static char const ESCAPE = '\\';
	static char const DOUBLE_QUOTE = '"';
	static char const SINGLE_QUOTE = '\'';
	enum class STATE {
		NORMAL,
		IN_COMMENT,
		IN_SINGLE_QUOTE,
		IN_DOUBLE_QUOTE
	};
	HPrepocessor* _owner;
	yaal::hcore::HString::const_iterator _cur;
	STATE _state;
	bool _escape; /*!< Is a QUOTE substate. */
public:
	HIterator( void )
		: _owner( nullptr )
		, _cur()
		, _state( STATE::NORMAL )
		, _escape( false ) {
	}
	HIterator( HIterator const& it_ )
		: _owner( it_._owner )
		, _cur( it_._cur )
		, _state( it_._state )
		, _escape( it_._escape ) {
	}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			HIterator tmp( it_ );
			swap( tmp );
		}
		return ( *this );
	}
	void swap( HIterator& it_ ) {
		if ( &it_ != this ) {
			using yaal::swap;
			swap( _owner, it_._owner );
			swap( _cur, it_._cur );
			swap( _state, it_._state );
			swap( _escape, it_._escape );
		}
		return;
	}
	HIterator& operator ++ ( void ) {
		++ _cur;
		if ( _cur != _owner->_end ) {
			make_readable();
		}
		return ( *this );
	}
	HIterator operator ++ ( int ) const {
		HIterator it( *this );
		++ it;
		return it;
	}
	bool operator == ( HIterator const& it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( it_._cur == _cur );
	}
	bool operator != ( HIterator const& it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( it_._cur != _cur );
	}
	trait::strip_const<value_type>::type operator * ( void ) const {
		M_ASSERT( _owner );
		return ( *_cur );
	}
	yaal::hcore::HString::const_iterator raw( void ) const {
		return ( _cur );
	}
private:
	HIterator( HPrepocessor* owner_, yaal::hcore::HString::const_iterator pos_ )
		: _owner( owner_ )
		, _cur( pos_ )
		, _state( STATE::NORMAL )
		, _escape( false ) {
		if ( _cur != _owner->_end ) {
			make_readable();
		}
		return;
	}
	friend class HPrepocessor;
	void make_readable( void );
	yaal::hcore::HString::const_iterator try_skip_whitespace( yaal::hcore::HString::const_iterator, code_point_t );
	yaal::hcore::HString::const_iterator skip_whitespace( yaal::hcore::HString::const_iterator );
	yaal::hcore::HString::const_iterator skip_comment( yaal::hcore::HString::const_iterator );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_PREPROCESSOR_HXX_INCLUDED */

