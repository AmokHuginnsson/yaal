/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  introspect.cxx - this file is integral part of `yaal' project.

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

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "introspect.hxx"
#include "memory.hxx"

#ifdef HAVE_EXECINFO_H
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

