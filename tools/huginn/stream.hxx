/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  stream.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HStream : public HHuginn::HIterable {
	yaal::hcore::HStreamInterface::ptr_t _stream;
	yaal::hcore::HChunk _buffer;
	yaal::hcore::HString _lineBuffer;
public:
	HStream( HHuginn::HClass*, yaal::hcore::HStreamInterface::ptr_t );
	static HHuginn::value_t read( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const&, int );
	static HHuginn::value_t read_line( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const&, int );
	static HHuginn::value_t write( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const&, int );
	yaal::hcore::HString const& read_line_impl( void );
	bool is_valid( void ) const;
	static HHuginn::class_t get_class( HHuginn* );
private:
	yaal::hcore::HString read_impl( int long );
	void write_impl( yaal::hcore::HString const& );
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED */

