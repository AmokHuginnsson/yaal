/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_SOURCE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SOURCE_HXX_INCLUDED 1

#include "hcore/hmap.hxx"
#include "hcore/hstreaminterface.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSource final {
public:
	typedef HSource this_type;
	typedef yaal::hcore::HMap<int, int> skips_t;
	typedef yaal::hcore::HMap<int, int> line_numbers_t;
	typedef yaal::hcore::HLookupMap<int, yaal::hcore::HString> comments_t;
private:
	yaal::hcore::HString _name;
	yaal::hcore::HString _orig;
	yaal::hcore::HString _preprocessed;
	skips_t _skips;
	line_numbers_t _lineNumbers;
	comments_t _comments;
	int _skippedLines;
public:
	HSource( void );
	void load( yaal::hcore::HStreamInterface&, yaal::hcore::HString const&, int );
	void reset( void );
	void preprocess( void );
	char const* error_message( int ) const;
	int real_position( int ) const;
	HHuginn::HCoordinate get_coordinate( int ) const;
	yaal::hcore::HString get_snippet( int, int ) const;
	yaal::hcore::HString const& get_comment( int ) const;
	yaal::hcore::HString const& name( void ) const;
	yaal::hcore::HString::const_iterator begin( void ) const;
	yaal::hcore::HString::const_iterator end( void ) const;
	void dump_preprocessed( yaal::hcore::HStreamInterface& ) const;
private:
	int line_number( int ) const;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SOURCE_HXX_INCLUDED */

