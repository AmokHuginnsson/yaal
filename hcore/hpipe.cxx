/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <unistd.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hpipe.hxx"
#include "hrawfile.hxx"
#include "system.hxx"

namespace yaal {

namespace hcore {

HPipe::HPipe( void )
	: _in()
	, _out() {
	M_PROLOG
	int fds[2] = { -1, -1 };
	M_ENSURE( ::pipe( fds ) == 0 );
	_in = make_pointer<HRawFile>( fds[1], HRawFile::OWNERSHIP::ACQUIRED );
	_out = make_pointer<HRawFile>( fds[0], HRawFile::OWNERSHIP::ACQUIRED );
	return;
	M_EPILOG
}

HPipe::~HPipe( void ) {
	M_PROLOG
	HRawFile* inStream( static_cast<HRawFile*>( _in.raw() ) );
	HRawFile* outStream( static_cast<HRawFile*>( _out.raw() ) );
	M_ENSURE( ! inStream->is_valid() || ( inStream->close() == 0 ) );
	M_ENSURE( ! outStream->is_valid() || ( outStream->close() == 0 ) );
	return;
	M_DESTRUCTOR_EPILOG
}

int long HPipe::read( void* buffer_, int long size_ ) {
	M_PROLOG
	return ( _out->read( buffer_, size_ ) );
	M_EPILOG
}

int long HPipe::write( void const* buffer_, int long size_ ) {
	M_PROLOG
	return ( _in->write( buffer_, size_ ) );
	M_EPILOG
}

yaal::hcore::HStreamInterface::ptr_t const& HPipe::in( void ) const {
	return ( _in );
}

yaal::hcore::HStreamInterface::ptr_t const& HPipe::out( void ) const {
	return ( _out );
}

}

}

