/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HStream : public HHuginn::HIterable {
	yaal::hcore::HStreamInterface::ptr_t _stream;
	yaal::hcore::HChunk _buffer;
	yaal::hcore::HString _lineBuffer;
	yaal::hcore::HUTF8String _converter;
public:
	HStream( HHuginn::HClass const*, yaal::hcore::HStreamInterface::ptr_t );
	static HHuginn::value_t read( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t read_line( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t write( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t seek( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	yaal::hcore::HString const& read_line_impl( void );
	bool is_valid( void ) const;
	static HHuginn::class_t get_class( HRuntime* );
private:
	yaal::hcore::HString read_impl( int long );
	void write_impl( yaal::hcore::HString const& );
	void seek_impl( int long, yaal::hcore::HStreamInterface::SEEK );
	virtual HIterator do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override __attribute__((noreturn));
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_STREAM_HXX_INCLUDED */

