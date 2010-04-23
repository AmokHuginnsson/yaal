/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hsynchronizedstream.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hsynchronizedstream.hxx
 * \brief Synchronize access to any stream.
 *
 * Declaration of HSynchronizedStreamBase class.
 */

#ifndef YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED
#define YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hthread.hxx"
#include "hcore/trait.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief HSynchronizedStreamBase makes use of HStreamInterface derivates thread safe.
 */
class HSynchronizedStreamBase : public HStreamInterface
	{
protected:
	typedef HSynchronizedStreamBase self_t;
	typedef HStreamInterface hier_t;
	HMutex _mutex;
public:
	HSynchronizedStreamBase( void );
protected:
	virtual HStreamInterface& do_output( HString const& );
	virtual HStreamInterface& do_output( char const* const& );
	virtual HStreamInterface& do_output( char const& );
	virtual HStreamInterface& do_output( int short const& );
	virtual HStreamInterface& do_output( int short unsigned const& );
	virtual HStreamInterface& do_output( int const& );
	virtual HStreamInterface& do_output( int unsigned const& );
	virtual HStreamInterface& do_output( int long const& );
	virtual HStreamInterface& do_output( int long unsigned const& );
	virtual HStreamInterface& do_output( double const& );
	virtual HStreamInterface& do_output( double long const& );
	virtual HStreamInterface& do_output( float const& );
	virtual HStreamInterface& do_output( void const* const& );
	virtual HStreamInterface& do_output( manipulator_t const& );
	virtual HStreamInterface& do_output( HManipulator const& );
	virtual HStreamInterface& do_input( HString& );
	virtual HStreamInterface& do_input( char& );
	virtual HStreamInterface& do_input( int short& );
	virtual HStreamInterface& do_input( int short unsigned& );
	virtual HStreamInterface& do_input( int& );
	virtual HStreamInterface& do_input( int unsigned& );
	virtual HStreamInterface& do_input( int long& );
	virtual HStreamInterface& do_input( int long unsigned& );
	virtual HStreamInterface& do_input( double& );
	virtual HStreamInterface& do_input( double long& );
	virtual HStreamInterface& do_input( float& );
	virtual HStreamInterface& do_input( void const*& );
	virtual int long do_read_until( yaal::hcore::HString&, char const* const, bool );
	virtual int long do_read_until_n( yaal::hcore::HString&, int long const&, char const* const, bool );
	virtual HStreamInterface& do_set_fill( int );
	virtual HStreamInterface& do_set_width( int );
	virtual HStreamInterface& do_set_base( BASES::enum_t );
	};

typedef HExceptionT<HSynchronizedStreamBase, HStreamInterfaceException> HSynchronizedStreamBaseException;

namespace
{

template<typename T>
struct stream_ptr;
template<>
struct stream_ptr<HStreamInterface::ptr_t>
	{
	static HStreamInterface* get( HStreamInterface::ptr_t& p )
		{ return ( p.raw() ); }
	static HStreamInterface const* get( HStreamInterface::ptr_t const& p )
		{ return ( p.raw() ); }
	};
template<typename T>
struct stream_ptr
	{
	static typename trait::strip_reference<T>::type* get( typename trait::strip_reference<T>::type& p )
		{ return ( &p ); }
	};

}

template<typename stream_t = HStreamInterface::ptr_t>
class HSynchronizedStream : public HSynchronizedStreamBase
	{
protected:
	typedef HSynchronizedStream self_t;
	typedef HSynchronizedStreamBase hier_t;
private:
	stream_t _stream;
public:
	HSynchronizedStream( void );
	HSynchronizedStream( stream_t );
	template<typename call_t>
	HSynchronizedStream& operator()( call_t );
	void reset( stream_t = stream_t() );
private:
	virtual int long do_write( void const* const, int long const& );
	virtual int long do_read( void* const, int long const& );
	virtual void do_flush( void ) const;
	virtual bool do_is_valid( void ) const;
	};

template<typename stream_t>
HSynchronizedStream<stream_t>::HSynchronizedStream( void )
	: HSynchronizedStreamBase(), _stream()
	{
	return;
	}

template<typename stream_t>
HSynchronizedStream<stream_t>::HSynchronizedStream( stream_t stream_ )
	: HSynchronizedStreamBase(), _stream( stream_ )
	{
	return;
	}

template<typename stream_t>
void HSynchronizedStream<stream_t>::reset( stream_t stream_ )
	{
	M_PROLOG
	_stream = stream_;
	M_EPILOG
	}

template<typename stream_t>
int long HSynchronizedStream<stream_t>::do_write( void const* const buf_, int long const& size_ )
	{
	M_PROLOG
	HStreamInterface* stream( stream_ptr<stream_t>::get( _stream ) );
	return ( stream ? stream->write( buf_, size_ ) : 0 );
	M_EPILOG
	}

template<typename stream_t>
int long HSynchronizedStream<stream_t>::do_read( void* const buf_, int long const& size_ )
	{
	M_PROLOG
	HStreamInterface* stream( stream_ptr<stream_t>::get( _stream ) );
	return ( stream ? stream->read( buf_, size_ ) : 0 );
	M_EPILOG
	}

template<typename stream_t>
void HSynchronizedStream<stream_t>::do_flush( void ) const
	{
	M_PROLOG
	HStreamInterface const* stream( stream_ptr<stream_t>::get( _stream ) );
	if ( stream )
		stream->flush();
	return;
	M_EPILOG
	}

template<typename stream_t>
bool HSynchronizedStream<stream_t>::do_is_valid( void ) const
	{
	M_PROLOG
	HStreamInterface const* stream( stream_ptr<stream_t>::get( _stream ) );
	return ( stream ? stream->is_valid() : false );
	M_EPILOG
	}

template<typename stream_t>
template<typename call_t>
HSynchronizedStream<stream_t>& HSynchronizedStream<stream_t>::operator()( call_t call_ )
	{
	M_PROLOG
	HLock l( _mutex );
	HStreamInterface* stream( stream_ptr<stream_t>::get( _stream ) );
	if ( stream )
		call_( stream );
	return ( *this );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED */

