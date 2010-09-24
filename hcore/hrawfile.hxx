/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HRAWFILE_HXX_INCLUDED
#define YAAL_HCORE_HRAWFILE_HXX_INCLUDED

#include "hcore/hopenssl.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace hcore
{

typedef int file_descriptor_t;

/*! \brief Raw low level IO operations abstraction.
 */
class HRawFile : public HStreamInterface
	{
protected:
	typedef HRawFile this_type;
	typedef HStreamInterface base_type;
	typedef int long ( HRawFile::* READER_t )( void* const, int long );
	typedef int long ( HRawFile::* WRITER_t )( void const* const, int long );
	typedef int ( HRawFile::* CLOSER_t )( void );
	/*! \brief Type of underneath file descriptor.
	 */
	struct TYPE
		{
		/*! \brief Fd type flags.
		 */
		typedef enum
			{
			DEFAULT			= 0, /*!< means plain */
			PLAIN				= 1, /*!< fd represents plain raw IO */
			SSL_SERVER	= 2, /*!< fd represents SSL server part of IO */
			SSL_CLIENT	= 4  /*!< fd represents SSL client part of IO */
			} raw_file_type_t;
		};
	/*! \brief HRawFile event type that HRawFile user can wait for.
	 */
	struct ACTION
		{
		typedef enum
			{
			READ,
			WRITE
			} action_t;
		};
	TYPE::raw_file_type_t _type;
	file_descriptor_t _fileDescriptor; /* raw file descriptor of the file */
	int long _timeout;
	HOpenSSL::ptr_t _sSL;
	READER_t reader;
	WRITER_t writer;
	CLOSER_t closer;
public:
	HRawFile( TYPE::raw_file_type_t = TYPE::DEFAULT );
	virtual ~HRawFile( void );
	int close( void );
	file_descriptor_t get_file_descriptor( void ) const;
	/*! \brief Set write timeout for this raw file.
	 *
	 * \param timeout - a new timeout value in miliseconds.
	 */
	void set_timeout( int long timeout );
protected:
	virtual int do_close( void );
	int close_plain( void );
	int close_ssl( void );
	int long read_plain( void* const, int long );
	int long write_plain( void const* const, int long );
	int long read_ssl_loader( void* const, int long );
	int long write_ssl_loader( void const* const, int long );
	int long read_ssl( void* const, int long );
	int long write_ssl( void const* const, int long );
	virtual int long do_write( void const* const, int long );
	virtual int long do_read( void* const, int long );
	virtual void do_flush( void ) const;
	virtual bool do_is_valid( void ) const;
	bool wait_for( ACTION::action_t const&, int long* );
	};

typedef HExceptionT<HRawFile, HStreamInterfaceException> HRawFileException;

static int const LOW_TIMEOUT_WARNING = 100; /* miliseconds */
extern int long _writeTimeout_;

}

}

#endif /* not YAAL_HCORE_HRAWFILE_HXX_INCLUDED */

