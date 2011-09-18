/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	introspect.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "introspect.hxx"
#include "memory.hxx"

#if defined( HAVE_EXECINFO_H )
#include <execinfo.h>
#endif /* HAVE_EXECINFO_H */
#include <cxxabi.h>

#include "hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hcore {

#ifdef _EXECINFO_H
execution_info::strings_ptr_t execution_info::get_call_stack( int level_ )
#else /* _EXECINFO_H */
execution_info::strings_ptr_t execution_info::get_call_stack( int )
#endif /* not _EXECINFO_H */
	{
	strings_ptr_t frames( make_pointer<strings_t>() );
#ifdef _EXECINFO_H
	
	void** pointer = memory::calloc<void*>( level_ + 1 );
	int size( backtrace( pointer, level_ ) );
	char** strings = backtrace_symbols( pointer, size );

	if ( level_ < size )
		size = level_;
	for ( int ctr( 0 ); ctr < size; ++ ctr ) {
		char* ptr( strchr( strings[ ctr ], '(' ) );
		if ( ptr ) {
			char* end( strchr( ptr, '+' ) );
			if ( end )
				(*end) = 0;
			HString symbol( demangle( ptr + 1 ) );
			if ( ! symbol.is_empty() )
				frames->push_back( symbol );
			else if ( strings[ ctr ] )
				frames->push_back( strings[ ctr ] );
		}
	}

	memory::free( strings );
	memory::free( pointer );
#endif /* _EXECINFO_H */
	return ( frames );
}

void dump_call_stack( HStreamInterface& stream, int no ) {
	execution_info::strings_ptr_t frames = execution_info::get_call_stack( no );
	stream << "Obtained " << frames->get_size() << " stack frames." << endl;
	yaal::copy( frames->begin(), frames->end(), stream_iterator( stream, hcore::endl ) );
}

}

}

