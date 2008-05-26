/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HFILE_H
#define __YAAL_HCORE_HFILE_H

#include "hcore/hstring.h"
#include "hcore/hstreaminterface.h"

namespace yaal
{

namespace hcore
{

class HFile : public HStreamInterface
	{
public:
	typedef enum
		{
		D_READING = 1,
		D_WRITING = 2,
		D_APPEND = 4,
		D_TRUNCATE = 8
		} mode_open_t;
	typedef enum
		{
		D_DEFAULTS = 0, /* D_KEEP_NEWLINES | D_BUFFERED_READS */
		D_KEEP_NEWLINES = 1,
		D_STRIP_NEWLINES = 2,
		D_BUFFERED_READS = 4,
		D_UNBUFFERED_READS = 8
		} mode_read_t;
private:
	mode_open_t f_eMode;
	void* f_pvHandle;
	HString f_oPath;
	HString f_oError;
	bool f_bExternal;
public:
	HFile( mode_open_t const = D_READING, void* const = NULL );
	virtual ~HFile ( void );
	int open( char const* const );
	int close( void );
	int long read_line( HString&, mode_read_t = D_DEFAULTS, int const = 0 );
	HString const& get_path( void ) const;
	HString const& get_error( void ) const;
	virtual void flush( void ) const;
	bool operator ! ( void ) const;
private:
	virtual int long do_write( void const* const, int long const& );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long const& );
	int long get_line_length( void );
	HFile( HFile const& );
	HFile& operator = ( HFile const& );
	};

}

}

#endif /* not __YAAL_HCORE_HFILE_H */
