/*
---          libsource (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HLOG_HXX_INCLUDED
#define YAAL_HCORE_HLOG_HXX_INCLUDED

#include <cstdarg>

#include "hcore/hsingleton.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hfile.hxx"

namespace yaal
{

namespace hcore
{

#define log_trace ( yaal::hcore::log << "Log: " << __FILE__ << " : " << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ << " : " )

extern void* DEFAULT_LOG_STREAM;

/*! \brief Enumeration of available log levels.
 */
namespace LOG_TYPE
	{
	static int const DEBUG			= 1; 
	static int const INFO				= 2; 
	static int const NOTICE			= 4; 
	static int const WARNING		= 8;
	static int const ERROR			= 16; 
	static int const VCSHEADER	= 32;
	}

/*! \brief Logging utility.
 */
class HLog : public HField<HFile>, public HSynchronizedFile, private HSingletonInterface
	{
protected:
	typedef HLog self_t;
	typedef HStreamInterface hier_t;
private:
	typedef HField<HFile> _file;
	bool     _realMode;
	bool     _newLine;
	int long _type;
	size_t   _bufferSize;
	char*    _processName;
	HString  _loginName;
	HChunk   _hostName;
	HChunk   _buffer;
public:
	static int long _logMask;
public:
	/* already opened file */
	void rehash( void* = DEFAULT_LOG_STREAM, char const* const = NULL );
	/* log file name */
	void rehash( HString const&, char const* const = NULL );
	int operator()( char const* const, va_list );
	/*! \brief log ( "data %d", x ); will look nice
	 */
	int operator()( char const* const, ... );
	/*! \brief log ( "data %d", x ); will look nice
	 */
	int operator()( int long const, char const*, ... );
	/*! \brief sets log type
	 */
	HLog& operator()( int long const& );
	HLog& filter( int long const& );
private:
	HLog( void );
	virtual ~HLog( void );
	void do_rehash( void*, char const* const );
	void timestamp( void );
	virtual int long do_write( void const* const, int long const& );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long const& );
	virtual bool do_is_valid( void ) const;
	HLog( HLog const& );
	HLog& operator= ( HLog const& );
	friend class yaal::hcore::HSingleton<HLog>;
	friend class yaal::hcore::HDestructor<HLog>;
	};

typedef HExceptionT<HLog, HStreamInterfaceException> HLogException;
extern HLog& log;

}

}

#endif /* not YAAL_HCORE_HLOG_HXX_INCLUDED */

