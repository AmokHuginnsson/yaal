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
	HMutex _mutex;
private:
	owned_stream_t _streamOwned;
	yaal::hcore::HStreamInterface* _streamRef;
public:
	HSynchronizedStream( void );
	explicit HSynchronizedStream( owned_stream_t );
	explicit HSynchronizedStream( ref_stream_t );
	void reset( owned_stream_t = owned_stream_t() );
	void reset( ref_stream_t );
	template<typename call_t>
	HSynchronizedStream& operator()( call_t );
protected:
	virtual HStreamInterface& do_output( HString const& );
	virtual HStreamInterface& do_output( char const* );
	virtual HStreamInterface& do_output( bool );
	virtual HStreamInterface& do_output( char );
	virtual HStreamInterface& do_output( char unsigned );
	virtual HStreamInterface& do_output( int short );
	virtual HStreamInterface& do_output( int short unsigned );
	virtual HStreamInterface& do_output( int );
	virtual HStreamInterface& do_output( int unsigned );
	virtual HStreamInterface& do_output( int long );
	virtual HStreamInterface& do_output( int long unsigned );
	virtual HStreamInterface& do_output( int long long );
	virtual HStreamInterface& do_output( int long long unsigned );
	virtual HStreamInterface& do_output( double );
	virtual HStreamInterface& do_output( double long );
	virtual HStreamInterface& do_output( float );
	virtual HStreamInterface& do_output( void const* );
	virtual HStreamInterface& do_output( manipulator_t const& );
	virtual HStreamInterface& do_output( HManipulator const& );
	virtual HStreamInterface& do_input( HString& );
	virtual HStreamInterface& do_input( bool& );
	virtual HStreamInterface& do_input( char& );
	virtual HStreamInterface& do_input( char unsigned& );
	virtual HStreamInterface& do_input( int short& );
	virtual HStreamInterface& do_input( int short unsigned& );
	virtual HStreamInterface& do_input( int& );
	virtual HStreamInterface& do_input( int unsigned& );
	virtual HStreamInterface& do_input( int long& );
	virtual HStreamInterface& do_input( int long unsigned& );
	virtual HStreamInterface& do_input( int long long& );
	virtual HStreamInterface& do_input( int long long unsigned& );
	virtual HStreamInterface& do_input( double& );
	virtual HStreamInterface& do_input( double long& );
	virtual HStreamInterface& do_input( float& );
	virtual HStreamInterface& do_input( void const*& );
	virtual HStreamInterface& do_input( manipulator_t const& );
	virtual int long do_read_until( yaal::hcore::HString&, char const* const, bool );
	virtual int long do_read_until_n( yaal::hcore::HString&, int long, char const* const, bool );
	virtual int long do_read_while( yaal::hcore::HString&, char const* const, bool );
	virtual int long do_read_while_n( yaal::hcore::HString&, int long, char const* const, bool );
	virtual int do_peek( void );
	virtual HStreamInterface& do_set_fill( int );
	virtual HStreamInterface& do_set_width( int );
	virtual HStreamInterface& do_set_precision( int );
	virtual HStreamInterface& do_set_base( BASES::enum_t );
	virtual HStreamInterface& do_set_float_format( FLOAT_FORMAT::enum_t );
	virtual HStreamInterface& do_set_skipws( bool );
	virtual HStreamInterface& do_set_boolalpha( bool );
	virtual int long do_write( void const* const, int long );
	virtual int long do_read( void* const, int long );
	virtual void do_flush( void ) const;
	virtual bool do_is_valid( void ) const;
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

