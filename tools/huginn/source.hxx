/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  source.hxx - this file is integral part of `yaal' project.

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
	typedef yaal::hcore::HLookupMap<int, yaal::hcore::HString> comments_t;
private:
	yaal::hcore::HString _name;
	yaal::hcore::HChunk _orig;
	int _origSize;
	yaal::hcore::HChunk _preprocessed;
	int _preprocessedSize;
	skips_t _skips;
	comments_t _comments;
	int _skippedLines;
public:
	HSource( void );
	void load( yaal::hcore::HStreamInterface&, yaal::hcore::HString const&, int );
	void preprocess( void );
	char const* error_message( int ) const;
	int error_position( int ) const;
	HHuginn::HErrorCoordinate error_coordinate( int ) const;
	yaal::hcore::HString get_snippet( int, int ) const;
	yaal::hcore::HString const& get_comment( int ) const;
	yaal::hcore::HString const& name( void ) const;
	yaal::hcore::HString::const_iterator begin( void ) const;
	yaal::hcore::HString::const_iterator end( void ) const;
	void dump_preprocessed( yaal::hcore::HStreamInterface& ) const;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SOURCE_HXX_INCLUDED */

