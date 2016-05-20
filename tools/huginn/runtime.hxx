/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  runtime.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRuntime {
private:
	typedef HHuginn::type_id_t type_id_t;
	typedef HHuginn::identifier_id_t identifier_id_t;
	typedef HHuginn::value_t value_t;
	typedef HHuginn::values_t values_t;
	typedef HHuginn::function_t function_t;
	typedef HHuginn::field_definitions_t field_definitions_t;
	typedef HHuginn::class_t class_t;
	typedef yaal::hcore::HPointer<huginn::HThread> thread_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HThread::id_t, thread_t> threads_t;
	typedef yaal::hcore::HLookupMap<yaal::hcore::HString, identifier_id_t> identifier_ids_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> identifier_names_t;
	typedef yaal::hcore::HResource<huginn::HObjectFactory> object_factory_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, class_t> classes_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, value_t> packages_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, function_t> functions_t;
public:
	typedef yaal::hcore::HBoundCall<HHuginn::class_t ( type_id_t )> class_constructor_t;
private:
	HHuginn* _huginn;
	HHuginn::type_id_t::value_type _idGenerator;
	identifier_ids_t _identifierIds;
	identifier_names_t _identifierNames;
	/*
	 * Built-in types can by used as field definitions in user classes.
	 * User class needs to be able to use built-in types in its destructor.
	 * Hence order of two following fields:
	 * _objectFactory and _classes
	 */
	object_factory_t _objectFactory;
	value_t _none;
	value_t _true;
	value_t _false;
	classes_t _classes;
	functions_t _functions;
	threads_t _threads;
	packages_t _packages;
	HHuginn::list_t _argv;
	value_t _result;
	int _maxLocalVariableCount;
public:
	HRuntime( HHuginn* );
	huginn::HThread* current_thread( void );
	huginn::HFrame* current_frame( void );
	HHuginn* huginn( void ) const {
		return ( _huginn );
	}
	value_t& none_value( void ) {
		return ( _none );
	}
	value_t& true_value( void ) {
		return ( _true );
	}
	value_t& false_value( void ) {
		return ( _false );
	}
	huginn::HObjectFactory* object_factory( void ) {
		return ( _objectFactory.raw() );
	}
	void set_max_local_variable_count( int );
	int max_local_variable_count() const {
		return ( _maxLocalVariableCount );
	}
	identifier_id_t identifier_id( yaal::hcore::HString const& );
	yaal::hcore::HString const& identifier_name( identifier_id_t ) const;
	function_t* get_function( identifier_id_t );
	class_t get_class( identifier_id_t );
	value_t* get_package( identifier_id_t );
	void register_class_low( class_t );
	void register_function( identifier_id_t, function_t );
	void register_package( identifier_id_t, value_t );
	class_t create_class( identifier_id_t, HHuginn::HClass const*, field_definitions_t const& );
	class_t create_class( yaal::hcore::HString const&, HHuginn::HClass const*, field_definitions_t const& );
	class_t create_class( class_constructor_t const& );
	void execute( void );

	/*! \brief Call (execute) Huginn function.
	 *
	 * \param name_ - name of the function to call.
	 * \param argv_ - array of arguments that shall be passed to function call.
	 * \param position_ - call context information for error reporting.
	 * \return Result returned by called Huginn function.
	 */
	value_t call( yaal::hcore::HString const& name_, values_t const& argv_, int position_ );

	/*! \brief Get value returned by program's main().
	 *
	 * \return Value returned by program's main() function.
	 */
	value_t result( void ) const;

	/*! \brief Add argument for main() function.
	 *
	 * \param value_ - value of argument being added.
	 */
	void add_argument( yaal::hcore::HString const& value_ );

	/*! \brief Remove all argument currently held for main() function.
	 */
	void clear_arguments( void );

	/*! \brief Dump Huginn Virtual Machine state onto given stream.
	 *
	 * \param stream_ - stream where VM state shall be dumped.
	 */
	void dump_vm_state( yaal::hcore::HStreamInterface& );
	void register_builtins( void );
private:
	void register_builtin_function( char const*, function_t&& );
private:
	HRuntime( HRuntime const& ) = delete;
	HRuntime& operator = ( HRuntime const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED */

