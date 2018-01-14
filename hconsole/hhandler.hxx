/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	void register_preprocess_handler( int, int const*, HANDLER_t );
	void register_postprocess_handler( int, int const*, HANDLER_t );
};

typedef yaal::hcore::HExceptionT<HHandler> HHandlerException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HHANDLER_HXX_INCLUDED */

