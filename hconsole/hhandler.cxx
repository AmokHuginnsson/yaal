/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hhandler.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hhandler.hxx"
#include "hcore/htokenizer.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HHandler::HHandler ( size_t keyHandlers_, size_t commandHandlers_ )
				: _preprocessHandlers ( keyHandlers_ ),
					_postprocessHandlers ( keyHandlers_ ),
					_commandHandlers ( commandHandlers_ ),
					_command() {
	M_PROLOG
	return;
	M_EPILOG
}

HHandler::~HHandler ( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

int HHandler::register_preprocess_handler_internal( int codeCount_, int const * codes_,
		HANDLER_t HANDLER ) {
	M_PROLOG
	if ( codes_ ) {
		for ( int ctr( 0 ); ctr < codeCount_; ctr ++ )
			_preprocessHandlers[ codes_ [ ctr ] ] = HANDLER;
	} else
		_preprocessHandlers[ codeCount_ ] = HANDLER;
	return ( 0 );
	M_EPILOG
}

int HHandler::register_postprocess_handler_internal( int codeCount_, int const * codes_,
		HANDLER_t HANDLER ) {
	M_PROLOG
	if ( codes_ ) {
		for ( int ctr( 0 ); ctr < codeCount_; ctr ++ )
			_postprocessHandlers[ codes_ [ ctr ] ] = HANDLER;
	} else
		_postprocessHandlers[ codeCount_ ] = HANDLER;
	return ( 0 );
	M_EPILOG
}

int HHandler::process_input_with_handlers( int code_, process_handler_key_map_t const& map_ ) {
	M_PROLOG
	process_handler_key_map_t::const_iterator it( map_.find( code_ ) );
	if ( it != map_.end() )
		code_ = ( this->*( it->second ) )( code_, NULL );
	return ( code_ );
	M_EPILOG
}

HString HHandler::process_command( void ) {
	M_PROLOG
	HString command;
	if ( ! _command.is_empty() ) {
		command = HTokenizer( _command, " \t" )[ 0 ];
		process_handler_command_map_t::iterator it( _commandHandlers.find( command ) );
		if ( it != _commandHandlers.end() ) {
			static_cast<void>( ( this->*( it->second ) )( 0, _command.raw() ) );
			_command = "";
		}
	}
	return ( _command );
	M_EPILOG
}

}

}

