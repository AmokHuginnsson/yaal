/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HFILE_H
#define __HCORE_HFILE_H

#include "hstring.h"

namespace stdhapi
{

namespace hcore
{

class HFile
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
protected:
	/*{*/
	mode_open_t f_eMode;
	void * f_pvHandle;
	HString f_oPath;
	HString f_oError;
	/*}*/
private:
	/*{*/
	bool f_bExternal;
	/*}*/
public:
	/*{*/
	HFile ( mode_open_t const = D_READING, void * const = NULL );
	virtual ~HFile ( void );
	int open ( char const * const );
	int close ( void );
	int read_line ( HString &, mode_read_t = D_DEFAULTS, int const = 0 );
	HString const & get_path ( void ) const;
	HString const & get_error ( void ) const;
	void flush ( void ) const;
	HFile & operator << ( char const * const );
	HFile & operator << ( char const );
	HFile & operator << ( int const );
	HFile & operator << ( int long const );
	HFile & operator << ( double const );
	HFile & operator << ( void const * const );
	HFile & operator << ( HFile & ( * const ) ( HFile & ) );
	operator bool const ( void ) const;
	int write ( void const * const, int );
	int read ( void * const, int );
	/*}*/
protected:
	/*{*/
	int get_line_length ( void );
	int scan_line ( HString &, int const );
	/*}*/
private:
	/*{*/
	HFile ( HFile const & );
	HFile & operator = ( HFile const & );
	/*}*/
	friend HFile & endl ( HFile & );
	friend HFile & flush ( HFile & );
	};

template struct enum_t < HFile::mode_open_t >;
template struct enum_t < HFile::mode_read_t >;

}

}

#endif /* not __HCORE_HFILE_H */
