/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hprogramoptionshandler.hxx
 *
 * \brief Infrastructure for handling program options.
 * This file contains HProgramOptionsHandler class.
 * Declaration of HOption structure.
 * Declaration of command line handling routines.
 */

#ifndef YAAL_HCORE_HPROGRAMOPTIONSHANDLER_HXX_INCLUDED
#define YAAL_HCORE_HPROGRAMOPTIONSHANDLER_HXX_INCLUDED 1

#include "hcore/hpointer.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hpair.hxx"
#include "hcore/reflection.hxx"
#include "hcore/memory.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/programoptionshelper.hxx"
#include "hcore/system.hxx"

namespace yaal {

namespace hcore {

class HProgramOptionsHandler;
typedef HExceptionT<HProgramOptionsHandler> HProgramOptionsHandlerException;

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
		/*! \brief Option argument ontological policy.
		 */
		enum class ARGUMENT {
			NONE,     /*!< No argument is expected. */
			OPTIONAL, /*!< Argument is optional but is present it must immediately follow an option (without separating whitespace). */
			REQUIRED  /*!< Argument is required, missing argument is an error unless there are more options following, then next option is interpreted as an argument. */
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
		bool _verbatim;
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
		bool verbatim( void ) const;
		HOption& verbatim( bool );
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
			_setter = yaal::hcore::call( static_cast<void (HOption::*)( T&, yaal::hcore::HString const& )>( &HOption::set ), this, yaal::ref( recipient_ ), _1 );
			_getter = yaal::hcore::call( static_cast<yaal::hcore::HString (HOption::*)( T const& )>( &HOption::get ), this, yaal::cref( recipient_ ) );
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
		void set( T& recipient_, yaal::hcore::HString const& value_ ) {
			M_PROLOG
			try {
				program_options_helper::set_option_value_from_string( recipient_, value_ );
			} catch ( HException const& e ) {
				throw HProgramOptionsHandlerException(
					"Option `"_ys
						.append( !_longForm.is_empty() ? "--" : "-" )
						.append( !_longForm.is_empty() ? _longForm : to_string( code_point_t( static_cast<code_point_t::value_type>( _shortForm ) ) ) )
						.append( "' got an invalid value: " )
						.append( e.what() )
				);
			}
			return;
			M_EPILOG
		}
		template<typename T>
		yaal::hcore::HString get( T const& value_ ) {
			HString s;
			try {
				s = program_options_helper::get_string_from_option_value( value_ );
			} catch ( HException const& e ) {
				throw HProgramOptionsHandlerException(
					"Option's `"_ys
						.append( !_longForm.is_empty() ? "--" : "-" )
						.append( !_longForm.is_empty() ? _longForm : to_string( code_point_t( static_cast<code_point_t::value_type>( _shortForm ) ) ) )
						.append( "' value is not printable: " )
						.append( e.what() )
				);
			}
			return ( s );
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
	system::argv_t process_command_line( system::argv_t&& argv, int* unknown = nullptr );
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
	system::argv_t process_command_line( system::argv_t&&, char**, int* );
	void verify_new_option( HOption& );
	void set_option( HOption&, HString const& );
	void set_from_env( void );
};

inline void swap( yaal::hcore::HProgramOptionsHandler::HOption& a, yaal::hcore::HProgramOptionsHandler::HOption& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HPROGRAMOPTIONSHANDLER_HXX_INCLUDED */

