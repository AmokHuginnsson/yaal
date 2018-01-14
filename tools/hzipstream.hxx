/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hzipstream.hxx
 * \brief Synchronize access to any stream.
 *
 * Declaration of HZipStreamBase class.
 */

#ifndef YAAL_TOOLS_HZIPSTREAM_HXX_INCLUDED
#define YAAL_TOOLS_HZIPSTREAM_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

class HZipStream : public yaal::hcore::HStreamInterface {
public:
	typedef HZipStream this_type;
	typedef yaal::hcore::HStreamInterface base_type;
	typedef yaal::hcore::HStreamInterface::ptr_t owned_stream_t;
	typedef yaal::hcore::HStreamInterface& ref_stream_t;
	enum class MODE {
		DEFLATE,
		INFLATE
	};
private:
	MODE _mode;
	int _error;
	owned_stream_t _streamOwned;
	yaal::hcore::HStreamInterface* _streamRef;
	yaal::hcore::HChunk _zStream;
	yaal::hcore::HChunk _zBufferIn;
	yaal::hcore::HChunk _zBufferOut;
	int long _offset;
	bool _flushed;
public:
	HZipStream( MODE );
	virtual ~HZipStream( void );
	explicit HZipStream( owned_stream_t, MODE );
	explicit HZipStream( ref_stream_t, MODE );
	template<typename call_t>
	HZipStream& operator()( call_t );
	void reset( owned_stream_t = owned_stream_t() );
	void reset( ref_stream_t );
	char const* error_message( int );
private:
	void init( void );
	void cleanup( void );
	int long prepare_data( void );
	virtual int long do_write( void const*, int long ) override;
	virtual int long do_read( void*, int long ) override;
	virtual void do_flush( void ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
	HZipStream( HZipStream const& );
	HZipStream& operator = ( HZipStream const& );
};

typedef yaal::hcore::HExceptionT<HZipStream, yaal::hcore::HStreamInterfaceException> HZipStreamException;

}

}

#endif /* #ifndef YAAL_TOOLS_HZIPSTREAM_HXX_INCLUDED */

