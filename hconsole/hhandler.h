/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.h - this file is integral part of `yaal' project.

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

#include "hcore/hhashmap.h"

namespace yaal
{

namespace hconsole
{

class HHandler
	{
protected:
	/*{*/
	typedef int ( HHandler::* HANDLER_t ) ( int, void * );
	typedef hcore::HHashMap < int, HANDLER_t > process_handler_key_map_t;
	typedef hcore::HHashMap < hcore::HString, HANDLER_t > process_handler_command_map_t;
	process_handler_key_map_t f_oPreprocessHandlers;
	process_handler_key_map_t f_oPostprocessHandlers;
	process_handler_command_map_t f_oCommandHandlers;
	hcore::HString f_oCommand;
	/*}*/
public:
	/*{*/
	HHandler ( size_t = 32, size_t = 32 );
	virtual ~HHandler ( void );
	/*}*/
protected:
	/*{*/
	int process_input_with_handlers ( int, const process_handler_key_map_t & );
	hcore::HString process_command ( void );
	template < typename tType >
	int register_preprocess_handler ( int a_iCount, int const * a_piTab, tType HANDLER )
		{
		return ( register_preprocess_handler_internal ( a_iCount, a_piTab, static_cast < HANDLER_t > ( HANDLER ) ) );
		}
	template < typename tType >
	int register_postprocess_handler ( int a_iCount, int const * a_piTab, tType HANDLER )
		{
		return ( register_postprocess_handler_internal ( a_iCount, a_piTab, static_cast < HANDLER_t > ( HANDLER ) ) );
		}
	int register_preprocess_handler_internal ( int, int const *, HANDLER_t );
	int register_postprocess_handler_internal ( int, int const *, HANDLER_t );
	/*}*/
	};

}

}

#endif /* not __HCONSOLE_HHANDLER_H */
