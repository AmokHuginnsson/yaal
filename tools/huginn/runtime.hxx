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
#include "thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRuntime : public HIntrospecteeInterface {
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
	typedef yaal::hcore::HPointer<huginn::HObjectFactory> object_factory_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, class_t> classes_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, value_t> packages_t;
	/*! \brief A type for storing functions.
	 *
	 * Symbol resolving stores pointers to map values (value_t*),
	 * so we cannot use HLookupMap here as adding and removing entries
	 * to HLookupMap invalidates old pointers.
	 */
	typedef yaal::hcore::HHashMap<identifier_id_t, value_t> functions_store_t;
	typedef yaal::hcore::HHashSet<identifier_id_t> functions_available_t;
	typedef yaal::hcore::HArray<HHuginn::class_t> dependencies_t;
public:
	typedef yaal::hcore::HArray<yaal::hcore::HString> identifier_names_t;
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
	 *
	 * Also _objectFactory contains memory pools used everywhere else
	 * so it must be practically first to create - last to destroy.
	 */
	object_factory_t _objectFactory;
	value_t _none;
	value_t _true;
	value_t _false;
	threads_t _threads;
	/*
	 * 1.
	 * Resolved function references are kept by naked/weak/dumb pointer (value_t*) in runtime.
	 * They must be kept by weak reference to avoid self-referencing cycle of smart pointers
	 * in case of recursive call chain: `foo() { foo(); }'.
	 *
	 * 2.
	 * Loading sub-module involves independent symbol resolving stage at end of compilation
	 * of given submodule.
	 *
	 * 3.
	 * Function definitions loaded from sub-module must not be available in current global namespace.
	 *
	 * Because of 1 and 2 we need to have single function definition store shared by all sub-modules.
	 * Because of 3 we need to keep track which functions are defined in current module only.
	 *
	 * Hence use of _functionsStore and _functionsAvailable.
	 */
	functions_store_t _functionsStore; /*!< All function ever defined in all sub-modules. */
	functions_available_t _functionsAvailable; /*!< Functions available in current module context. */
	dependencies_t _dependencies;
	classes_t _classes;
	packages_t _packages;
	HHuginn::list_t _argv;
	value_t _result;
	huginn::HThread::frame_t _incrementalFrame;
	int _maxLocalVariableCount;
	int _maxCallStackSize;
public:
	HRuntime( HHuginn* );
	huginn::HThread* current_thread( void );
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
	void set_max_call_stack_size( int );
	int max_local_variable_count( void ) const {
		return ( _maxLocalVariableCount );
	}
	int max_call_stack_size( void ) const {
		return ( _maxCallStackSize );
	}
	identifier_id_t identifier_id( yaal::hcore::HString const& );
	yaal::hcore::HString const& identifier_name( identifier_id_t ) const;
	value_t* get_function( identifier_id_t, bool = false );
	class_t get_class( identifier_id_t );
	value_t* get_package( identifier_id_t );
	void register_class_low( class_t, HHuginn::ACCESS );
	void register_function( identifier_id_t, function_t, yaal::hcore::HString const& );
	void register_package( identifier_id_t, identifier_id_t, HHuginn::paths_t const&, HHuginn::compiler_setup_t, int );
	class_t create_class( identifier_id_t, HHuginn::HClass const*, field_definitions_t const&, yaal::hcore::HString const& );
	class_t create_class( yaal::hcore::HString const&, HHuginn::HClass const*, field_definitions_t const&, yaal::hcore::HString const& );
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

	/*! \brief Call (execute) Huginn function.
	 *
	 * \param identifier_ - identifier of the function to call.
	 * \param argv_ - array of arguments that shall be passed to function call.
	 * \param position_ - call context information for error reporting.
	 * \return Result returned by called Huginn function.
	 */
	value_t call( identifier_id_t identifier_, values_t const& argv_, int position_ );

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

	/*! \brief Reset the runtime state so it can be used again with new program source.
	 */
	void reset( void );

	/*! \brief Dump Huginn Virtual Machine state onto given stream.
	 *
	 * \param stream_ - stream where VM state shall be dumped.
	 */
	void dump_vm_state( yaal::hcore::HStreamInterface& ) const;
	void dump_docs( yaal::hcore::HStreamInterface& ) const;
	void register_builtins( void );

	yaal::hcore::HString suggestion( HHuginn::identifier_id_t ) const;

	yaal::hcore::HString const& function_name( void const* ) const;
	void copy_text( HRuntime& );
	HHuginn::class_t make_package( yaal::hcore::HString const&, HRuntime const& );
	void set_incremental_frame( huginn::HThread::frame_t const& );
	huginn::HThread::frame_t const& incremental_frame( void ) const;
protected:
	virtual call_stack_t do_get_call_stack( void ) override;
private:
	void register_builtin_function( yaal::hcore::HString const&, function_t&&, yaal::hcore::HString const& );
private:
	HRuntime( HRuntime const& ) = delete;
	HRuntime& operator = ( HRuntime const& ) = delete;
};

namespace huginn_builtin {

HHuginn::value_t string( HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t integer( HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t real( HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t boolean( HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t character( HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t number( HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t size( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t type( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t copy( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t observe( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t use( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t list( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t deque( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t dict( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t order( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t lookup( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t set( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t print( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );
HHuginn::value_t input( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t const&, int );

}

extern HHuginn::identifier_id_t const INVALID_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_NONE_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_OBSERVER_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_REFERENCE_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_FUNCTION_REFERENCE_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_OBJECT_REFERENCE_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_METHOD_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_BOUND_METHOD_IDENTIFIER;
extern HHuginn::identifier_id_t const TYPE_UNKNOWN_IDENTIFIER;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED */

