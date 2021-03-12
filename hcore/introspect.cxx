/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	return frames;
}

void dump_call_stack( HStreamInterface& stream, int no ) {
	execution_info::strings_ptr_t frames = execution_info::get_call_stack( no );
	stream << "Obtained " << frames->get_size() << " stack frames." << endl;
	yaal::copy( frames->begin(), frames->end(), stream_iterator( stream, hcore::endl ) );
}

}

}

