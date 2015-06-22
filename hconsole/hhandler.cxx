/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hhandler.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhandler.hxx"
#include "hcore/htokenizer.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HHandler::HHandler( int keyHandlers_, int commandHandlers_ )
	: _preprocessHandlers( keyHandlers_ ),
		_postprocessHandlers( keyHandlers_ ),
		_commandHandlers( commandHandlers_ ),
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

int HHandler::register_preprocess_handler( int codeCount_, int const* codes_,
		HANDLER_t HANDLER ) {
	M_PROLOG
	if ( codes_ ) {
		for ( int ctr( 0 ); ctr < codeCount_; ++ ctr )
			_preprocessHandlers[ codes_ [ ctr ] ] = HANDLER;
	} else
		_preprocessHandlers[ codeCount_ ] = HANDLER;
	return ( 0 );
	M_EPILOG
}

int HHandler::register_postprocess_handler( int codeCount_, int const* codes_,
		HANDLER_t HANDLER ) {
	M_PROLOG
	if ( codes_ ) {
		for ( int ctr( 0 ); ctr < codeCount_; ++ ctr )
			_postprocessHandlers[ codes_[ ctr ] ] = HANDLER;
	} else
		_postprocessHandlers[ codeCount_ ] = HANDLER;
	return ( 0 );
	M_EPILOG
}

void HHandler::register_command_handler( yaal::hcore::HString const& command_, HANDLER_t handler_ ) {
	M_PROLOG
	_commandHandlers[command_] = handler_;
	return;
	M_EPILOG
}

bool HHandler::process_input_with_handlers( HKeyPressEvent const& keyPress_, process_handler_key_map_t const& map_ ) {
	M_PROLOG
	process_handler_key_map_t::const_iterator it( map_.find( keyPress_.get_key_code() ) );
	bool consumed( false );
	if ( it != map_.end() ) {
		consumed = it->second( keyPress_ );
	}
	return ( consumed );
	M_EPILOG
}

HString HHandler::process_command( void ) {
	M_PROLOG
	HString command;
	if ( ! _command.is_empty() ) {
		command = HTokenizer( _command, " \t", HTokenizer::DELIMITED_BY_ANY_OF )[ 0 ];
		if ( process_command( command ) ) {
			_command = "";
		}
	}
	return ( _command );
	M_EPILOG
}

bool HHandler::process_command( yaal::hcore::HString const& command_ ) {
	M_PROLOG
	bool found( false );
	process_handler_command_map_t::iterator it( _commandHandlers.find( command_ ) );
	if ( it != _commandHandlers.end() ) {
		found = true;
		it->second( HCommandEvent( command_ ) );
	}
	return ( found );
	M_EPILOG
}

void HHandler::execute_command( yaal::hcore::HString const& command_ ) {
	M_PROLOG
	process_command( command_ );
	M_EPILOG
}

}

}

