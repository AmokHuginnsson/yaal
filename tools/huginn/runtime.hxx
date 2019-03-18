/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/runtime.hxx
 * \brief Declaration of huginn::HRuntime class.
 */

#ifndef YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/objectfactory.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRuntime : public HIntrospecteeInterface {
private:
	typedef HHuginn::type_id_t type_id_t;
	typedef HHuginn::identifier_id_t identifier_id_t;
	typedef HHuginn::value_t value_t;
	typedef HHuginn::function_t function_t;
	typedef HHuginn::field_definitions_t field_definitions_t;
	typedef HHuginn::class_t class_t;
	typedef yaal::hcore::HPointer<huginn::HThread> thread_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HThread::id_t, thread_t> threads_t;
	typedef yaal::hcore::HLookupMap<yaal::hcore::HString, identifier_id_t> identifier_ids_t;
	typedef yaal::hcore::HPointer<huginn::HObjectFactory> object_factory_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, class_t> classes_t;
	typedef yaal::hcore::HHashSet<void const*> function_ids_t;
	typedef yaal::hcore::HLookupMap<identifier_id_t, value_t> values_t;
	/*! \brief A type for storing functions.
	 *
	 * Symbol resolving stores pointers to map values (value_t*),
	 * so we cannot use HLookupMap here as adding and removing entries
	 * to HLookupMap invalidates old pointers.
	 */
	typedef yaal::hcore::HList<value_t> values_store_t;
	typedef yaal::hcore::HPointer<values_store_t> shared_values_store_t;
	typedef yaal::hcore::HHashMap<identifier_id_t, value_t const*> global_definitions_t;
public:
	typedef yaal::hcore::HArray<HHuginn::class_t> dependencies_t;
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
	value_t const* _none;
	value_t const* _true;
	value_t const* _false;
	threads_t _threads;
	/*
	 * All classes must be kept directly in runtime so it will be simpler
	 * to reregister, i.e. update class runtime with update_runtime() for
	 * classes defined in submodules.
	 *
	 * Order of _dependencies and _classes in HRuntime definition is vital!
	 */
	dependencies_t _dependencies; /*!< Extra pointers to classes available in runtime, used to enforce order of desturctors! */
	function_ids_t _functionIds; /*!< Needed by `*function_reference*` to `string` conversion. */
	classes_t _classes; /*!< All classes defined for this runtime, but not including classes from submodules. */
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
	 * Once resolved, function reference accessed in submodule must stay resolved
	 * in context of that submodule.
	 *
	 * 4.
	 * Function definitions loaded from sub-module must not be available in current global namespace.
	 *
	 * Because of 1, 2 and 3 we need to have single function definition store shared by all sub-modules.
	 * Because of 4 we need to keep track which functions are defined in current module only.
	 *
	 * It also means that submodule classes must be registered in runtime so its constructors are
	 * still available in its submodule context.
	 *
	 * Hence use of _valuesStore and _globalDefinitions.
	 */
	shared_values_store_t _valuesStore; /*!< All functions ever defined in all sub-modules. */
	global_definitions_t _globalDefinitions; /*!< Functions available in current module context. */
	HHuginn::value_t _argv;
	value_t _result;
	huginn::HThread::frame_t _incrementalFrame;
	int _maxLocalVariableCount;
	int _maxCallStackSize;
	HHuginn::paths_t _modulePaths;
	HHuginn::compiler_setup_t _compilerSetup;
	HHuginn::call_stack_t _trace;
public:
	HRuntime( HHuginn* );
	virtual ~HRuntime( void );
	huginn::HThread* current_thread( void );
	HHuginn* huginn( void ) const {
		return ( _huginn );
	}
	value_t const& none_value( void ) {
		return ( *_none );
	}
	value_t const& true_value( void ) {
		return ( *_true );
	}
	value_t const& false_value( void ) {
		return ( *_false );
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
	type_id_t new_type_id( void );
	identifier_id_t try_identifier_id( yaal::hcore::HString const& ) const;
	identifier_id_t identifier_id( yaal::hcore::HString const& );
	yaal::hcore::HString const& identifier_name( identifier_id_t ) const;
	value_t const* get_global( identifier_id_t ) const;
	class_t get_class( identifier_id_t ) const;
	huginn::HClass const* get_class( void const* ) const;
	HHuginn::value_t find_package( identifier_id_t ) const;
	void register_class( class_t, HHuginn::VISIBILITY );
	/*! \brief Remove compiled class from runtime.
	 *
	 * Code compilation works in such a way that class definitions
	 * are added to runtime as they are compiled.
	 * Symbol resolving is performed as last stage of compilation,
	 * so it is possible that during compilation a class with invalid
	 * symbol references are added to runtime.
	 *
	 * In case of scripts this does not pose any problem as runtime
	 * gets to be destroyed anyway in case of compilation failure.
	 *
	 * In case of incremental mode it is a problem cause we would
	 * end up with dangling class with invalid symbol references
	 * that would trash next statement added.
	 *
	 * So we need a way to remove from runtime all compiled classes
	 * that fail to resolve its symbols in resolve_symbols().
	 *
	 * \param identifier_ - identifier of the class to remove.
	 */
	void drop_class( identifier_id_t identifier_ );
	void register_function( identifier_id_t, function_t, yaal::hcore::HString const& );
	/*! \brief Remove compiled function from runtime.
	 *
	 * See comment for \e drop_class().
	 *
	 * \param identifier_ - identifier of function to remove.
	 */
	void drop_global( identifier_id_t identifier_ );
	void register_global( identifier_id_t, HHuginn::value_t const& );
	void set_setup( HHuginn::paths_t const&, HHuginn::compiler_setup_t );
	void register_package( identifier_id_t, identifier_id_t, int );
	void import_symbols( identifier_id_t, HHuginn::identifiers_t const&, int );
	class_t create_class(
		identifier_id_t,
		yaal::hcore::HString const&,
		HHuginn::ACCESS,
		huginn::HClass::TYPE = huginn::HClass::TYPE::BUILTIN,
		huginn::HClass const* = nullptr,
		huginn::HClass::create_instance_t = nullptr
	);
	class_t create_class(
		yaal::hcore::HString const&,
		yaal::hcore::HString const&,
		HHuginn::ACCESS,
		huginn::HClass::TYPE = huginn::HClass::TYPE::BUILTIN,
		huginn::HClass const* = nullptr,
		huginn::HClass::create_instance_t = nullptr
	);
	class_t create_class( class_constructor_t const& );
	void execute( void );

	/*! \brief Call (execute) Huginn function.
	 *
	 * \param name_ - name of the function to call.
	 * \param argv_ - array of arguments that shall be passed to function call.
	 * \return Result returned by called Huginn function.
	 */
	value_t call( yaal::hcore::HString const& name_, HHuginn::values_t const& argv_ );

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
	void add_argument( HHuginn::value_t const& value_ );

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
	yaal::hcore::HString suggestion( huginn::HClass const*, HHuginn::identifier_id_t ) const;

	yaal::hcore::HString const& function_name( void const* ) const;
	yaal::hcore::HString const* package_name( huginn::HClass const* ) const;
	void copy_text( HRuntime& );
	void fix_references( void );
	void finalize( void );
	HHuginn::class_t make_package( yaal::hcore::HString const&, HRuntime const& );
	template<typename... T>
	HHuginn::value_t create_method( T&&... args_ ) {
		return (
			_objectFactory->create_method( yaal::forward<T>( args_ )... )
		);
	}
	HHuginn::value_t const& boolean_value( bool value_ ) const {
		return ( value_ ? *_true : *_false );
	}
	void set_incremental_frame( huginn::HThread::frame_t const& );
	huginn::HThread::frame_t const& incremental_frame( void ) const;
	HHuginn::paths_t const& module_paths( void ) const;
	HHuginn::compiler_setup_t compiler_setup( void ) const;
	int file_id( void );
	HHuginn::call_stack_t get_call_stack( HThread* );
	HHuginn::call_stack_t const& trace( void ) const;
	using HIntrospecteeInterface::get_call_stack;
	variable_views_t get_locals( HThread*, int );
	using HIntrospecteeInterface::get_locals;
protected:
	virtual HHuginn::call_stack_t do_get_call_stack( void ) override;
	virtual variable_views_t do_get_locals( int ) override;
private:
	value_t call( identifier_id_t identifier_, HHuginn::values_t& argv_, int position_ );
	void register_builtin_function( yaal::hcore::HString const&, function_t&&, yaal::hcore::HString const& );
private:
	HRuntime( HRuntime const& ) = delete;
	HRuntime& operator = ( HRuntime const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RUNTIME_HXX_INCLUDED */

