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
#include "hcore/safe_int.hxx"

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

class HStreamClass : public HHuginn::HClass {
public:
	typedef HStreamClass this_type;
	typedef HHuginn::HClass base_type;
private:
	enumeration::HEnumerationClass::ptr_t _seekEnumerationClass;
public:
	HStreamClass( HRuntime* runtime_, HHuginn::type_id_t typeId_, HHuginn::identifier_id_t identifierId_ )
		: HClass(
			runtime_,
			typeId_,
			identifierId_,
			nullptr,
			"The `Stream` class gives an interface for stream based I/O operations."
		)
		, _seekEnumerationClass() {
		HHuginn::field_definitions_t fd{
			{ "read",      runtime_->create_method( &HStream::read ),      "read all data from given stream" },
			{ "read_line", runtime_->create_method( &HStream::read_line ), "read single line of text from given stream" },
			{ "write",     runtime_->create_method( &HStream::write ),     "( *value* ) - write given value info this stream" },
			{ "seek",      runtime_->create_method( &HStream::seek ),     "( *offset*, *anchor* ) - move reading/writing position to the *offset* counted from an *anchor*" }
		};
		redefine( nullptr, fd );
		_seekEnumerationClass = add_enumeration_as_member(
			this,
			enumeration::create_class(
				runtime_,
				"SEEK",
				enumeration::descriptions_t{
					{ "BEGIN", "Count offset position form the beginning of the stream.", static_cast<int>( HStreamInterface::SEEK::BEGIN ) },
					{ "CURRENT", "Count offset position form the current position in the stream.", static_cast<int>( HStreamInterface::SEEK::CURRENT ) },
					{ "END", "Count offset position form the end of the stream.", static_cast<int>( HStreamInterface::SEEK::END ) }
				},
				"The `SEEK` is set of possible anchors used for seeking in seekable streams.",
				HHuginn::VISIBILITY::PACKAGE
			),
			"set of possible modes used for seeking in streams.",
			HHuginn::HClass::MEMBER_TYPE::STATIC
		);
	}
	HHuginn::HClass const* seek_class( void ) const {
		return ( _seekEnumerationClass->enumeral_class() );
	}
};

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

HHuginn::value_t HStream::seek( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	HStreamClass const* sc( static_cast<HStreamClass const*>( s->HValue::get_class() ) );
	verify_signature_by_class( "Stream.seek", values_, { thread_->object_factory().integer_class(), sc->seek_class() }, thread_, position_ );
	try {
		s->seek_impl( safe_int::cast<int long>( get_integer( values_[0] ) ), static_cast<HStreamInterface::SEEK>( get_enumeral( values_[1] ) ) );
	} catch ( HException const& e ) {
		raise( e, thread_, position_ );
	}
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

void HStream::seek_impl( int long offset_, yaal::hcore::HStreamInterface::SEEK anchor_ ) {
	M_PROLOG
	_stream->seek( offset_, anchor_ );
	return;
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
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( "Stream" ) );
	HHuginn::class_t c( runtime_->get_class( classIdentifier ) );
	if ( ! c ) {
		c =	runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &classIdentifier] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HStreamClass>(
							runtime_,
							typeId_,
							classIdentifier
						)
					);
				}
			)
		);
		runtime_->huginn()->register_class( c, HHuginn::ACCESS::PRIVATE, HHuginn::VISIBILITY::GLOBAL );
	}
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

