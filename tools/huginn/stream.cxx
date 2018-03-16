/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

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
#include "hcore/unicode.hxx"

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
	HHuginn::class_t _exceptionClass;
public:
	HStreamClass( HRuntime* runtime_, HHuginn::type_id_t typeId_, HHuginn::identifier_id_t identifierId_ )
		: HClass(
			runtime_,
			typeId_,
			identifierId_,
			nullptr,
			"The `Stream` class gives an interface for stream based I/O operations."
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
			{ "seek",            runtime_->create_method( &HStream::seek ),                                                           "( *offset*, *anchor* ) - move reading/writing position to the *offset* counted from an *anchor*" },
			{ "read_line",       runtime_->create_method( &HStream::read_line ),                                                      "read single line of text from this stream" },
			{ "write_line",      runtime_->create_method( &HStream::write_line ),                                                     "( *strVal* ) - write entriety of given *strVal* info this stream" }
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

HHuginn::HClass const* HStream::exception_class( void ) const {
	return ( static_cast<HStreamClass const*>( HValue::get_class() )->exception_class() );
}

HHuginn::value_t HStream::read_fwd( char const* name_, reader_t reader_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( name_, values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = ( static_cast<HStream*>( object_->raw() )->*reader_ )( thread_, get_integer( values_[0] ), position_ );
	} catch ( HException const& e ) {
		raise( e, thread_, position_ );
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
		raise( e, thread_, position_ );
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

HHuginn::value_t HStream::write_line( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Stream.write", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	HStream* s( static_cast<HStream*>( object_->raw() ) );
	s->write_line_impl( thread_, get_string( values_[0] ), position_ );
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
		raise( e, thread_, position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HStream::read_blob( HThread* thread_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	HChunk c;
	int long count( safe_int::cast<int long>( count_ ) );
	_buffer.realloc( count );
	int long nRead( _stream->read( _buffer.raw(), count ) );
	if ( nRead > 0 ) {
		c.realloc( nRead, HChunk::STRATEGY::EXACT );
		memcpy( c.raw(), _buffer.raw(), static_cast<size_t>( nRead ) );
	} else if ( nRead < 0 ) {
		thread_->raise( exception_class(), "Invalid read", position_ );
	}
	return ( thread_->runtime().object_factory()->create_blob( yaal::move( c ) ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_string( HThread* thread_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	HString s;
	int long count( safe_int::cast<int long>( count_ ) );
	_buffer.realloc( count );
	int long nRead( _stream->read( _buffer.raw(), count ) );
	if ( nRead > 0 ) {
		s.assign( _buffer.get<char>(), nRead );
	} else if ( nRead < 0 ) {
		thread_->raise( exception_class(), "Read failed.", position_ );
	}
	return ( thread_->object_factory().create_string( s ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_integer( HThread* thread_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	HHuginn::HInteger::value_type val( 0 );
	int count( safe_int::cast<int>( count_ ) );
	int nRead( 0 );
	switch ( count ) {
		case ( sizeof ( yaal::i8_t ) ): {
			yaal::i8_t i8( 0 );
			nRead = static_cast<int>( _stream->read( &i8, count ) );
			val = i8;
		} break;
		case ( sizeof ( yaal::i16_t ) ): {
			yaal::i16_t i16( 0 );
			nRead = static_cast<int>( _stream->read( &i16, count ) );
			val = i16;
		} break;
		case ( sizeof ( yaal::i32_t ) ): {
			yaal::i32_t i32( 0 );
			nRead = static_cast<int>( _stream->read( &i32, count ) );
			val = i32;
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nRead != count ) {
		thread_->raise( exception_class(), "Not enough data in the stream.", position_ );
	}
	return ( thread_->object_factory().create_integer( val ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_integer_unsigned( HThread* thread_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	HHuginn::HInteger::value_type val( 0 );
	int count( safe_int::cast<int>( count_ ) );
	int nRead( 0 );
	switch ( count ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( 0 );
			nRead = static_cast<int>( _stream->read( &u8, count ) );
			val = static_cast<HHuginn::HInteger::value_type>( u8 );
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( 0 );
			nRead = static_cast<int>( _stream->read( &u16, count ) );
			val = static_cast<HHuginn::HInteger::value_type>( u16 );
		} break;
		case ( sizeof ( yaal::u32_t ) ): {
			yaal::u32_t u32( 0 );
			nRead = static_cast<int>( _stream->read( &u32, count ) );
			val = static_cast<HHuginn::HInteger::value_type>( u32 );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nRead != count ) {
		thread_->raise( exception_class(), "Not enough data in the stream.", position_ );
	}
	return ( thread_->object_factory().create_integer( val ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_real( HThread* thread_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	HHuginn::HReal::value_type val( 0 );
	int count( safe_int::cast<int>( count_ ) );
	int nRead( 0 );
	switch ( count ) {
		case ( sizeof ( float ) ): {
			float f( 0 );
			nRead = static_cast<int>( _stream->read( &f, count ) );
			val = f;
		} break;
#if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG
		case ( sizeof ( double ) ): {
			double d( 0 );
			nRead = static_cast<int>( _stream->read( &d, count ) );
			val = d;
		} break;
#endif /* #if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG */
		case ( sizeof ( HHuginn::HReal::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid read size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nRead != count ) {
		thread_->raise( exception_class(), "Not enough data in the stream.", position_ );
	}
	return ( thread_->object_factory().create_real( val ) );
	M_EPILOG
}

HHuginn::value_t HStream::read_character( HThread* thread_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	HHuginn::HCharacter::value_type::value_type val( 0 );
	int count( safe_int::cast<int>( count_ ) );
	int nRead( 0 );
	switch ( count ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( 0 );
			nRead = static_cast<int>( _stream->read( &u8, count ) );
			val = u8;
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( 0 );
			nRead = static_cast<int>( _stream->read( &u16, count ) );
			val = u16;
		} break;
		case ( sizeof ( HHuginn::HCharacter::value_type::value_type ) ): {
			nRead = static_cast<int>( _stream->read( &val, count ) );
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
	unicode::rank( cp );
	if ( nRead != count ) {
		thread_->raise( exception_class(), "Not enough data in the stream.", position_ );
	}
	return ( thread_->object_factory().create_character( code_point_t( cp ) ) );
	M_EPILOG
}

void HStream::write_blob( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	int long count( safe_int::cast<int long>( count_ ) );
	HHuginn::HBlob const& blob( *static_cast<HHuginn::HBlob const*>( value_.raw() ) );
	if ( count <= blob.get_size() ) {
		int long nWrite( _stream->write( blob.value().raw(), count ) );
		if ( nWrite != count ) {
			thread_->raise( exception_class(), "Write failed.", position_ );
		}
	} else {
		thread_->raise( exception_class(), "Write bigger than source blob.", position_ );
	}
	return;
	M_EPILOG
}

void HStream::write_string( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	int long count( safe_int::cast<int long>( count_ ) );
	HHuginn::HString::value_type const& s( static_cast<HHuginn::HString const*>( value_.raw() )->value() );
	if ( count <= s.get_length() ) {
		_converter.assign( s.begin(), s.begin() + count );
		int long nWrite( _stream->write( _converter.c_str(), _converter.byte_count() ) );
		if ( nWrite != _converter.byte_count() ) {
			thread_->raise( exception_class(), "Write failed.", position_ );
		}
	} else {
		thread_->raise( exception_class(), "Write bigger than source string.", position_ );
	}
	return;
	M_EPILOG
}

void HStream::write_integer( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	int long count( safe_int::cast<int long>( count_ ) );
	HHuginn::HInteger::value_type val( static_cast<HHuginn::HInteger const*>( value_.raw() )->value() );
	int nWrite( 0 );
	switch ( count ) {
		case ( sizeof ( yaal::i8_t ) ): {
			yaal::i8_t i8( static_cast<yaal::i8_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &i8, count ) );
		} break;
		case ( sizeof ( yaal::i16_t ) ): {
			yaal::i16_t i16( static_cast<yaal::i16_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &i16, count ) );
		} break;
		case ( sizeof ( yaal::i32_t ) ): {
			yaal::i32_t i32( static_cast<yaal::i32_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &i32, count ) );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nWrite = static_cast<int>( _stream->write( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nWrite != count ) {
		thread_->raise( exception_class(), "Write failed.", position_ );
	}
	return;
	M_EPILOG
}

void HStream::write_integer_unsigned( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	int long count( safe_int::cast<int long>( count_ ) );
	HHuginn::HInteger::value_type val( static_cast<HHuginn::HInteger const*>( value_.raw() )->value() );
	if ( ( val < 0 ) && ( count != static_cast<int>( sizeof ( HHuginn::HInteger::value_type ) ) ) ) {
		throw HHuginn::HHuginnRuntimeException(
			"Missuse of _unsigned_ integer writer.",
			thread_->current_frame()->file_id(),
			position_
		);
	}
	int nWrite( 0 );
	switch ( count ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( static_cast<yaal::u8_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &u8, count ) );
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( static_cast<yaal::u16_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &u16, count ) );
		} break;
		case ( sizeof ( yaal::u32_t ) ): {
			yaal::u32_t u32( static_cast<yaal::u32_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &u32, count ) );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nWrite = static_cast<int>( _stream->write( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nWrite != count ) {
		thread_->raise( exception_class(), "Write failed.", position_ );
	}
	return;
	M_EPILOG
}

void HStream::write_real( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	int long count( safe_int::cast<int long>( count_ ) );
	HHuginn::HReal::value_type val( static_cast<HHuginn::HReal const*>( value_.raw() )->value() );
	int nWrite( 0 );
	switch ( count ) {
		case ( sizeof ( float ) ): {
			float f( static_cast<float>( val ) );
			nWrite = static_cast<int>( _stream->write( &f, count ) );
		} break;
#if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG
		case ( sizeof ( double ) ): {
			double d( static_cast<double>( val ) );
			nWrite = static_cast<int>( _stream->write( &d, count ) );
		} break;
#endif /* #if SIZEOF_DOUBLE != SIZEOF_DOUBLE_LONG */
		case ( sizeof ( HHuginn::HReal::value_type ) ): {
			nWrite = static_cast<int>( _stream->write( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nWrite != count ) {
		thread_->raise( exception_class(), "Write failed.", position_ );
	}
	return;
	M_EPILOG
}

void HStream::write_character( HThread* thread_, HHuginn::value_t const& value_, HHuginn::HInteger::value_type count_, int position_ ) {
	M_PROLOG
	int long count( safe_int::cast<int long>( count_ ) );
	HHuginn::HCharacter::value_type::value_type val( static_cast<HHuginn::HCharacter const*>( value_.raw() )->value().get() );
	int nWrite( 0 );
	switch ( count ) {
		case ( sizeof ( yaal::u8_t ) ): {
			yaal::u8_t u8( static_cast<yaal::u8_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &u8, count ) );
		} break;
		case ( sizeof ( yaal::u16_t ) ): {
			yaal::u16_t u16( static_cast<yaal::u16_t>( val ) );
			nWrite = static_cast<int>( _stream->write( &u16, count ) );
		} break;
		case ( sizeof ( HHuginn::HInteger::value_type ) ): {
			nWrite = static_cast<int>( _stream->write( &val, count ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid write size.",
				thread_->current_frame()->file_id(),
				position_
			);
		}
	}
	if ( nWrite != count ) {
		thread_->raise( exception_class(), "Write failed.", position_ );
	}
	return;
	M_EPILOG
}

yaal::hcore::HString const& HStream::read_line_raw( HThread* thread_, int position_ ) {
	M_PROLOG
	try {
		_stream->read_until( _lineBuffer, HStreamInterface::eols, false );
	} catch ( HException const& e ) {
		raise( e, thread_, position_ );
	}
	return ( _lineBuffer );
	M_EPILOG
}

HHuginn::value_t HStream::read_line_impl( HThread* thread_, int position_ ) {
	M_PROLOG
	return ( thread_->object_factory().create_string( read_line_raw( thread_, position_ ) ) );
	M_EPILOG
}

void HStream::write_line_impl( HThread* thread_, HString const& val_, int position_ ) {
	M_PROLOG
	try {
		_converter = val_;
		_stream->write( _converter.c_str(), _converter.byte_count() );
	} catch ( HException const& e ) {
		raise( e, thread_, position_ );
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

HHuginn::HIterable::HIterator HStream::do_iterator( HThread* thread_, int position_ ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HStreamIterator( thread_, this, position_ ) );
	return ( HIterator( yaal::move( impl ) ) );
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

