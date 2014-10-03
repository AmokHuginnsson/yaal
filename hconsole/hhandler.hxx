/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED
#define YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED 1

#include "hcore/hhashmap.hxx"
#include "hcore/hboundcall.hxx"
#include "hconsole/hevent.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Interface for callback based control flows.
 */
class HHandler {
public:
	typedef HHandler this_type;
protected:
	typedef yaal::hcore::HBoundCall<bool ( HEvent const& )> HANDLER_t;
	typedef hcore::HHashMap<int, HANDLER_t> process_handler_key_map_t;
	typedef hcore::HHashMap<hcore::HString, HANDLER_t> process_handler_command_map_t;
	process_handler_key_map_t _preprocessHandlers;
	process_handler_key_map_t _postprocessHandlers;
	process_handler_command_map_t _commandHandlers;
	hcore::HString _command;
public:
	HHandler( int = 32, int = 32 );
	virtual ~HHandler( void );
	void execute_command( yaal::hcore::HString const& );
	void register_command_handler( yaal::hcore::HString const&, HANDLER_t );
protected:
	bool process_input_with_handlers( HKeyPressEvent const&, const process_handler_key_map_t& );
	hcore::HString process_command( void );
	bool process_command( yaal::hcore::HString const& );
	int register_preprocess_handler( int, int const*, HANDLER_t );
	int register_postprocess_handler( int, int const*, HANDLER_t );
};

typedef yaal::hcore::HExceptionT<HHandler> HHandlerException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED */

