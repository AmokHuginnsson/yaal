/*
---         `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hprogramoptionshandler.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hprogramoptionshandler.hxx
 *
 * \brief Infrastructure for handling program options.
 * This file contains HProgramOptionsHandler class.
 * Declaration of OOption structure.
 * Declaration of command line hadling routines.
 */

#ifndef YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED
#define YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "hcore/harray.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hpair.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Handle program options from command-line and configuration (resource) file.
 */
class HProgramOptionsHandler
	{
	typedef HProgramOptionsHandler self_t;
public:
	typedef bool ( *RC_CALLBACK_t )( HString&, HString& );
	typedef void ( *param_callback_t )( void* );
	typedef yaal::hcore::HPair<param_callback_t, void*> simple_callback_t; 
	class HOptionValueInterface
		{
		typedef HOptionValueInterface self_t;
	public:
		typedef HPointer<HOptionValueInterface> ptr_t;
		virtual ~HOptionValueInterface( void ) {}
		void set( HString const& );
		type_t get_type( void ) const;
		template<typename tType>
		tType const& get( void ) const;
	protected:
		virtual void do_set( HString const& ) = 0;
		virtual type_t do_get_type( void ) const = 0;
		virtual void const* do_get( void ) const = 0;
		};
	/*! \brief Basic program configuration item.
	 */
	struct OOption
		{
		struct TYPE
			{
			typedef enum
				{
				D_NONE,
				D_OPTIONAL,
				D_REQUIRED
				} enum_t;
			};
		char const* f_pcName;
		HOptionValueInterface::ptr_t f_oValue;
		char const* f_pcShortForm;
		TYPE::enum_t f_eSwitchType;
		char const* f_pcArgument;
		char const* f_pcDescription;
		simple_callback_t CALLBACK;
		OOption( void );
		OOption(
				char const*, HOptionValueInterface::ptr_t,
				char const*, TYPE::enum_t,
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
	options_t f_oOptions;
public:
	/*! \brief Trivial default constructor.
	 */
	HProgramOptionsHandler( void ) : f_oOptions() {}
	/*! \brief Add new option descriptor to option handler.
	 *
	 * \param name - option name.
	 */
	HProgramOptionsHandler& operator()( char const* name, HOptionValueInterface::ptr_t, char const*, OOption::TYPE::enum_t const&, char const*, char const*, simple_callback_t const& = simple_callback_t( NULL, NULL ) );
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
	int process_command_line( int const& argc, char* const* const argv, int* const unknown = NULL );
	/*! \brief Reading configuration files.
	 *
	 * process_rc_file gives easy to use API for reading configuration files
	 * that are common in Unix world.
	 */
	int process_rc_file( HString const&, HString const&, RC_CALLBACK_t = NULL );
	options_t const& get_options( void ) const
		{ return ( f_oOptions ); }
private:
	void set_option( OOption&, HString const& );
	};

typedef HExceptionT<HProgramOptionsHandler> HProgramOptionsHandlerException;

/*! \brief Helper functions for program options handling infrastructure.
 */
namespace program_options_helper
{

/*! \brief Set variable value based on string.
 */
template<typename tType>
void set_option_value_from_string( tType& object, HString const& value )
	{
	M_PROLOG
	object = lexical_cast<tType>( value );
	return;
	M_EPILOG
	}

}

template<typename tType>
class HProgramOptionsHandler::HOptionValue : public HProgramOptionsHandler::HOptionValueInterface, private HNonCopyable
	{
	tType& _instance;
	typedef HProgramOptionsHandler::HOptionValue<tType> self_t;
protected:
	typedef HProgramOptionsHandler::HOptionValueInterface hier_t;
public:
	HOptionValue( tType& instance ) : _instance( instance ) {}
protected:
	virtual void do_set( HString const& value )
		{
		M_PROLOG
		using yaal::hcore::program_options_helper::set_option_value_from_string;
		set_option_value_from_string( _instance, value );
		return;
		M_EPILOG
		}
	virtual type_t do_get_type( void ) const
		{
		return ( TYPE::symbolic<tType>::type );
		}
	virtual void const* do_get( void ) const
		{
		return ( &_instance );
		}
	};

template<typename tType>
tType const& HProgramOptionsHandler::HOptionValueInterface::get( void ) const
	{
	M_PROLOG
	return ( *static_cast<tType const*>( do_get() ) );
	M_EPILOG
	}

namespace program_options_helper
{

/*! \brief Type safe option container.
 */
template<typename tType>
HProgramOptionsHandler::HOptionValueInterface::ptr_t option_value( tType& instance )
	{
	HProgramOptionsHandler::HOptionValueInterface::ptr_t value( new HProgramOptionsHandler::HOptionValue<tType>( instance ) );
	return ( value );
	}

static HProgramOptionsHandler::HOptionValueInterface::ptr_t no_value;

inline yaal::hcore::HProgramOptionsHandler::simple_callback_t callback( yaal::hcore::HProgramOptionsHandler::param_callback_t func, void* param )
	{
	return ( HProgramOptionsHandler::simple_callback_t( func, param ) );
	}

int reload_configuration( void );

}

}

inline void swap( yaal::hcore::HProgramOptionsHandler::OOption& a, yaal::hcore::HProgramOptionsHandler::OOption& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_PROGRAMOPTIONSHANDLER_HXX_INCLUDED */

