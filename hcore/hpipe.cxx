/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipe.cxx - this file is integral part of `yaal' project.

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

#include <unistd.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hpipe.hxx"
#include "hrawfile.hxx"
#include "system.hxx"

namespace yaal {

namespace hcore {

HPipe::HPipe( void )
	: _pipe() {
	M_PROLOG
	M_ENSURE( ::pipe( _pipe ) == 0 );
	return;
	M_EPILOG
}

HPipe::~HPipe( void ) {
	M_PROLOG
	M_TEMP_FAILURE_RETRY( system::close( _pipe[ 1 ] ) );
	M_TEMP_FAILURE_RETRY( system::close( _pipe[ 0 ] ) );
	return;
	M_DESTRUCTOR_EPILOG
}

int HPipe::get_reader_fd( void ) const {
	return ( _pipe[ 0 ] );
}

int long HPipe::do_read( void* const buffer_, int long size_ ) {
	M_PROLOG
	return ( ::read( _pipe[ 0 ], buffer_, size_ ) );
	M_EPILOG
}

int long HPipe::do_write( void const* const buffer_, int long size_ ) {
	M_PROLOG
	M_ASSERT( _pipe[1] >= 0 );
	int long nWritten( 0 );
	int long nWriteChunk( 0 );
	do {
		nWriteChunk = M_TEMP_FAILURE_RETRY( ::write( _pipe[ 1 ],
					static_cast<char const* const>( buffer_ ) + nWritten,
					size_ - nWritten ) );
		nWritten += nWriteChunk;
	} while ( ( nWriteChunk > 0 ) && ( nWritten < size_ ) );
	return ( nWritten );
	M_EPILOG
}

void HPipe::do_flush( void ) const {
}

bool HPipe::do_is_valid( void ) const {
	M_PROLOG
	return ( ( _pipe[ 0 ] >= 0 ) && ( _pipe[ 1 ] >= 0 ) );
	M_EPILOG
}

}

}

