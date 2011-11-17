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
 * Declaration of OOption structure.
 * Declaration of command line hadling routines.
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

namespace yaal {

namespace hcore {

/*! \brief Handle program options from command-line and configuration (resource) file.
 */
class HProgramOptionsHandler {
/* cppcheck-suppress variableHidingTypedef */
	typedef HProgramOptionsHandler this_type;
public:
	typedef bool ( *RC_CALLBACK_t )( HString&, HString& );
	typedef void ( *param_callback_t )( void* );
	typedef yaal::hcore::HPair<param_callback_t, void*> simple_callback_t;
	class HOptionValueInterface {
		typedef HOptionValueInterface this_type;
	public:
		typedef HPointer<HOptionValueInterface> ptr_t;
		virtual ~HOptionValueInterface( void ) {}
		void set( HString const& );
		type_id_t get_type( void ) const;
		template<typename tType>
		tType const& get( void ) const;
		void const* id( void ) const;
	protected:
		virtual void do_set( HString const& ) = 0;
		virtual type_id_t do_get_type( void ) const = 0;
		virtual void const* do_get( void ) const = 0;
	};
	/*! \brief Basic program configuration item.
	 */
	struct OOption {
		struct TYPE {
			typedef enum {
				NONE,
				OPTIONAL,
				REQUIRED
			} enum_t;
		};
		char const* _name;
		HOptionValueInterface::ptr_t _value;
		int _shortForm;
		TYPE::enum_t _switchType;
		char const* _description;
		char const* _argument;
		simple_callback_t CALLBACK;
		OOption( void );
		OOption(
				char const*, HOptionValueInterface::ptr_t,
				int, TYPE::enum_t,
				char const*, char const*,
				HProgramOptionsHandler::simple_callback_t const& );
		OOption( OOption const& );
		OOption& operator = ( OOption const& );
		void swap( OOption& );
	};
	template<typename tType>
	class HOptionValue;
	typedef HArray<OOption> options_t;
private:
	options_t _options;
public:
	/*! \brief Trivial default constructor.
	 */
	HProgramOptionsHandler( void ) : _options() {}
	/*! \brief Add new option descriptor to option handler.
	 *
	 * \param name - option name.
	 * \param value - value storage helper.
	 * \param shortForm - a short form of option name.
	 * \param argType - type of option with respect to requirements of addtional arguments.
	 * \param desc - option descrition for help generation.
	 * \param argName - name of addtional option argument for generated help message.
	 * \param callback - a function invoked when option is encountered.
	 */
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t value,
			int shortForm, OOption::TYPE::enum_t argType,
			char const* desc = NULL, char const* argName = NULL,
			simple_callback_t const& callback = simple_callback_t( NULL, NULL ) );
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t value,
			int shortForm, OOption::TYPE::enum_t argType,
			char const* desc,
			simple_callback_t const& callback );
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t value,
			OOption::TYPE::enum_t argType, char const* desc,
			simple_callback_t const& callback );
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t value,
			char const* shortForm, OOption::TYPE::enum_t argType,
			char const* desc = NULL, char const* argName = NULL,
			simple_callback_t const& callback = simple_callback_t( NULL, NULL ) );
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t value,
			char const* shortForm, OOption::TYPE::enum_t argType,
			char const* desc, simple_callback_t const& callback );
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t value,
			OOption::TYPE::enum_t argType,
			char const* desc = NULL, char const* argName = NULL,
			simple_callback_t const& callback = simple_callback_t( NULL, NULL ) );
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
	int process_command_line( int argc, char* const* const argv, int* const unknown = NULL );
	/*! \brief Reading configuration files.
	 *
	 * process_rc_file gives easy to use API for reading configuration files
	 * that are common in Unix world.
	 */
	int process_rc_file( HString const&, HString const&, RC_CALLBACK_t = NULL );
	options_t const& get_options( void ) const
		{ return ( _options ); }
private:
	void set_option( OOption&, HString const& );
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

template<typename tType>
class HProgramOptionsHandler::HOptionValue : public HProgramOptionsHandler::HOptionValueInterface, private trait::HNonCopyable {
	tType& _instance;
	typedef HProgramOptionsHandler::HOptionValue<tType> this_type;
protected:
	typedef HProgramOptionsHandler::HOptionValueInterface base_type;
public:
	HOptionValue( tType& instance ) : _instance( instance ) {}
protected:
	virtual void do_set( HString const& value ) {
		M_PROLOG
		using yaal::hcore::program_options_helper::set_option_value_from_string;
		set_option_value_from_string( _instance, value );
		return;
		M_EPILOG
	}
	virtual type_id_t do_get_type( void ) const {
		return ( TYPE::symbolic<tType>::value );
	}
	virtual void const* do_get( void ) const {
		return ( &_instance );
	}
};

template<typename tType>
tType const& HProgramOptionsHandler::HOptionValueInterface::get( void ) const {
	M_PROLOG
	return ( *static_cast<tType const*>( do_get() ) );
	M_EPILOG
}

namespace program_options_helper {

/*! \brief Type safe option container.
 */
template<typename tType>
HProgramOptionsHandler::HOptionValueInterface::ptr_t option_value( tType& instance ) {
	HProgramOptionsHandler::HOptionValueInterface::ptr_t value( make_pointer<HProgramOptionsHandler::HOptionValue<tType> >( ref( instance ) ) );
	return ( value );
}

static HProgramOptionsHandler::HOptionValueInterface::ptr_t no_value;

inline yaal::hcore::HProgramOptionsHandler::simple_callback_t callback( yaal::hcore::HProgramOptionsHandler::param_callback_t func, void* param ) {
	return ( HProgramOptionsHandler::simple_callback_t( func, param ) );
}

int reload_configuration( void );

}

}

inline void swap( yaal::hcore::HProgramOptionsHandler::OOption& a, yaal::hcore::HProgramOptionsHandler::OOption& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED */

