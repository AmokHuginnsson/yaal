/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "tools/huginn/iterable.hxx"
#include "tools/huginn/integer.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HStream : public huginn::HIterable {
public:
	typedef HHuginn::value_t ( HStream::*reader_t )( HThread*, huginn::HInteger::value_type, int );
	typedef void ( HStream::*writer_t )( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	enum class IO {
		READ,
		WRITE
	};
private:
	yaal::hcore::HStreamInterface::ptr_t _stream;
	yaal::hcore::HChunk _buffer;
	yaal::hcore::HString _lineBuffer;
	yaal::hcore::HUTF8String _converter;
public:
	HStream( HClass const*, yaal::hcore::HStreamInterface::ptr_t );
	static HHuginn::value_t read_fwd( char const*, reader_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t read_line( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t flush( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t deserialize( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t write_fwd( char const*, HHuginn::TYPE, writer_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t write_line( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t serialize( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t seek( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	bool is_valid( void ) const;
	static HHuginn::class_t get_class( HRuntime* );
	void raise( HThread*, yaal::hcore::HString const&, int, HClass const* = nullptr ) const;
	bool post_io( huginn::HThread*, int long, int long, IO, int ) const;
	void write( HThread*, HHuginn::value_t const&, int );
	HClass const* exception_class( void ) const;
	yaal::hcore::HStreamInterface::ptr_t raw( void ) const;
private:
	HHuginn::value_t read_blob( HThread*, huginn::HInteger::value_type, int );
	HHuginn::value_t read_string( HThread*, huginn::HInteger::value_type, int );
	HHuginn::value_t read_integer( HThread*, huginn::HInteger::value_type, int );
	HHuginn::value_t read_integer_unsigned( HThread*, huginn::HInteger::value_type, int );
	HHuginn::value_t read_real( HThread*, huginn::HInteger::value_type, int );
	HHuginn::value_t read_character( HThread*, huginn::HInteger::value_type, int );
	HHuginn::value_t read_line_impl( HThread*, int );
	HHuginn::value_t deserialize_impl( HThread*, int );
	yaal::hcore::HString& read_line_raw( HThread*, int );
	void write_blob( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	void write_string( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	void write_integer( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	void write_integer_unsigned( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	void write_real( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	void write_character( HThread*, HHuginn::value_t const&, huginn::HInteger::value_type, int );
	void write_line_impl( HThread*, yaal::hcore::HString const&, int );
	void serialize_impl( HThread*, HHuginn::value_t const&, int );
	void seek_impl( int long, yaal::hcore::HStreamInterface::SEEK );
	virtual iterator_t do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override __attribute__((noreturn));
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
	int long read_len( HThread*, int );
private:
	friend class HStreamClass;
	friend class HStreamIterator;
};

class HStreamDecorator : public yaal::hcore::HStreamInterface {
public:
	typedef HStreamDecorator this_type;
	typedef yaal::hcore::HStreamInterface base_type;
private:
	HHuginn::value_t _value;
	yaal::hcore::HStreamInterface::ptr_t _stream;
public:
	HStreamDecorator( HHuginn::value_t const& value_, yaal::hcore::HStreamInterface::ptr_t const& stream_ )
		: _value( value_ )
		, _stream( stream_ ) {
		return;
	}
private:
	virtual int long do_write( void const* data_, int long size_ ) override {
		return ( _stream->write( data_, size_ ) );
	}
	virtual void do_flush( void ) override {
		_stream->flush();
	}
	virtual int long do_read( void* data_, int long size_ ) override {
		return ( _stream->read( data_, size_ ) );
	}
	virtual int long do_read_some( void* data_, int long size_ ) override {
		return ( _stream->read_some( data_, size_ ) );
	}
	virtual void do_seek( int long pos_, SEEK seek_ ) override {
		_stream->seek( pos_, seek_ );
	}
	virtual bool do_is_valid( void ) const override {
		return ( _stream->is_valid() );
	}
	virtual POLL_TYPE do_poll_type( void ) const override {
		return ( _stream->poll_type() );
	}
	virtual void const* do_data( void ) const override {
		return ( _stream->data() );
	}
	virtual int do_immediate_read_size( void ) const override {
		return ( _stream->immediate_read_size() );
	}
	virtual int do_pending_write_size( void ) const override {
		return ( _stream->pending_write_size() );
	}
	HStreamDecorator( HStreamDecorator const& src ) = delete;
	HStreamDecorator& operator = ( HStreamDecorator const& src ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED */

