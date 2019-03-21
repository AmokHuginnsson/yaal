/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_SEMANTICBUFFER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SEMANTICBUFFER_HXX_INCLUDED 1

#include <cstring>

#include "tools/hhuginn.hxx"
#include "tools/huginn/helper.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSemanticBuffer {
private:
	yaal::hcore::HChunk& _buffer;
	yaal::hcore::HUTF8String& _converter;
	int long _size;
	HCycleTracker _cycleTracker;
	struct OContext {
		huginn::HThread* _thread;
		int _position;
	} _context;
public:
	HSemanticBuffer( yaal::hcore::HChunk&, yaal::hcore::HUTF8String&, huginn::HThread*, int );
	void* data( void ) const {
		return ( _buffer.raw() );
	}
	int long size( void ) const {
		return ( _size );
	}
	void serialize( HHuginn::value_t const& );
	template <typename T>
	void write( T val_ ) {
		M_PROLOG
		int long s( static_cast<int>( sizeof ( val_ ) ) );
		_buffer.realloc( _size + s );
		memcpy( _buffer.get<char>() + _size, &val_, static_cast<size_t>( s ) );
		_size += s;
		return;
		M_EPILOG
	}
	void write( yaal::hcore::HString const& );
	void write( void const*, int long );
private:
	HSemanticBuffer( HSemanticBuffer&& ) = delete;
	HSemanticBuffer& operator = ( HSemanticBuffer&& ) = delete;
	HSemanticBuffer( HSemanticBuffer const& ) = delete;
	HSemanticBuffer& operator = ( HSemanticBuffer const& ) = delete;
};

}

}

}

#endif /* #ifndef SEMANTICBUFFER_HXX_INCLUDED */

