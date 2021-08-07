/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hsynchronizedstream.hxx
 * \brief Synchronize access to any stream.
 *
 * Declaration of HSynchronizedStream class.
 */

#ifndef YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED
#define YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hthread.hxx"
#include "hcore/trait.hxx"

namespace yaal {

namespace hcore {

/*! \brief HSynchronizedStream makes use of HStreamInterface derivates thread safe.
 */
class HSynchronizedStream : public HStreamInterface {
public:
	typedef HSynchronizedStream this_type;
	typedef HStreamInterface base_type;
	typedef HStreamInterface::ptr_t owned_stream_t;
	typedef HStreamInterface& ref_stream_t;
protected:
	mutable HMutex _mutex;
private:
	owned_stream_t _streamOwned;
	yaal::hcore::HStreamInterface* _streamRef;
public:
	HSynchronizedStream( void );
	explicit HSynchronizedStream( owned_stream_t );
	explicit HSynchronizedStream( ref_stream_t );
	void reset_owned( owned_stream_t = owned_stream_t() );
	void reset_referenced( ref_stream_t );
	HLock acquire( void );
	template<typename call_t>
	HSynchronizedStream& operator()( call_t );
protected:
	virtual HStreamInterface& do_output( HString const& ) override;
	virtual HStreamInterface& do_output( char const* ) override;
	virtual HStreamInterface& do_output( bool ) override;
	virtual HStreamInterface& do_output( code_point_t ) override;
	virtual HStreamInterface& do_output( char ) override;
	virtual HStreamInterface& do_output( char unsigned ) override;
	virtual HStreamInterface& do_output( int short ) override;
	virtual HStreamInterface& do_output( int short unsigned ) override;
	virtual HStreamInterface& do_output( int ) override;
	virtual HStreamInterface& do_output( int unsigned ) override;
	virtual HStreamInterface& do_output( int long ) override;
	virtual HStreamInterface& do_output( int long unsigned ) override;
	virtual HStreamInterface& do_output( int long long ) override;
	virtual HStreamInterface& do_output( int long long unsigned ) override;
	virtual HStreamInterface& do_output( double ) override;
	virtual HStreamInterface& do_output( double long ) override;
	virtual HStreamInterface& do_output( float ) override;
	virtual HStreamInterface& do_output( void const* ) override;
	virtual HStreamInterface& do_output( manipulator_t const& ) override;
	virtual HStreamInterface& do_output( HManipulator const& ) override;
	virtual HStreamInterface& do_input( HString& ) override;
	virtual HStreamInterface& do_input( bool& ) override;
	virtual HStreamInterface& do_input( code_point_t& ) override;
	virtual HStreamInterface& do_input( char& ) override;
	virtual HStreamInterface& do_input( char unsigned& ) override;
	virtual HStreamInterface& do_input( int short& ) override;
	virtual HStreamInterface& do_input( int short unsigned& ) override;
	virtual HStreamInterface& do_input( int& ) override;
	virtual HStreamInterface& do_input( int unsigned& ) override;
	virtual HStreamInterface& do_input( int long& ) override;
	virtual HStreamInterface& do_input( int long unsigned& ) override;
	virtual HStreamInterface& do_input( int long long& ) override;
	virtual HStreamInterface& do_input( int long long unsigned& ) override;
	virtual HStreamInterface& do_input( double& ) override;
	virtual HStreamInterface& do_input( double long& ) override;
	virtual HStreamInterface& do_input( float& ) override;
	virtual HStreamInterface& do_input( void const*& ) override;
	virtual HStreamInterface& do_input( manipulator_t const& ) override;
	virtual int long do_read_until( yaal::hcore::HString&, char const*, bool ) override;
	virtual int long do_read_until_n( yaal::hcore::HString&, int long, char const*, bool ) override;
	virtual int long do_read_while( yaal::hcore::HString&, char const* ) override;
	virtual int long do_read_while_n( yaal::hcore::HString&, int long, char const* ) override;
	virtual HStreamInterface& do_consume( yaal::hcore::HString const& ) override;
	virtual HStreamInterface& do_ignore( int ) override;
	virtual int do_peek( void ) override;
	virtual int do_immediate_read_size( void ) const override;
	virtual int do_pending_write_size( void ) const override;
	virtual HStreamInterface& do_set_fill( code_point_t ) override;
	virtual HStreamInterface& do_set_width( int ) override;
	virtual HStreamInterface& do_set_precision( int ) override;
	virtual HStreamInterface& do_set_mode( MODE ) override;
	virtual HStreamInterface& do_set_base( BASE ) override;
	virtual HStreamInterface& do_set_float_format( FLOAT_FORMAT ) override;
	virtual HStreamInterface& do_set_adjust( ADJUST ) override;
	virtual HStreamInterface& do_set_skipws( bool ) override;
	virtual HStreamInterface& do_set_boolalpha( bool ) override;
	virtual HStreamInterface& do_set_buffered_io( bool ) override;
	virtual HStreamInterface& do_set_io_buffer_size( int ) override;
	virtual bool do_get_skipws( void ) const override;
	virtual bool do_get_boolalpha( void ) const override;
	virtual bool do_get_buffered_io( void ) const override;
	virtual int do_get_io_buffer_size( void ) const override;
	virtual code_point_t do_get_fill( void ) const override;
	virtual int do_get_width( void ) const override;
	virtual int do_get_precision( void ) const override;
	virtual MODE do_get_mode( void ) const override;
	virtual BASE do_get_base( void ) const override;
	virtual FLOAT_FORMAT do_get_float_format( void ) const override;
	virtual ADJUST do_get_adjust( void ) const override;
	virtual bool do_good( void ) const override;
	virtual bool do_fail( void ) const override;
	virtual bool do_bad( void ) const override;
	virtual void do_clear( void ) override;
	virtual void do_reset( void ) override;
	virtual int long do_write( void const*, int long ) override;
	virtual int long do_read( void*, int long ) override;
	virtual int long do_read_some( void*, int long ) override;
	virtual void do_unread( void const*, int long ) override;
	virtual void do_seek( int long, SEEK ) override;
	virtual void do_flush( void ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
private:
	HSynchronizedStream( HSynchronizedStream const& );
	HSynchronizedStream& operator = ( HSynchronizedStream const& );
};

typedef HExceptionT<HSynchronizedStream, HStreamInterfaceException> HSynchronizedStreamException;


template<typename call_t>
HSynchronizedStream& HSynchronizedStream::operator()( call_t call_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef ) {
		call_( _streamRef );
	}
	return ( *this );
	M_EPILOG
}

}

}

#endif /* #ifndef YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED */

