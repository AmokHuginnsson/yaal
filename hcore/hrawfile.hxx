/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hrawfile.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hrawfile.hxx
 * \brief Declaration of HRawFile class.
 */

#ifndef YAAL_HCORE_HRAWFILE_HXX_INCLUDED
#define YAAL_HCORE_HRAWFILE_HXX_INCLUDED 1

#include "hcore/hopenssl.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace hcore {

typedef int file_descriptor_t;

/*! \brief Raw low level IO operations abstraction.
 */
class HRawFile : public HStreamInterface {
public:
	typedef HRawFile this_type;
	typedef HStreamInterface base_type;
	/*! \brief Type of underneath file descriptor.
	 */
	struct TYPE {
		/*! \brief Fd type flags.
		 */
		typedef enum {
			DEFAULT     = 0, /*!< means plain */
			PLAIN       = 1, /*!< fd represents plain raw IO */
			SSL_SERVER  = 2, /*!< fd represents SSL server part of IO */
			SSL_CLIENT  = 4  /*!< fd represents SSL client part of IO */
		} raw_file_type_t;
	};
	/*! \brief HRawFile event type that HRawFile user can wait for.
	 */
	struct ACTION {
		typedef enum {
			READ = 1,
			WRITE = 2,
			BOTH = READ | WRITE
		} action_t;
	};
	/*! \brief Ownership.
	 */
	enum class OWNERSHIP {
		NONE,
		ACQUIRED,
		EXTERNAL
	};
protected:
	typedef int long ( HRawFile::* READER_t )( void*, int long );
	typedef int long ( HRawFile::* WRITER_t )( void const*, int long );
	typedef int ( HRawFile::* CLOSER_t )( void );
	TYPE::raw_file_type_t _type;
	file_descriptor_t _fileDescriptor; /* raw file descriptor of the file */
	int long _timeout;
	HOpenSSL::ptr_t _ssl;
	READER_t reader;
	WRITER_t writer;
	CLOSER_t closer;
	OWNERSHIP _ownership;
public:
	HRawFile( file_descriptor_t, OWNERSHIP ownership );
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
	int long read_plain( void*, int long );
	int long write_plain( void const*, int long );
	int long read_ssl_loader( void*, int long );
	int long write_ssl_loader( void const*, int long );
	int long read_ssl( void*, int long );
	int long write_ssl( void const*, int long );
	virtual int long do_write( void const*, int long ) override;
	virtual int long do_read( void*, int long ) override;
	virtual void do_flush( void ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
	bool wait_for( ACTION::action_t const&, int long* );
};

typedef HExceptionT<HRawFile, HStreamInterfaceException> HRawFileException;

static int const LOW_TIMEOUT_WARNING = 100; /* miliseconds */
extern M_YAAL_HCORE_PUBLIC_API int long _writeTimeout_;

}

}

#endif /* #ifndef YAAL_HCORE_HRAWFILE_HXX_INCLUDED */

