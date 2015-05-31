/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/hsynchronizedstream.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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
	void reset( owned_stream_t = owned_stream_t() );
	void reset( ref_stream_t );
	external_lock_t acquire( void );
	template<typename call_t>
	HSynchronizedStream& operator()( call_t );
protected:
	virtual HStreamInterface& do_output( HString const& ) override;
	virtual HStreamInterface& do_output( char const* ) override;
	virtual HStreamInterface& do_output( bool ) override;
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
	virtual int long do_read_until( yaal::hcore::HString&, char const* const, bool ) override;
	virtual int long do_read_until_n( yaal::hcore::HString&, int long, char const* const, bool ) override;
	virtual int long do_read_while( yaal::hcore::HString&, char const* const, bool ) override;
	virtual int long do_read_while_n( yaal::hcore::HString&, int long, char const* const, bool ) override;
	virtual int do_peek( void ) override;
	virtual HStreamInterface& do_set_fill( int ) override;
	virtual HStreamInterface& do_set_width( int ) override;
	virtual HStreamInterface& do_set_precision( int ) override;
	virtual HStreamInterface& do_set_base( BASES ) override;
	virtual HStreamInterface& do_set_float_format( FLOAT_FORMAT ) override;
	virtual HStreamInterface& do_set_adjust( ADJUST ) override;
	virtual HStreamInterface& do_set_skipws( bool ) override;
	virtual HStreamInterface& do_set_boolalpha( bool ) override;
	virtual bool do_get_skipws( void ) const override;
	virtual bool do_get_boolalpha( void ) const override;
	virtual int do_get_fill( void ) const override;
	virtual int do_get_width( void ) const override;
	virtual int do_get_precision( void ) const override;
	virtual BASES do_get_base( void ) const override;
	virtual FLOAT_FORMAT do_get_float_format( void ) const override;
	virtual ADJUST do_get_adjust( void ) const override;
	virtual bool do_good( void ) const override;
	virtual int long do_write( void const* const, int long ) override;
	virtual int long do_read( void* const, int long ) override;
	virtual void do_flush( void ) override;
	virtual bool do_is_valid( void ) const override;
private:
	HSynchronizedStream( HSynchronizedStream const& );
	HSynchronizedStream& operator = ( HSynchronizedStream const& );
};

typedef HExceptionT<HSynchronizedStream, HStreamInterfaceException> HSynchronizedStreamException;


template<typename call_t>
HSynchronizedStream& HSynchronizedStream::operator()( call_t call_ ) {
	M_PROLOG
	HLock l( _mutex );
	if ( _streamRef )
		call_( _streamRef );
	return ( *this );
	M_EPILOG
}

}

}

#endif /* #ifndef YAAL_HCORE_HSYNCHRONIZEDSTREAM_HXX_INCLUDED */

