/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hserial.h - this file is integral part of `stdhapi' project.

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

#ifndef __TOOLS_HSERIAL_H
#define __TOOLS_HSERIAL_H

#ifndef NULL
#	define NULL 0
#endif /* not NULL */

#include "hcore/hstring.h"
#include "hcore/hrawfile.h"

namespace stdhapi
{

namespace tools
{

class HSerial : public hcore::HRawFile
	{
public:
	typedef enum
		{
		D_DEFAULT,
		D_TEXT,
		D_BINARY
		} mode_t;
	typedef enum
		{
		D_SPEED_DEFAULT,
		D_SPEED_B115200,
#ifdef HAVE_DECL_B7600
		D_SPEED_B76800,
#endif /* HAVE_DECL_B7600 */
		D_SPEED_B57600,
		D_SPEED_B38400,
#ifdef HAVE_DECL_B28800
		D_SPEED_B28800,
#endif /* HAVE_DECL_B28800 */
		D_SPEED_B19200,
#ifdef HAVE_DECL_B14400
		D_SPEED_B14400,
#endif /* HAVE_DECL_B14400 */
		D_SPEED_B9600,
#ifdef HAVE_DECL_B7200
		D_SPEED_B7200,
#endif /* HAVE_DECL_B7200 */
		D_SPEED_B4800,
		D_SPEED_B2400
		} speed_t;
protected:
	/*{*/
	hcore::HString f_oDevicePath;
	hcore::HString f_oTIO;
	hcore::HString f_oBackUpTIO;
	/*}*/
public:
	/*{*/
	HSerial ( char const * const = NULL, mode_t = D_DEFAULT ); /* device path, canonical (or not) */
	virtual ~HSerial ( void );
	bool open ( void );
	virtual int close ( void );
	void set_speed ( speed_t );
	void flush ( int );
	void wait_for_eot ( void );
	int read ( void * const, int const, int const, int const = 0 );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

}

}

#endif /* not __TOOLS_HSERIAL_H */

