/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCONSOLE_HHANDLER_H
#define __HCONSOLE_HHANDLER_H

#include <sys/types.h>

#include "./hcore/hmap.h"

#define M_REGISTER_PREPROCESS_HANDLER( count, tab, handler ) \
	register_preprocess_handler ( count, tab, ( HANDLER_t ) & handler )
#define M_REGISTER_POSTPROCESS_HANDLER( count, tab, handler ) \
	register_postprocess_handler ( count, tab, ( HANDLER_t ) & handler )

class HHandler
	{
protected:
	/*{*/
	typedef int ( HHandler::* HANDLER_t ) ( int, void * = NULL );
	typedef HMap < int, HANDLER_t > process_handler_key_map_t;
	typedef HMap < HString, HANDLER_t > process_handler_command_map_t;
	process_handler_key_map_t f_oPreprocessHandlers;
	process_handler_key_map_t f_oPostprocessHandlers;
	process_handler_command_map_t f_oCommandHandlers;
	HString f_oCommand;
	/*}*/
public:
	/*{*/
	HHandler ( size_t = 32, size_t = 32 );
	virtual ~HHandler ( void );
	/*}*/
protected:
	/*{*/
	int process_input ( int, process_handler_key_map_t & );
	HString process_command ( void );
	int register_preprocess_handler ( int, int *, HANDLER_t );
	int register_postprocess_handler ( int, int *, HANDLER_t );
	/*}*/
	};

#endif /* not __HCONSOLE_HHANDLER_H */
