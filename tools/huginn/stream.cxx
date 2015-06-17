/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  stream.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "hcore/hstreaminterface.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HStream : public HHuginn::HObject {
	HStreamInterface::ptr_t _stream;
	HChunk _buffer;
	static HHuginn::HClass _streamClass_;
public:
	HStream( HStreamInterface::ptr_t stream_ )
		: HObject( &_streamClass_ )
		, _stream( stream_ )
		, _buffer() {
		return;
	}
	static HHuginn::value_t read( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Stream.read", values_, 1, 1, position_ );
		verify_arg_type( "Stream.read", values_, 0, HHuginn::TYPE::INTEGER, true, position_ );
		int long size( static_cast<int long>( static_cast<HHuginn::HInteger const*>( values_[0].raw() )->value() ) );
		HStream* s( static_cast<HStream*>( object_ ) );
		return ( make_pointer<HHuginn::HString>( s->read_impl( size ) ) );
		M_EPILOG
	}
	static HHuginn::value_t write( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Stream.write", values_, 1, 1, position_ );
		verify_arg_type( "Stream.write", values_, 0, HHuginn::TYPE::STRING, true, position_ );
		HString const& val( static_cast<HHuginn::HString const*>( values_[0].raw() )->value() );
		HStream* s( static_cast<HStream*>( object_ ) );
		s->write_impl( val );
		return ( _none_ );
		M_EPILOG
	}
private:
	HString read_impl( int long size_ ) {
		M_PROLOG
		_buffer.realloc( size_ );
		int long nRead( _stream->read( _buffer.raw(), size_ ) );
		HString s( _buffer.get<char>(), nRead );
		return ( s );
		M_EPILOG
	}
	void write_impl( HString const& val_ ) {
		M_PROLOG
		_stream->write( val_.raw(), val_.get_length() );
		M_EPILOG
	}
};

HHuginn::HClass HStream::_streamClass_(
	nullptr,
	HHuginn::HType::register_type( "Stream", nullptr ),
	nullptr,
	/* methods */ {
		"read",
		"read"
	}, {
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HStream::read, _1, _2, _3, _4 ) ),
		make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HStream::write, _1, _2, _3, _4 ) )
	}
);

}

}

}

