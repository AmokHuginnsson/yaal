/*
---         `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

  hprogramoptionshandler.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hprogramoptionshandler.hxx
 *
 * \brief Infrastructure for handling program options.
 * This file contains HProgramOptionsHandler class.
 * Declaration of HOption structure.
 * Declaration of command line handling routines.
 */

#ifndef YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED
#define YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/harray.hxx"
#include "hcore/hdeque.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hset.hxx"
#include "hcore/hhashset.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hpair.hxx"
#include "hcore/reflection.hxx"
#include "hcore/memory.hxx"
#include "hcore/hboundcall.hxx"

namespace yaal {

namespace hcore {

namespace program_options_helper {

template<typename tType>
void set_option_value_from_string( tType& object, yaal::hcore::HString const& value );

template<typename tType>
void set_option_value_from_string( yaal::hcore::HArray<tType>& object, yaal::hcore::HString const& value );

template<typename tType>
void set_option_value_from_string( yaal::hcore::HDeque<tType>& object, yaal::hcore::HString const& value );

template<typename tType>
void set_option_value_from_string( yaal::hcore::HList<tType>& object, yaal::hcore::HString const& value );

template<typename tType>
void set_option_value_from_string( yaal::hcore::HSet<tType>& object, yaal::hcore::HString const& value );

template<typename tType>
void set_option_value_from_string( yaal::hcore::HHashSet<tType>& object, yaal::hcore::HString const& value );

}

/*! \brief Handle program options from command-line and configuration (resource) file.
 */
class HProgramOptionsHandler {
/* cppcheck-suppress variableHidingTypedef */
	typedef HProgramOptionsHandler this_type;
public:
	typedef bool ( *RC_CALLBACK_t )( HString&, HString& );
	/*! \brief Basic program configuration item.
	 */
	class HOption {
	public:
		enum class ARGUMENT {
			NONE,
			OPTIONAL,
			REQUIRED
		};
		typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> setter_t;
		typedef yaal::hcore::HBoundCall<yaal::hcore::HString ( void )> getter_t;
	private:
		int _shortForm;
		yaal::hcore::HString _longForm;
		ARGUMENT _switchType;
		yaal::hcore::HString _description;
		yaal::hcore::HString _argument;
		yaal::hcore::HString _defaultValue;
		setter_t _setter;
		getter_t _getter;
		TYPE _recipientType;
		u64_t _valueId;
	public:
		HOption( void );
		yaal::hcore::HString const& long_form( void ) const;
		HOption& long_form( yaal::hcore::HString const& );
		int short_form( void ) const;
		HOption& short_form( int );
		u64_t value_id( void ) const;
		ARGUMENT switch_type( void ) const;
		HOption& switch_type( ARGUMENT );
		yaal::hcore::HString const& description( void ) const;
		HOption& description( yaal::hcore::HString const& );
		yaal::hcore::HString const& argument_name( void ) const;
		HOption& argument_name( yaal::hcore::HString const& );
		yaal::hcore::HString const& default_value( void ) const;
		HOption& default_value( yaal::hcore::HString const& );
		HOption& setter( setter_t const& );
		HOption& setter( setter_t::function_t const& setter_ ) {
			return ( setter( setter_t( setter_ ) ) );
		}
		HOption& getter( getter_t const& );
		HOption& getter( getter_t::function_t const& getter_ ) {
			return ( getter( getter_t( getter_ ) ) );
		}
		template<typename T>
		HOption& recipient( T& recipient_ ) {
			M_PROLOG
			_setter = yaal::hcore::call( static_cast<void (*)( T&, yaal::hcore::HString const& )>( &HOption::set ), yaal::ref( recipient_ ), _1 );
			_getter = yaal::hcore::call( static_cast<yaal::hcore::HString (*)( T const& )>( &HOption::get ), yaal::cref( recipient_ ) );
			_recipientType = yaal::symbolic_type<T>::value;
			_valueId = reinterpret_cast<u64_t>( &recipient_ );
			return ( *this );
			M_EPILOG
		}
		TYPE recipient_type( void ) const;
		bool used( void ) const;
		void set( yaal::hcore::HString const& );
		yaal::hcore::HString get( void ) const;
		HOption( HOption const& );
		HOption& operator = ( HOption const& );
		void swap( HOption& );
	private:
		template<typename T>
		static void set( T& recipient_, yaal::hcore::HString const& value_ ) {
			M_PROLOG
			program_options_helper::set_option_value_from_string( recipient_, value_ );
			return;
			M_EPILOG
		}
		template<typename T>
		static yaal::hcore::HString get( T const& value_ ) {
			return ( lexical_cast<yaal::hcore::HString>( value_ ) );
		}
	};
	template<typename tType>
	class HOptionValue;
	typedef HArray<HOption> options_t;
private:
	options_t _options;
	yaal::hcore::HString _projectName;
	yaal::hcore::HString _syscondDir;
public:
	/*! \brief Trivial default constructor.
	 *
	 * \param projectName_ - name of the project that this handler will provide support for.
	 * \param sysconfDir_ - path to directory where global system configuration files for this project are stored.
	 */
	HProgramOptionsHandler( yaal::hcore::HString const& projectName_ = PACKAGE_NAME, yaal::hcore::HString const& sysconfDir_ = SYSCONFDIR );
	HProgramOptionsHandler& operator()( HOption );
	/*! \brief Parse command line options and set program setup variables.
	 *
	 * process_command_line gives easy to use API for interpreting and handling
	 * command line switches that are common in Unix world.
	 *
	 * \param argc - argument count, number of arguments on command line.
	 * \param argv - table of arguments from command line.
	 * \param setup - array of supported program options descriptions.
	 * \param count - number of supported program options.
	 * \param[out] unknown - index of first unknown option in \e argv.
	 */
	int process_command_line( int argc, char** argv, int* unknown = nullptr );
	/*! \brief Reading configuration files.
	 *
	 * process_rc_file gives easy to use API for reading configuration files
	 * that are common in Unix world.
	 *
	 * \param section_ - name of the section of configuration file that should be parsed and applied.
	 */
	int process_rc_file( HString const& section_, RC_CALLBACK_t = nullptr );
	options_t const& get_options( void ) const
		{ return ( _options ); }
private:
	void verify_new_option( HOption& );
	void set_option( HOption&, HString const& );
	void set_from_env( void );
};

typedef HExceptionT<HProgramOptionsHandler> HProgramOptionsHandlerException;

/*! \brief Helper functions for program options handling infrastructure.
 */
namespace program_options_helper {

template<typename tType>
void set_option_value_from_string( HArray<tType>& object, HString const& value ) {
	M_PROLOG
	object.push_back( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( HDeque<tType>& object, HString const& value ) {
	M_PROLOG
	object.push_back( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( HList<tType>& object, HString const& value ) {
	M_PROLOG
	object.push_back( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( HSet<tType>& object, HString const& value ) {
	M_PROLOG
	object.insert( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( HHashSet<tType>& object, HString const& value ) {
	M_PROLOG
	object.insert( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

/*! \brief Set variable value based on string.
 */
template<typename tType>
void set_option_value_from_string( tType& object, HString const& value ) {
	M_PROLOG
	object = lexical_cast<tType>( value );
	return;
	M_EPILOG
}

}

namespace program_options_helper {

int reload_configuration( void );

}

inline void swap( yaal::hcore::HProgramOptionsHandler::HOption& a, yaal::hcore::HProgramOptionsHandler::HOption& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED */

