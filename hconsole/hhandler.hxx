/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED
#define YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED 1

#include "hcore/hhashmap.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Interface for callback based control flows.
 */
class HHandler {
public:
	typedef HHandler this_type;
protected:
	typedef int ( HHandler::* HANDLER_t ) ( int, void const* );
	typedef hcore::HHashMap<int, HANDLER_t> process_handler_key_map_t;
	typedef hcore::HHashMap<hcore::HString, HANDLER_t> process_handler_command_map_t;
	process_handler_key_map_t _preprocessHandlers;
	process_handler_key_map_t _postprocessHandlers;
	process_handler_command_map_t _commandHandlers;
	hcore::HString _command;
public:
	HHandler( size_t = 32, size_t = 32 );
	virtual ~HHandler( void );
protected:
	int process_input_with_handlers( int, const process_handler_key_map_t& );
	hcore::HString process_command( void );
	template<typename tType>
	int register_preprocess_handler( int count_, int const* tab_, tType HANDLER ) {
		return ( register_preprocess_handler_internal( count_, tab_, static_cast<HANDLER_t>( HANDLER ) ) );
	}
	template<typename tType>
	int register_postprocess_handler( int count_, int const* tab_, tType HANDLER ) {
		return ( register_postprocess_handler_internal( count_, tab_, static_cast<HANDLER_t>( HANDLER ) ) );
	}
	int register_preprocess_handler_internal( int, int const *, HANDLER_t );
	int register_postprocess_handler_internal( int, int const *, HANDLER_t );
};

typedef yaal::hcore::HExceptionT<HHandler> HHandlerException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED */

