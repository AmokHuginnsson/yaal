/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdes.h - this file is integral part of `stdhapi' project.

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

#ifndef __HDES_H
#define __HDES_H

#define D_DES_SIDES_COUNT		2
#define D_DES_IKEY_SIZE			6
#define D_DES_BLOCK_SIZE		8
#define D_DES_IKEYS_COUNT		16
#define D_DES_PASSWORD_SIZE	16

typedef unsigned char uc_t;

class HDes
	{
protected:
	/*{*/
	uc_t f_pppcIKeys [ D_DES_SIDES_COUNT ]
		[ D_DES_IKEYS_COUNT ] [ D_DES_IKEY_SIZE ];
	/*}*/
public:
	/*{*/
	HDes ( void );
	virtual ~HDes ( void );
	void crypt ( uc_t *, int, int );
	void generate_keys ( uc_t * );
	void flush_keys ( void );
	/*}*/
protected:
	/*{*/
	void _des ( uc_t * /* block */, int /* side */, int /* part */ );
	void _3des ( uc_t * /* block */, int /* side */ );
	void permutate ( uc_t * /* buffer */, const uc_t * /* tab */, int /* len */ );
	/*}*/
	};

#endif /* not __HDES_H */

