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
#include "semanticbuffer.hxx"
#include "packagefactory.hxx"
#include "hcore/safe_int.hxx"
#include "hcore/number.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hsocket.hxx"
#include "hcore/unicode.hxx"
#include "tools/hstringstream.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace hcore {
struct HNumber::ElementaryFunctions {
	static HHuginn::value_t deserialize( huginn::HThread* thread_, HStreamInterface::ptr_t& stream_, HStream const* context_, int position_ ) {
		M_PROLOG
		HHuginn::value_t v( thread_->object_factory().create_number( HNumber() ) );
		HNumber& n( static_cast<HHuginn::HNumber*>( v.raw() )->value() );
		int long toRead( 0 );
		int long nRead( 0 );
		do {
			toRead = static_cast<int>( sizeof ( n._precision ) );
			nRead = stream_->read( &n._precision, toRead );
			if ( toRead != nRead ) {
				break;
			}

			toRead = static_cast<int>( sizeof ( n._leafCount ) );
			nRead = stream_->read( &n._leafCount, toRead );
			if ( toRead != nRead ) {
				break;
			}

			toRead = static_cast<int>( sizeof ( n._integralPartSize ) );
			nRead = stream_->read( &n._integralPartSize, toRead );
			if ( toRead != nRead ) {
				break;
			}

			i8_t i8( 0 );
			toRead = static_cast<int>( sizeof ( i8 ) );
			nRead = stream_->read( &i8, toRead );
			if ( toRead != nRead ) {
				break;
			}
			n._negative = i8 != 0;

			if (
				( ( i8 != 0 ) && ( i8 != 1 ) )
				|| ( n._precision < number::HARDCODED_MINIMUM_PRECISION )
				|| ( n._leafCount < 0 )
				|| ( n._integralPartSize < 0 )
				|| ( n._integralPartSize > n._leafCount )
			) {
				context_->raise( thread_, "Malformed Huginn data stream.", position_, context_->exception_class() );
				break;
			}
			if ( n._leafCount > 0 ) {
				n._canonical.realloc( chunk_size<HNumber::integer_t>( n._leafCount ) );
				toRead = n._leafCount * static_cast<int>( sizeof ( HNumber::integer_t ) );
				nRead = stream_->read( n._canonical.raw(), toRead );
			}
		} while ( false );
		context_->post_io( thread_, toRead, nRead, HStream::IO::READ, position_ );
		return ( v );
		M_EPILOG
	}
};
}

namespace tools {

namespace huginn {

class HStreamClass : public HHuginn::HClass {
public:
	typedef HStreamClass this_type;
	typedef HHuginn::HClass base_type;
private:
	enumeration::HEnumerationClass::ptr_t _seekEnumerationClass;
	HHuginn::class_t _exceptionClass;
public:
	HStreamClass( HRuntime* runtime_, HHuginn::type_id_t typeId_, HHuginn::identifier_id_t identifierId_ )
		: HClass(
			runtime_,
			typeId_,
			identifierId_,
			"The `Stream` class gives an interface for stream based I/O operations.",
			HHuginn::ACCESS::PRIVATE
		)
		, _seekEnumerationClass()
		, _exceptionClass() {
		HHuginn::field_definitions_t fd{
			{ "read_blob",       runtime_->create_method( &HStream::read_fwd,  "Stream.read_blob",       &HStream::read_blob ),       "( *count* ) read *count* bytes of data from this stream" },
			{ "read_string",     runtime_->create_method( &HStream::read_fwd,  "Stream.read_string",     &HStream::read_string ),     "( *count* ) read *count* bytes of UTF-8 encoded characters as a `string` from this stream" },
			{ "read_integer",    runtime_->create_method( &HStream::read_fwd,  "Stream.read_integer",    &HStream::read_integer ),    "( *count* ) read *count==(1|2|4|8)* bytes of data as an `integer` from this stream" },
			{ "read_integer_unsigned", runtime_->create_method( &HStream::read_fwd,  "Stream.read_integer_unsigned",    &HStream::read_integer_unsigned ), "( *count* ) read *count==(1|2|4|8)* bytes of data as an `integer` from this stream's unsigned raw storage" },
			{ "read_real",       runtime_->create_method( &HStream::read_fwd,  "Stream.read_real",       &HStream::read_real ),
				"( *count* ) read *count==(" M_STRINGIFY( SIZEOF_FLOAT )
#if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG
					"|" M_STRINGIFY( SIZEOF_DOUBLE )
#endif /* #if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG */
					"|" M_STRINGIFY( SIZEOF_DOUBLE_LONG ) ")* bytes of data as a `real` from this stream"
			},
			{ "read_character",  runtime_->create_method( &HStream::read_fwd,  "Stream.read_character",  &HStream::read_character ),  "( *count* ) read *count==(1|2|4)* bytes of data as a `character` from this stream" },
			{ "write_blob",      runtime_->create_method( &HStream::write_fwd, "Stream.write_blob",      HHuginn::TYPE::BLOB,      &HStream::write_blob ),      "( *blobVal*, *count* ) - write *count* number of bytes from given *blobVal* info this stream" },
			{ "write_string",    runtime_->create_method( &HStream::write_fwd, "Stream.write_string",    HHuginn::TYPE::STRING,    &HStream::write_string ),    "( *strVal*, *count* ) - write *count* number of code points from given *strVal* info this stream" },
			{ "write_integer",   runtime_->create_method( &HStream::write_fwd, "Stream.write_integer",   HHuginn::TYPE::INTEGER,   &HStream::write_integer ) ,  "( *intVal*, *count* ) - write *count==(1|2|4|8)* number of bytes from given *intVal* info this stream" },
			{ "write_integer_unsigned",   runtime_->create_method( &HStream::write_fwd, "Stream.write_integer_unsigned",   HHuginn::TYPE::INTEGER,   &HStream::write_integer_unsigned ) ,  "( *intVal*, *count* ) - write *count==(1|2|4|8)* number of bytes from given *intVal* info this stream's unsigned raw storage" },
			{ "write_real",      runtime_->create_method( &HStream::write_fwd, "Stream.write_real",      HHuginn::TYPE::REAL,      &HStream::write_real ),
				"( *realVal*, *count* ) - write *count==(" M_STRINGIFY( SIZEOF_FLOAT )
#if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG
					"|" M_STRINGIFY( SIZEOF_DOUBLE )
#endif /* #if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG */
					"|" M_STRINGIFY( SIZEOF_DOUBLE_LONG ) ")* number of bytes from given *realVal* info this stream" },
			{ "write_character", runtime_->create_method( &HStream::write_fwd, "Stream.write_character", HHuginn::TYPE::CHARACTER, &HStream::write_character ), "( *charVal*, *count* ) - write *count==(1|2|4)* number of bytes from given *charVal* info this stream" },
			{ "seek",            runtime_->create_method( &HStream::seek ),        "( *offset*, *anchor* ) - move reading/writing position to the *offset* counted from an *anchor*" },
			{ "read_line",       runtime_->create_method( &HStream::read_line ),   "read single line of text from this stream" },
			{ "deserialize",     runtime_->create_method( &HStream::deserialize ), "deserialize single Huginn object from this stream" },
			{ "write_line",      runtime_->create_method( &HStream::write_line ),  "( *strVal* ) - write entriety of given *strVal* info this stream" },
			{ "serialize",       runtime_->create_method( &HStream::serialize ),   "( *val* ) - serialize given *val* info this stream" }
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
				HHuginn::VISIBILITY::PACKAGE,
				this
			),
			"set of possible modes used for seeking in streams.",
			HHuginn::HClass::MEMBER_TYPE::STATIC
		);
		_exceptionClass = class_exception( this );
	}
	HHuginn::HClass const* seek_class( void ) const {
		return ( _seekEnumerationClass->enumeral_class() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
};

class HStreamIterator : public HIteratorInterface {
	HStream* _stream;
	HString _lineCache;
	HObjectFactory* _objectFactory;
public:
	HStreamIterator( HThread* thread_, HStream* stream_, int position_ )
		: _stream( stream_ )
		, _lineCache( _stream->read_line_raw( thread_, position_ ) )
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
		_lineCache = _stream->read_line_raw( thread_, position_ );
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

yaal::hcore::HStreamInterface::ptr_t HStream::raw( void ) const {
	return ( _stream );
}

HHuginn::HClass const* HStream::exception_class( void ) const {
	return ( static_cast<HStreamClass const*>( HValue::get_class() )->exception_class() );
}

void HStream::raise( huginn::HThread* thread_, hcore::HString const& message_, int position_, HHuginn::HClass const* excClass_ ) const {
	M_PROLOG
	HRuntime& r( thread_->runtime() );
	char const* packageName( nullptr );
	HStreamInterface const* s( _stream.raw() );
	HHuginn::HClass const* c( excClass_ );
	if ( c ) {
		/* Exception class was explicitly requested. */
	} else if ( dynamic_cast<HFile const*>( s ) ) {
		packageName = "FileSystem";
	} else if ( dynamic_cast<HSocket const*>( s ) || dynamic_cast<HOpenSSL const*>( s ) ) {
		packageName = "Network";
	} else if ( dynamic_cast<HStringStream const*>( s ) ) {
		packageName = "Text";
	} else if ( dynamic_cast<HSynchronizedStream const*>( s ) || dynamic_cast<HRawFile const*>( s ) ) {
		packageName = "OperatingSystem";
	}
	if ( ! c ) {
		HHuginn::value_t package( r.find_package( r.try_identifier_id( packageName ? packageName : "" ) ) );
		c = !! package ? static_cast<HPackage*>( package.raw() )->exception_class() : exception_class();
	}
	thread_->raise( c, message_, position_ );
	return;
	M_EPILOG
}

bool HStream::post_io( huginn::HThread* thread_, int long expected_, int long actual_, IO io_, int position_, HHuginn::HClass const* excClass_ ) const {
	M_PROLOG
	if ( actual_ < 0 ) {
		raise( thread_, io_ == IO::READ ? "Read failed." : "Write failed.", position_, excClass_ );
	} else if ( actual_ != expected_ ) {
		raise( thread_, io_ == IO::READ ? "Not enough data in the stream." : "Stream overflow on write.", position_, excClass_ );
	}
	return ( expected_ == actual_ );
	M_EPILOG
}

HHuginn::value_t HStream::read_fwd( char const* name_, reader_t reader_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( name_, values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = ( static_cast<HStream*>( object_->raw() )->*reader_ )( thread_, get_integer( values_[0] ), position_ );
	} catch ( HException const& e ) {
		HStream* s( static_cast<HStream*>( object_->raw() ) );
		s->raise( thread_, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HStream::write_fwd( char const* name_, HHuginn::TYPE type_, writer_t writer_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( name_, values_, { type_, HHuginn::TYPE::INTEGER }, thread_, position_ );
	try {
		( static_cast<HStream*>( object_->raw() )->*writer_ )( thread_, values_[0], get_integer( values_[1] ), position_ );
	} catch ( HException const& e ) {
		HStream* s( static_cast<HStream*>( object_->raw() ) );
		s->raise( thread_, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HStream::read_line( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Stream.read_line", values_, 0, 0, thread_, position_ );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	return ( s->read_line_impl( thread_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HStream::deserialize( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Stream.deserialize", values_, 0, 0, thread_, position_ );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = s->deserialize_impl( thread_, position_ );
	} catch ( HException const& e ) {
		s->raise( thread_, e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HStream::write_line( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Stream.write", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	s->write_line_impl( thread_, get_string( values_[0] ), position_ );
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HStream::serialize( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "Stream.serialize", values_, 1, 1, thread_, position_ );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	s->serialize_impl( thread_, values_[0], position_ );
	return ( *object_ );
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
		s->raise( thread_, e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HStream::read_blob( HThread* thread_, HHuginn::HInteger::value_type toRead_, int position_ ) {
	M_PROLOG
	if ( toRead_ < 0 ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid read size.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HChunk c;
	int long toRead( safe_int::cast<int long>( toRead_ ) );
	int long nRead( 0 );
	if ( toRead > 0 ) {
		_buffer.realloc( toRead );
		nRead = _stream->read( _buffer.raw(), toRead );
	}
	if ( nRead > 0 ) {
		c.realloc( nRead, HChunk::STRATEGY::EXACT );
		memcpy( c.raw(), _buffer.raw(), static_cast<size_t>( nRead ) );
	}
	post_io( thread_, nRead, nRead, IO::READ, position_ );
	return ( thread_->runtime().object_factory()->create_blob( yaal::move( c ) ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_string( HThread* thread_, HHuginn::HInteger::value_type toRead_, int position_ ) {
	M_PROLOG
	if ( toRead_ < 0 ) {
		throw HHuginn::HHuginnRuntimeException(
			"Invalid read size.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HString s;
	int long toRead( safe_int::cast<int long>( toRead_ ) );
	int long nRead( 0 );
	if ( toRead > 0 ) {
		_buffer.realloc( toRead );
		nRead = _stream->read( _buffer.raw(), toRead );
	}
	if ( nRead > 0 ) {
		try {
			s.assign( _buffer.get<char>(), nRead );
		} catch ( HStringException const& e ) {
			raise( thread_, e.what(), position_, thread_->runtime().object_factory()->conversion_exception_class() );
		}
	}
	post_io( thread_, nRead, nRead, IO::READ, position_ );
	return ( thread_->object_factory().create_string( s ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_integer( HThread* thread_, HHuginn::HInteger::value_type toRead_, int position_ ) {
	M_PROLOG
	HHuginn::HInteger::value_type val( 0 );
	int toRead( safe_int::cast<int>( toRead_ ) );
	int nRead( 0 );
	switch ( toRead ) {
		case ( sizeof ( yaal::i8_t ) ): {
			yaal::i8_t i8( 0 );
			nRead = static_cast<int>( _stream->read( &i8, toRead ) );
			val = i8;
		} break;
		case ( sizeof ( yaal::i16_t ) ): {
			yaal::i16_t i16( 0 );
			nRead = static_cast<int>( _stream->read( &i16, toRead ) );
			val = i16;
		} break;
		case ( sizeof ( yaal::i32_t ) ): {
			yaal::i32_t i32( 0 );
			nRead = static_cast<int>( _stream->read( &i32, toRead ) );
			val = i32;
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, toRead ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toRead, nRead, IO::READ, position_ );
	return ( thread_->object_factory().create_integer( val ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_integer_unsigned( HThread* thread_, HHuginn::HInteger::value_type toRead_, int position_ ) {
	M_PROLOG
	HHuginn::HInteger::value_type val( 0 );
	int toRead( safe_int::cast<int>( toRead_ ) );
	int nRead( 0 );
	switch ( toRead ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( 0 );
			nRead = static_cast<int>( _stream->read( &u8, toRead ) );
			val = static_cast<HHuginn::HInteger::value_type>( u8 );
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( 0 );
			nRead = static_cast<int>( _stream->read( &u16, toRead ) );
			val = static_cast<HHuginn::HInteger::value_type>( u16 );
		} break;
		case ( sizeof ( yaal::u32_t ) ): {
			yaal::u32_t u32( 0 );
			nRead = static_cast<int>( _stream->read( &u32, toRead ) );
			val = static_cast<HHuginn::HInteger::value_type>( u32 );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, toRead ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toRead, nRead, IO::READ, position_ );
	return ( thread_->object_factory().create_integer( val ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_real( HThread* thread_, HHuginn::HInteger::value_type toRead_, int position_ ) {
	M_PROLOG
	HHuginn::HReal::value_type val( 0 );
	int toRead( safe_int::cast<int>( toRead_ ) );
	int nRead( 0 );
	switch ( toRead ) {
		case ( sizeof ( float ) ): {
			float f( 0 );
			nRead = static_cast<int>( _stream->read( &f, toRead ) );
			val = f;
		} break;
#if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG
		case ( sizeof ( double ) ): {
			double d( 0 );
			nRead = static_cast<int>( _stream->read( &d, toRead ) );
			val = d;
		} break;
#endif /* #if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG */
		case ( sizeof ( HHuginn::HReal::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, toRead ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toRead, nRead, IO::READ, position_ );
	return ( thread_->object_factory().create_real( val ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_character( HThread* thread_, HHuginn::HInteger::value_type toRead_, int position_ ) {
	M_PROLOG
	HHuginn::HCharacter::value_type::value_type val( 0 );
	int toRead( safe_int::cast<int>( toRead_ ) );
	int nRead( 0 );
	switch ( toRead ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( 0 );
			nRead = static_cast<int>( _stream->read( &u8, toRead ) );
			val = u8;
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( 0 );
			nRead = static_cast<int>( _stream->read( &u16, toRead ) );
			val = u16;
		} break;
		case ( sizeof ( HHuginn::HCharacter::value_type::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, toRead ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	code_point_t cp( val );
	if ( post_io( thread_, toRead, nRead, IO::READ, position_ ) ) {
		try {
			unicode::rank( cp );
		} catch ( HException const& e ) {
			raise( thread_, e.what(), position_, thread_->runtime().object_factory()->conversion_exception_class() );
		}
	}
	return ( thread_->object_factory().create_character( code_point_t( cp ) ) );
	M_EPILOG
}

void HStream::write_blob( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type toWrite_, int position_ ) {
	M_PROLOG
	int long toWrite( safe_int::cast<int long>( toWrite_ ) );
	HHuginn::HBlob const& blob( *static_cast<HHuginn::HBlob const*>( value_.raw() ) );
	if ( toWrite <= blob.get_size() ) {
		int long nWritten( _stream->write( blob.value().raw(), toWrite ) );
		post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	} else {
		raise( thread_, "Write bigger than source blob.", position_, thread_->runtime().object_factory()->runtime_exception_class() );
	}
	return;
	M_EPILOG
}

void HStream::write_string( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type toWrite_, int position_ ) {
	M_PROLOG
	int long toWrite( safe_int::cast<int long>( toWrite_ ) );
	HHuginn::HString::value_type const& s( static_cast<HHuginn::HString const*>( value_.raw() )->value() );
	if ( toWrite <= s.get_length() ) {
		_converter.assign( s.begin(), s.begin() + toWrite );
		int long nWritten( _stream->write( _converter.c_str(), _converter.byte_count() ) );
		post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	} else {
		raise( thread_, "Write bigger than source string.", position_, thread_->runtime().object_factory()->runtime_exception_class() );
	}
	return;
	M_EPILOG
}

void HStream::write_integer( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type toWrite_, int position_ ) {
	M_PROLOG
	int long toWrite( safe_int::cast<int long>( toWrite_ ) );
	HHuginn::HInteger::value_type val( static_cast<HHuginn::HInteger const*>( value_.raw() )->value() );
	int nWritten( 0 );
	switch ( toWrite ) {
		case ( sizeof ( yaal::i8_t ) ): {
			yaal::i8_t i8( safe_int::cast<yaal::i8_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &i8, toWrite ) );
		} break;
		case ( sizeof ( yaal::i16_t ) ): {
			yaal::i16_t i16( safe_int::cast<yaal::i16_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &i16, toWrite ) );
		} break;
		case ( sizeof ( yaal::i32_t ) ): {
			yaal::i32_t i32( safe_int::cast<yaal::i32_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &i32, toWrite ) );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nWritten = static_cast<int>( _stream->write( &val, toWrite ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	return;
	M_EPILOG
}

void HStream::write_integer_unsigned( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type toWrite_, int position_ ) {
	M_PROLOG
	int long toWrite( safe_int::cast<int long>( toWrite_ ) );
	HHuginn::HInteger::value_type val( static_cast<HHuginn::HInteger const*>( value_.raw() )->value() );
	if ( ( val < 0 ) && ( toWrite != static_cast<int>( sizeof ( HHuginn::HInteger::value_type ) ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Missuse of _unsigned_ integer writer.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	int nWritten( 0 );
	switch ( toWrite ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( safe_int::cast<yaal::u8_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &u8, toWrite ) );
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( safe_int::cast<yaal::u16_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &u16, toWrite ) );
		} break;
		case ( sizeof ( yaal::u32_t ) ): {
			yaal::u32_t u32( safe_int::cast<yaal::u32_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &u32, toWrite ) );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nWritten = static_cast<int>( _stream->write( &val, toWrite ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	return;
	M_EPILOG
}

void HStream::write_real( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type toWrite_, int position_ ) {
	M_PROLOG
	int long toWrite( safe_int::cast<int long>( toWrite_ ) );
	HHuginn::HReal::value_type val( static_cast<HHuginn::HReal const*>( value_.raw() )->value() );
	int nWritten( 0 );
	switch ( toWrite ) {
		case ( sizeof ( float ) ): {
			float f( static_cast<float>( val ) );
			nWritten = static_cast<int>( _stream->write( &f, toWrite ) );
		} break;
#if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG
		case ( sizeof ( double ) ): {
			double d( static_cast<double>( val ) );
			nWritten = static_cast<int>( _stream->write( &d, toWrite ) );
		} break;
#endif /* #if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG */
		case ( sizeof ( HHuginn::HReal::value_type ) ): {
			nWritten = static_cast<int>( _stream->write( &val, toWrite ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	return;
	M_EPILOG
}

void HStream::write_character( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type toWrite_, int position_ ) {
	M_PROLOG
	int long toWrite( safe_int::cast<int long>( toWrite_ ) );
	HHuginn::HCharacter::value_type::value_type val( static_cast<HHuginn::HCharacter const*>( value_.raw() )->value().get() );
	int nWritten( 0 );
	switch ( toWrite ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( safe_int::cast<yaal::u8_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &u8, toWrite ) );
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( safe_int::cast<yaal::u16_t>( val ) );
			nWritten = static_cast<int>( _stream->write( &u16, toWrite ) );
		} break;
		case ( sizeof ( HHuginn::HCharacter::value_type ) ): {
			nWritten = static_cast<int>( _stream->write( &val, toWrite ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HStream::read_line_raw( HThread* thread_, int position_ ) {
	M_PROLOG
	try {
		_stream->read_until( _lineBuffer, HStreamInterface::eols, false );
	} catch ( HStringException const& e ) {
		raise( thread_, e.what(), position_, thread_->runtime().object_factory()->conversion_exception_class() );
	} catch ( HException const& e ) {
		raise( thread_, e.what(), position_ );
	}
	return ( _lineBuffer );
	M_EPILOG
}

HHuginn::value_t HStream::read_line_impl( HThread* thread_, int position_ ) {
	M_PROLOG
	HString const& line( read_line_raw( thread_, position_ ) );
	return ( ! line.is_empty() ? thread_->object_factory().create_string( line ) : thread_->runtime().none_value() );
	M_EPILOG
}

int long HStream::read_len( HThread* thread_, int position_ ) {
	int long len( 0 );
	int long toRead = static_cast<int>( sizeof ( len ) );
	int long nRead = _stream->read( &len, toRead );
	if ( ( nRead != toRead ) || ( len < 0 ) ) {
		raise( thread_, "Malformed Huginn data stream.", position_, exception_class() );
		len = -1;
	}
	return ( len );
}

HHuginn::value_t HStream::deserialize_impl( HThread* thread_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v( thread_->runtime().none_value() );
	u8_t tRaw( 0 );
	int long toRead( static_cast<int>( sizeof ( tRaw ) ) );
	int long nRead( _stream->read( &tRaw, toRead ) );
	int long len( 0 );
	if ( ! post_io( thread_, toRead, nRead, IO::READ, position_ ) ) {
		return ( thread_->runtime().none_value() );
	}
	HHuginn::TYPE t( static_cast<HHuginn::TYPE>( tRaw ) );
	HFrame* f( thread_->current_frame() );
	switch ( t ) {
		case ( HHuginn::TYPE::NONE ): break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			i8_t i8( 0 );
			toRead = 1;
			nRead = _stream->read( &i8, toRead );
			if ( nRead != toRead ) {
				break;
			}
			if ( ( i8 != 0 ) && ( i8 != 1 ) ) {
				raise( thread_, "Malformed Huginn data stream.", position_, exception_class() );
				break;
			}
			v = i8 ? thread_->runtime().true_value() : thread_->runtime().false_value();
		} break;
		case ( HHuginn::TYPE::INTEGER ): {
			v = read_integer( thread_, static_cast<int>( sizeof ( HHuginn::HInteger::value_type ) ), position_ );
		} break;
		case ( HHuginn::TYPE::REAL ): {
			v = read_real( thread_, static_cast<int>( sizeof ( HHuginn::HReal::value_type ) ), position_ );
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			v = read_character( thread_, static_cast<int>( sizeof ( HHuginn::HCharacter::value_type::value_type ) ), position_ );
		} break;
		case ( HHuginn::TYPE::STRING ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			v = read_string( thread_, len, position_ );
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			v = HNumber::ElementaryFunctions::deserialize( thread_, _stream, this, position_ );
		} break;
		case ( HHuginn::TYPE::TUPLE ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			HHuginn::HTuple::values_t data;
			data.reserve( len );
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				data.push_back( deserialize_impl( thread_, position_ ) );
			}
			v = thread_->object_factory().create_tuple( yaal::move( data ) );
		} break;
		case ( HHuginn::TYPE::LIST ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			HHuginn::HList::values_t data;
			data.reserve( len );
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				data.push_back( deserialize_impl( thread_, position_ ) );
			}
			v = thread_->object_factory().create_list( yaal::move( data ) );
		} break;
		case ( HHuginn::TYPE::DEQUE ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			HHuginn::HDeque::values_t data;
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				data.push_back( deserialize_impl( thread_, position_ ) );
			}
			v = thread_->object_factory().create_deque( yaal::move( data ) );
		} break;
		case ( HHuginn::TYPE::DICT ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			v = thread_->object_factory().create_dict();
			HHuginn::HDict& val( *static_cast<HHuginn::HDict*>( v.raw() ) );
			HHuginn::HDict::values_t& data( val.value() );
			HHuginn::HClass const* keyType( nullptr );
			HAnchorGuard<HHuginn::HDict> ag( val, thread_, position_ );
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				HHuginn::value_t key( deserialize_impl( thread_, position_ ) );
				HHuginn::HClass const* newKeyType( key->get_class() );
				if ( ! is_comparable( newKeyType ) || ( keyType && ( newKeyType != keyType ) ) ) {
					raise( thread_, "Malformed Huginn data stream.", position_, exception_class() );
					break;
				}
				keyType = newKeyType;
				HHuginn::value_t value( deserialize_impl( thread_, position_ ) );
				data.insert( make_pair( key, value ) );
			}
			if ( keyType ) {
				val.update_key_type( thread_, keyType, position_ );
			}
		} break;
		case ( HHuginn::TYPE::LOOKUP ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			v = thread_->object_factory().create_lookup();
			HHuginn::HLookup& val( *static_cast<HHuginn::HLookup*>( v.raw() ) );
			HHuginn::HLookup::values_t& data( val.value() );
			HAnchorGuard<HHuginn::HLookup> ag( val, thread_, position_ );
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				HHuginn::value_t key( deserialize_impl( thread_, position_ ) );
				HHuginn::value_t value( deserialize_impl( thread_, position_ ) );
				data.insert( make_pair( key, value ) );
			}
		} break;
		case ( HHuginn::TYPE::ORDER ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			v = thread_->object_factory().create_order();
			HHuginn::HOrder& val( *static_cast<HHuginn::HOrder*>( v.raw() ) );
			HHuginn::HOrder::values_t& data( val.value() );
			HHuginn::HClass const* keyType( nullptr );
			HAnchorGuard<HHuginn::HOrder> ag( val, thread_, position_ );
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				HHuginn::value_t key( deserialize_impl( thread_, position_ ) );
				HHuginn::HClass const* newKeyType( key->get_class() );
				if ( ! is_comparable( newKeyType ) || ( keyType && ( newKeyType != keyType ) ) ) {
					raise( thread_, "Malformed Huginn data stream.", position_, exception_class() );
					break;
				}
				keyType = newKeyType;
				data.insert( key );
			}
			if ( keyType ) {
				val.update_key_type( thread_, keyType, position_ );
			}
		} break;
		case ( HHuginn::TYPE::SET ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			v = thread_->object_factory().create_set();
			HHuginn::HSet& val( *static_cast<HHuginn::HSet*>( v.raw() ) );
			HHuginn::HSet::values_t& data( val.value() );
			HAnchorGuard<HHuginn::HSet> ag( val, thread_, position_ );
			for ( int long i( 0 ); f->can_continue() && ( i < len ); ++ i ) {
				data.insert( deserialize_impl( thread_, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ): {
			if ( ( len = read_len( thread_, position_ ) ) < 0 ) {
				break;
			}
			_buffer.realloc( len );
			_stream->read( _buffer.raw(), len );
			try {
				HString n( _buffer.get<char>(), len );
				HHuginn::identifier_id_t id( thread_->runtime().identifier_id( n ) );
				HHuginn::value_t const* fun( thread_->runtime().get_global( id ) );
				if ( ! fun ) {
					raise( thread_, "Function `"_ys.append( n ).append( "' is not defined." ), position_, exception_class() );
				} else if ( (*fun)->type_id() != HHuginn::TYPE::FUNCTION_REFERENCE ) {
					raise( thread_, "Symbol `"_ys.append( n ).append( "' is not a function." ), position_, exception_class() );
				} else {
					v = *fun;
				}
			} catch ( HStringException const& ) {
				raise( thread_, "Malformed Huginn data stream.", position_, exception_class() );
			}
		} break;
		default: {
			raise( thread_, "Unknown value type: "_ys.append( static_cast<int>( t ) ), position_, exception_class() );
		}
	}
	post_io( thread_, toRead, nRead, IO::READ, position_ );
	return ( v );
	M_EPILOG
}

void HStream::write_line_impl( HThread* thread_, HString const& val_, int position_ ) {
	M_PROLOG
	try {
		_converter = val_;
		int long toWrite( _converter.byte_count() );
		int long nWritten( _stream->write( _converter.c_str(), toWrite ) );
		post_io( thread_, toWrite, nWritten, IO::WRITE, position_ );
	} catch ( HException const& e ) {
		raise( thread_, e.what(), position_ );
	}
	return;
	M_EPILOG
}

void HStream::serialize_impl( HThread* thread_, HHuginn::value_t const& val_, int position_ ) {
	M_PROLOG
	try {
		HSemanticBuffer sb( _buffer, _converter, thread_, position_ );
		sb.serialize( val_ );
		int long nWritten( _stream->write( sb.data(), sb.size() ) );
		post_io( thread_, sb.size(), nWritten, IO::WRITE, position_ );
	} catch ( HException const& e ) {
		raise( thread_, e.what(), position_ );
	}
	return;
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

HHuginn::HIterable::iterator_t HStream::do_iterator( HThread* thread_, int position_ ) {
	return ( make_pointer<HStreamIterator>( thread_, this, position_ ) );
}

int long HStream::do_size( huginn::HThread* thread_, int position_ ) const {
	throw HHuginn::HHuginnRuntimeException( "Getting size of `Stream' is an invalid operation.", thread_->current_frame()->file_id(), position_ );
}

HHuginn::class_t HStream::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::identifier_id_t classIdentifier( runtime_->identifier_id( "Stream" ) );
	HHuginn::class_t c( runtime_->get_class( classIdentifier ) );
	if ( ! c ) {
		c = runtime_->create_class(
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
		runtime_->huginn()->register_class( c, HHuginn::VISIBILITY::GLOBAL );
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

