/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "stream.hxx"
#include "runtime.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace {
void raise( hcore::HException const& exception_, huginn::HThread* thread_, int position_ ) {
	M_PROLOG
	HRuntime& r( thread_->runtime() );
	HHuginn::HClass const* c( r.get_class( r.identifier_id( "OperatingSystemException" ) ).raw() );
	if ( ! c ) {
		c = r.get_class( r.identifier_id( "FileSystemException" ) ).raw();
	}
	if ( ! c ) {
		c = r.object_factory()->exception_class();
	}
	thread_->raise( c, exception_.what(), position_ );
	return;
	M_EPILOG
}
}

class HStreamIterator : public HIteratorInterface {
	HStream* _stream;
	HString _lineCache;
	HObjectFactory* _objectFactory;
public:
	HStreamIterator( HStream* stream_ )
		: _stream( stream_ )
		, _lineCache( _stream->read_line_impl() )
		, _objectFactory( _stream->HValue::get_class()->runtime()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( ! _lineCache.is_empty() ? _objectFactory->create_string( _lineCache ) : _objectFactory->runtime().none_value() );
	}
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _stream->is_valid() );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		try {
			_lineCache = _stream->read_line_impl();
		} catch ( HException const& e ) {
			raise( e, thread_, position_ );
		}
	}
private:
	HStreamIterator( HStreamIterator const& ) = delete;
	HStreamIterator& operator = ( HStreamIterator const& ) = delete;
};

HStream::HStream( HHuginn::HClass const* class_, HStreamInterface::ptr_t stream_ )
	: HIterable( class_ )
	, _stream( stream_ )
	, _buffer()
	, _lineBuffer()
	, _converter() {
	return;
}

HHuginn::value_t HStream::read( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Stream.read", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	int size( static_cast<int>( get_integer( values_[0] ) ) );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( s->read_impl( size ) ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_line( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Stream.read_line", values_, 0, 0, thread_, position_ );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	HString const& line( s->read_line_impl() );
	return ( ! line.is_empty() ? thread_->object_factory().create_string( line ) : thread_->runtime().none_value() );
	M_EPILOG
}

HHuginn::value_t HStream::write( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Stream.write", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	HString const& val( get_string( values_[0] ) );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	s->write_impl( val );
	return ( thread_->runtime().none_value() );
	M_EPILOG
}

HString HStream::read_impl( int long size_ ) {
	M_PROLOG
	_buffer.realloc( size_ );
	int long nRead( _stream->read( _buffer.raw(), size_ ) );
	HString s( _buffer.get<char>(), nRead );
	return ( s );
	M_EPILOG
}

void HStream::write_impl( HString const& val_ ) {
	M_PROLOG
	_converter = val_;
	_stream->write( _converter.c_str(), _converter.byte_count() );
	M_EPILOG
}

yaal::hcore::HString const& HStream::read_line_impl( void ) {
	M_PROLOG
	_stream->read_until( _lineBuffer, HStreamInterface::eols, false );
	return ( _lineBuffer );
	M_EPILOG
}

bool HStream::is_valid( void ) const {
	M_PROLOG
	return ( _stream->is_valid() );
	M_EPILOG
}

HHuginn::HIterable::HIterator HStream::do_iterator( HThread* thread_, int position_ ) {
	try {
		HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStreamIterator( this ) );
		return ( HIterator( yaal::move( impl ) ) );
	} catch ( HException const& e ) {
		raise( e, thread_, position_ );
	}
	return ( HIterator::iterator_implementation_t( nullptr ) );
}

int long HStream::do_size( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Getting size of `Stream' is an invalid operation.", thread_->current_frame()->file_id(), position_ );
}

HHuginn::class_t HStream::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	char const name[] = "Stream";
	HHuginn::class_t c( runtime_->get_class( runtime_->identifier_id( name ) ) );
	if ( ! c ) {
		c = runtime_->create_class(
			name,
			nullptr,
			"The `Stream` class gives an interface for stream based I/O operations."
		);
		runtime_->huginn()->register_class( c );
	}
	HHuginn::field_definitions_t fd{
		{ "read",      runtime_->create_method( c.raw(), &HStream::read ),      "read all data from given stream" },
		{ "read_line", runtime_->create_method( c.raw(), &HStream::read_line ), "read single line of text from given stream" },
		{ "write",     runtime_->create_method( c.raw(), &HStream::write ),     "( *value* ) - write given value info this stream" }
	};
	c->redefine( nullptr, fd );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HStream::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on Stream.", thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

}

}

}

