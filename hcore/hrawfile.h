/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HRAWFILE_H
#define __YAAL_HCORE_HRAWFILE_H

#include "hcore/hopenssl.h"
#include "hcore/hstreaminterface.h"

namespace yaal
{

namespace hcore
{

typedef int file_descriptor_t;

class HRawFile : public HStreamInterface
	{
protected:
	typedef int ( HRawFile::* READER_t )( void* const, int const );
	typedef int ( HRawFile::* WRITER_t )( void const* const, int const );
	typedef int ( HRawFile::* CLOSER_t )( void );
	struct TYPE
		{
		typedef enum
			{
			D_DEFAULT			= 0, /* means plain */
			D_PLAIN				= 1,
			D_SSL_SERVER	= 2,
			D_SSL_CLIENT	= 4
			} raw_file_type_t;
		};
	TYPE::raw_file_type_t f_eType;
	file_descriptor_t f_iFileDescriptor; /* raw file descriptor of the file */
	HOpenSSL::ptr_t f_oSSL;
public:
	HRawFile( TYPE::raw_file_type_t = TYPE::D_DEFAULT );
	virtual ~HRawFile( void );
	int close( void );
	file_descriptor_t get_file_descriptor( void ) const;
	READER_t reader;
	WRITER_t writer;
	CLOSER_t closer;
protected:
	virtual int do_close( void );
	bool is_write_ready( void );
	int close_plain( void );
	int close_ssl( void );
	int read_plain( void* const, int const );
	int write_plain( void const* const, int const );
	int read_ssl_loader( void* const, int const );
	int write_ssl_loader( void const* const, int const );
	int read_ssl( void* const, int const );
	int write_ssl( void const* const, int const );
	virtual int do_write( void const* const, int const );
	virtual int do_read( void* const, int const );
	virtual void do_flush( void ) const;
	};

}

}

#endif /* not __YAAL_HCORE_HRAWFILE_H */

