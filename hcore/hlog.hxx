/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HLOG_HXX_INCLUDED
#define YAAL_HCORE_HLOG_HXX_INCLUDED 1

#include <cstdarg>

#include "hcore/hsingleton.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hfile.hxx"

namespace yaal {

namespace hcore {

#ifdef log_trace
#	error Yaal redefines log_trace macro.
#endif /* #ifdef log_trace */

#define log_trace ( yaal::hcore::log << "Log: " << __FILE__ << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ << " : " )

extern void* DEFAULT_LOG_STREAM;

/*! \brief Enumeration of available log levels.
 */
namespace LOG_TYPE {
	static int const DEBUG     = 1;
	static int const INFO      = 2;
	static int const NOTICE    = 4;
	static int const WARNING   = 8;
	static int const ERROR     = 16;
	static int const VCSHEADER = 32;
}

/*! \brief Logging utility.
 */
class HLog : public HField<HFile>, public HSynchronizedStream, public HSingleton<HLog> {
public:
	typedef HLog this_type;
	typedef HSynchronizedStream base_type;
private:
	typedef HField<HFile> _file;
	bool     _realMode;
	bool     _newLine;
	int long _type;
	size_t   _bufferSize;
	char const* _processName;
	HString  _loginName;
	HString  _hostName;
	HChunk   _buffer;
public:
	M_YAAL_HCORE_PUBLIC_API static int long _logMask;
public:
	/* already opened file */
	void rehash_stream( void* = DEFAULT_LOG_STREAM, char const* const = NULL );
	/* log file name */
	void rehash( HString const&, char const* const = NULL );
	/*! \brief log ( "data %d", x ); will look nice
	 */
	int operator()( char const* const, ... ) __attribute__(( format( printf, 2, 3 ) ));
	/*! \brief log ( "data %d", x ); will look nice
	 */
	int operator()( int long const, char const*, ... ) __attribute__(( format( printf, 3, 4 ) ));
	/*! \brief sets log type
	 */
	HLog& operator()( int long );
	HLog& filter( int long );
private:
	HLog( void );
	virtual ~HLog( void );
	void do_rehash( void*, char const* const );
	void timestamp( void );
	int vformat( char const* const, va_list ) __attribute__(( format( printf, 2, 0 ) ));
	void eol_reset( char const* const, int long );
	virtual int long do_write( void const* const, int long );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long );
	virtual bool do_is_valid( void ) const;
	HLog( HLog const& );
	HLog& operator= ( HLog const& );
	friend class yaal::hcore::HSingleton<HLog>;
	friend class yaal::hcore::HDestructor<HLog>;
};

typedef HExceptionT<HLog, HSynchronizedStreamException> HLogException;
extern M_YAAL_HCORE_PUBLIC_API HLog& log;

}

}

#endif /* #ifndef YAAL_HCORE_HLOG_HXX_INCLUDED */

