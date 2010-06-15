/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hzipstream.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/*! \file hcore/hzipstream.hxx
 * \brief Synchronize access to any stream.
 *
 * Declaration of HZipStreamBase class.
 */

#ifndef YAAL_TOOLS_HZIPSTREAM_HXX_INCLUDED
#define YAAL_TOOLS_HZIPSTREAM_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

class HZipStream : public yaal::hcore::HStreamInterface
	{
public:
	typedef HZipStream this_type;
	typedef yaal::hcore::HStreamInterface base_type;
	typedef yaal::hcore::HStreamInterface::ptr_t owned_stream_t;
	typedef yaal::hcore::HStreamInterface& ref_stream_t;
	struct MODE
		{
		typedef enum
			{
			DEFLATE,
			INFLATE
			} mode_t;
		};
private:
	MODE::mode_t _mode;
	int _error;
	owned_stream_t _streamOwned;
	yaal::hcore::HStreamInterface* _streamRef;
	yaal::hcore::HChunk _zStream;
	yaal::hcore::HChunk _zBufferIn;
	yaal::hcore::HChunk _zBufferOut;
	int long _offset;
public:
	HZipStream( MODE::mode_t );
	virtual ~HZipStream( void );
	explicit HZipStream( owned_stream_t, MODE::mode_t );
	explicit HZipStream( ref_stream_t, MODE::mode_t );
	template<typename call_t>
	HZipStream& operator()( call_t );
	void reset( owned_stream_t = owned_stream_t() );
	void reset( ref_stream_t );
	char const* error_message( int );
private:
	void init( void );
	void cleanup( void );
	int long prepare_data( void );
	virtual int long do_write( void const* const, int long const& );
	virtual int long do_read( void* const, int long const& );
	virtual void do_flush( void ) const;
	virtual bool do_is_valid( void ) const;
	HZipStream( HZipStream const& );
	HZipStream& operator = ( HZipStream const& );
	};

typedef yaal::hcore::HExceptionT<HZipStream, yaal::hcore::HStreamInterfaceException> HZipStreamException;

}

}

#endif /* not YAAL_TOOLS_HZIPSTREAM_HXX_INCLUDED */

