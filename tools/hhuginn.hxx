/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hhuginn.hxx
 * Declaration of HHuginn class and all its nested classes.
 */

#ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED
#define YAAL_TOOLS_HHUGINN_HXX_INCLUDED 1

#include <atomic>

#include "hcore/hmap.hxx"
#include "hcore/hset.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hstack.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hthread.hxx"
#include "hcore/htaggedpod.hxx"
#include "tools/executingparser.hxx"
#include "tools/huginn/valuereference.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSource;
class HFrame;
class HThread;
class HStatement;
class HExpression;
class HScope;
class HFunction;
/*! \brief ObjectFactory holds built-in type definitions local to HHuginn instance.
 */
class HObjectFactory;
struct OCompiler;
class HRuntime;
class HPackageFactory;
class HIteratorInterface;

namespace ERR_CODE {
enum {
	OP_NOT_SUM = 0,
	OP_NOT_SUB,
	OP_NOT_MUL,
	OP_NOT_DIV,
	OP_NOT_EXP,
	OP_NOT_CMP,
	OP_NOT_NUM,
	OPS_NOT_BOOL,
	OP_NOT_BOOL,
	IDX_NOT_INT,
	OP_NOT_COLL
};
}

extern M_YAAL_TOOLS_PUBLIC_API char const* _errMsgHHuginn_[];

}

class HIntrospectorInterface;

/*! \brief Huginn programming language implementation.
 */
class HHuginn {
public:
	typedef HHuginn this_type;
	struct type_tag;
	struct identifier_tag;
	typedef yaal::hcore::HTaggedPOD<int, type_tag> type_id_t;
	typedef yaal::hcore::HTaggedPOD<int, identifier_tag> identifier_id_t;
	typedef yaal::hcore::HPointer<HHuginn> ptr_t;
	class HNotifableReference;
	class HReferenceTracker;
	class HIterable;
	class HInvalidatingIterable;
	typedef yaal::hcore::HPointer<HIterable> iterable_t;
	typedef yaal::hcore::HPointer<huginn::HScope> scope_t;
	class HClass;
	typedef yaal::hcore::HPointer<HClass> class_t;
	class HObserver;
	class HReference;
	class HFunctionReference;
	class HValue;
	class HTaggedValue;
	typedef yaal::tools::huginn::HValueReference<HValue> value_t;
	typedef yaal::tools::huginn::HValueReferenceObserver<HValue> value_ref_t;
	class HBoolean;
	class HInteger;
	class HReal;
	class HString;
	class HCharacter;
	class HNumber;
	class HTuple;
	class HList;
	class HDeque;
	class HDict;
	class HOrder;
	class HLookup;
	class HSet;
	class HBlob;
	class HObject;
	class HException;
	class HEnumeral;
	class HObjectReference;
	class HTernaryEvaluator;
	class HValueHashHelper;
	class HValueCompareHelper;
	typedef yaal::hcore::HPointer<huginn::HExpression> expression_t;
	typedef yaal::hcore::HArray<HHuginn::expression_t> expressions_t;
	class HCoordinate;
	class HFieldDefinition;
	typedef yaal::hcore::HArray<HFieldDefinition> field_definitions_t;
	typedef yaal::hcore::HArray<identifier_id_t> identifiers_t;
	typedef yaal::hcore::HArray<identifier_id_t> field_identifiers_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> field_descriptions_t;
	typedef yaal::hcore::HArray<value_t> values_t;
	typedef yaal::hcore::HPointer<huginn::HFrame> frame_t;
	typedef yaal::hcore::HBoundCall<value_t ( huginn::HThread*, value_t*, values_t&, int )> function_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> paths_t;
	enum class TYPE {
		NONE,
		BOOLEAN,
		INTEGER,
		REAL,
		STRING,
		NUMBER,
		CHARACTER,
		TUPLE,
		LIST,
		DEQUE,
		DICT,
		ORDER,
		LOOKUP,
		SET,
		BLOB,
		OBSERVER,
		REFERENCE,
		FUNCTION_REFERENCE,
		OBJECT_REFERENCE,
		METHOD,
		UNBOUND_METHOD,
		BOUND_METHOD,
		VARIADIC_PARAMETERS,
		NAMED_PARAMETERS,
		UNKNOWN,
		NOT_BOOLEAN
	};
	struct COMPILER;
	/*! \brief Bits used for specifying how compiler should work.
	 */
	typedef yaal::hcore::HBitFlag<COMPILER> compiler_setup_t;
	struct COMPILER {
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const DEFAULT;
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const BE_STRICT;
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const BE_SLOPPY;
	};
	class HHuginnRuntimeException;
	typedef yaal::hcore::HPointer<huginn::HSource> source_t;
	typedef yaal::hcore::HArray<source_t> sources_t;
	typedef yaal::hcore::HResource<huginn::OCompiler> compiler_t;
	typedef yaal::hcore::HResource<huginn::HRuntime> runtime_t;
	/*! \brief Huginn class (constructor) access specification.
	 *
	 * The class (constructor) access mode is inferred from class definition context
	 * and cannot be set manually by user.
	 */
	enum class ACCESS {
		PRIVATE,
		PUBLIC
	};
	enum class VISIBILITY {
		GLOBAL,
		PACKAGE
	};
	enum class SYMBOL_KIND {
		UNKNOWN,
		CLASS,
		FIELD,
		METHOD,
		FUNCTION,
		ENUM,
		VARIABLE,
		PACKAGE
	};
private:
	enum class STATE {
		EMPTY,
		LOADED,
		PREPROCESSED,
		PARSED,
		COMPILED,
		RUNNING
	};
	STATE _state;
	runtime_t _runtime;
	sources_t _sources;
	compiler_t _compiler;
	HExecutingParser _engine;
	yaal::hcore::HString _errorMessage;
	int _errorPosition;
	int _errorFileId;
	typedef std::atomic<bool> flag_t;
	yaal::hcore::HStreamInterface::ptr_t _inputStream;
	yaal::hcore::HStreamInterface* _inputStreamRaw;
	yaal::hcore::HStreamInterface::ptr_t _outputStream;
	yaal::hcore::HStreamInterface* _outputStreamRaw;
	yaal::hcore::HStreamInterface::ptr_t _errorStream;
	yaal::hcore::HStreamInterface* _errorStreamRaw;
	yaal::hcore::HStreamInterface::ptr_t _logStream;
	yaal::hcore::HStreamInterface* _logStreamRaw;
	static flag_t _grammarVerified;
public:
	static paths_t const MODULE_PATHS;
public:
	HHuginn( void );
	virtual ~HHuginn( void );

	/*! \brief Store source in internal buffer.
	 *
	 * \param stream_ - stream that contains program source.
	 * \param skippedLines_ - number of lines skipped from real input.
	 */
	void load( yaal::hcore::HStreamInterface& stream_, int skippedLines_ = 0 );

	/*! \brief Store source in internal buffer.
	 *
	 * \param stream_ - stream that contains program source.
	 * \param name_ - symbolic name of program source.
	 * \param skippedLines_ - number of lines skipped from real input.
	 */
	void load( yaal::hcore::HStreamInterface& stream_, yaal::hcore::HString const& name_, int skippedLines_ = 0 );

	/*! \brief Preprocess loaded program source.
	 */
	void preprocess( void );

	/*! \brief Parse preprocessed program source.
	 *
	 * \return True iff whole input source was parsed successfully.
	 */
	bool parse( void );

	/*! \brief Compile parsed program.
	 *
	 * \param compilerSetup_ - decide how compiler should work.
	 * \param introspector_ - program state introspector.
	 * \return True iff compilation finished with no errors.
	 */
	bool compile( compiler_setup_t compilerSetup_ = COMPILER::DEFAULT, HIntrospectorInterface* introspector_ = nullptr );

	/*! \brief Compile parsed program.
	 *
	 * \param paths_ - paths to Huginn modules directories.
	 * \param compilerSetup_ - decide how compiler should work.
	 * \param introspector_ - program state introspector.
	 * \return True iff compilation finished with no errors.
	 */
	bool compile( paths_t const& paths_, compiler_setup_t compilerSetup_ = COMPILER::DEFAULT, HIntrospectorInterface* introspector_ = nullptr );

	/*! \brief Execute compiled program.
	 *
	 * \return True iff execution finished with no errors.
	 */
	bool execute( void );

	/*! \brief Get value returned by program's main().
	 *
	 * \return Value returned by program's main() function.
	 */
	value_t result( void ) const;

	/*! \brief Dump Huginn Virtual Machine state onto given stream.
	 *
	 * \param stream_ - stream where VM state shall be dumped.
	 */
	void dump_vm_state( yaal::hcore::HStreamInterface& ) const;

	/*! \brief Dump documentation strings into given stream.
	 *
	 * \param stream_ - stream where documentation string shall be dumped.
	 */
	void dump_docs( yaal::hcore::HStreamInterface& ) const;

	/*! \brief Add argument for main() function.
	 *
	 * \param value_ - value of argument being added.
	 */
	void add_argument( yaal::hcore::HString const& value_ );
	void add_argument( value_t const& value_ );

	/*! \brief Remove all argument currently held for main() function.
	 */
	void clear_arguments( void );

	/*! \brief Reset the interpreter state so it can be used again with new program source.
	 *
	 * \param undoSteps_ - number of statements in main() loop to undo for incremental mode.
	 */
	void reset( int undoSteps_ = 0 );

	/*! \brief Set maximum call stack size for this interpreter instance.
	 */
	void set_max_call_stack_size( int );
	void dump_preprocessed_source( yaal::hcore::HStreamInterface& ) const;
	int error_position( void ) const;
	int file_id( void ) const;
	HCoordinate error_coordinate( void ) const;
	HCoordinate get_coordinate( int, int ) const;
	int real_position( int, int ) const;
	yaal::hcore::HString source_name( int ) const;
	yaal::hcore::HString where( int, int ) const;
	yaal::hcore::HString error_message( void ) const;
	void set_input_stream( yaal::hcore::HStreamInterface& );
	void set_input_stream( yaal::hcore::HStreamInterface::ptr_t );
	void set_output_stream( yaal::hcore::HStreamInterface& );
	void set_output_stream( yaal::hcore::HStreamInterface::ptr_t );
	void set_error_stream( yaal::hcore::HStreamInterface& );
	void set_error_stream( yaal::hcore::HStreamInterface::ptr_t );
	void set_log_stream( yaal::hcore::HStreamInterface& );
	void set_log_stream( yaal::hcore::HStreamInterface::ptr_t );
	executing_parser::HRule make_engine( huginn::HRuntime* = nullptr );
	yaal::hcore::HStreamInterface& input_stream( void );
	yaal::hcore::HStreamInterface& output_stream( void );
	yaal::hcore::HStreamInterface& error_stream( void );
	yaal::hcore::HStreamInterface& log_stream( void );
	yaal::hcore::HString get_snippet( int, int ) const;
	yaal::hcore::HString const& get_comment( int ) const;
	void register_class( class_t, VISIBILITY = VISIBILITY::PACKAGE );
	void register_function( identifier_id_t );
	void register_function( yaal::hcore::HString const&, function_t&&, yaal::hcore::HString const& );
	static void disable_grammar_verification( void );
	value_t value( int long long );
	value_t value( code_point_t );
	value_t value( double long );
	value_t value( yaal::hcore::HNumber const& );
	value_t value( yaal::hcore::HString const& );
	value_t value( bool );
	value_t value( void const* );
	value_t const& value( value_t const& val_ ) const {
		return ( val_ );
	}
	value_t value( values_t&& val_ );
	value_t value( yaal::hcore::HDeque<value_t>&& val_ );
	template<typename T>
	value_t value( yaal::hcore::HArray<T> const& val_ ) {
		M_PROLOG
		values_t data;
		data.reserve( val_.get_size() );
		for ( T const& e : val_ ) {
			data.emplace_back( value( e ) );
		}
		return ( value( yaal::move( data ) ) );
		M_EPILOG
	}
	template<typename T>
	value_t value( yaal::hcore::HDeque<T> const& val_ ) {
		M_PROLOG
		yaal::hcore::HDeque<value_t> data;
		for ( T const& e : val_ ) {
			data.emplace_back( value( e ) );
		}
		return ( value( yaal::move( data ) ) );
		M_EPILOG
	}
	/* type forwarders */
	value_t value( int long val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( int val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( int short val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( int long long unsigned val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( int long unsigned val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( int unsigned val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( int short unsigned val_ ) {
		return ( value( static_cast<int long long>( val_ ) ) );
	}
	value_t value( double val_ ) {
		return ( value( static_cast<double long>( val_ ) ) );
	}
	value_t value( float val_ ) {
		return ( value( static_cast<double long>( val_ ) ) );
	}
	value_t value( char const* val_ ) {
		return ( value( yaal::hcore::HString( val_ ) ) );
	}
	value_t value( std::nullptr_t ) {
		return ( value( static_cast<void const*>( 0 ) ) );
	}
private:
	HHuginn( huginn::HRuntime* );
	huginn::HRuntime const& runtime( void ) const;
	void finalize_compilation( void );
	HClass const* commit_class( identifier_id_t );
	huginn::HSource& current_source( void );
	huginn::HSource const& current_source( void ) const;
	char const* error_message( int ) const;
	HHuginn( HHuginn const& ) = delete;
	HHuginn& operator = ( HHuginn const& ) = delete;
	friend yaal::hcore::HString to_string( HHuginn::value_t const&, HHuginn* );
	friend class huginn::HPackageFactory;
};

typedef yaal::hcore::HExceptionT<HHuginn> HHuginnException;

class HIntrospecteeInterface {
public:
	class HCallSite {
		yaal::hcore::HString _file;
		int _line;
		int _column;
		yaal::hcore::HString _context;
	public:
		HCallSite( yaal::hcore::HString const&, int, int, yaal::hcore::HString const& );
		yaal::hcore::HString const& file( void ) const;
		int line( void ) const;
		int column( void ) const;
		yaal::hcore::HString const& context( void ) const;
	};
	class HVariableView {
		yaal::hcore::HString _name;
		HHuginn::value_ref_t _view;
	public:
		HVariableView( yaal::hcore::HString const&, HHuginn::value_t const& );
		yaal::hcore::HString const& name( void ) const;
		HHuginn::value_t value( void ) const;
	};
	typedef yaal::hcore::HArray<HCallSite> call_stack_t;
	typedef yaal::hcore::HArray<HVariableView> variable_views_t;
public:
	virtual ~HIntrospecteeInterface( void ) {}
	call_stack_t get_call_stack( void );
	variable_views_t get_locals( int );
protected:
	virtual call_stack_t do_get_call_stack( void ) = 0;
	virtual variable_views_t do_get_locals( int ) = 0;
};

class HIntrospectorInterface {
public:
	typedef yaal::hcore::HArray<yaal::hcore::HString> symbols_t;
public:
	virtual ~HIntrospectorInterface( void ) {}
	void introspect( HIntrospecteeInterface& );
	void symbol( symbols_t const&, HHuginn::SYMBOL_KIND, int, int );
protected:
	virtual void do_introspect( HIntrospecteeInterface& ) = 0;
	virtual void do_symbol( symbols_t const&, HHuginn::SYMBOL_KIND, int, int ) {}
};

class HHuginn::HCoordinate {
public:
	typedef HCoordinate this_type;
private:
	int _line;
	int _column;
public:
	HCoordinate( int line_, int column_ )
		: _line( line_ ), _column( column_ ) {
	}
	int line( void ) const {
		return ( _line );
	}
	int column( void ) const {
		return ( _column );
	}
};

class HHuginn::HHuginnRuntimeException {
public:
	typedef HHuginn::HHuginnRuntimeException this_type;
private:
	yaal::hcore::HString _message;
	int _fileId;
	int _position;
public:
	HHuginnRuntimeException( yaal::hcore::HString const&, int, int );
	yaal::hcore::HString const& message( void ) const;
	int position( void ) const;
	int file_id( void ) const;
};

class HHuginn::HFieldDefinition {
	yaal::hcore::HString _name;
	value_t _value;
	yaal::hcore::HString _doc;
public:
	HFieldDefinition( yaal::hcore::HString const& name_, value_t const& value_, yaal::hcore::HString const& doc_ )
		: _name( name_ )
		, _value( value_ )
		, _doc( doc_ ) {
		return;
	}
	yaal::hcore::HString const& name( void ) const {
		return ( _name );
	}
	value_t const& value( void ) const {
		return ( _value );
	}
	yaal::hcore::HString const& doc( void ) const {
		return ( _doc );
	}
};

class HHuginn::HClass {
public:
	enum class TYPE {
		BUILTIN,
		USER
	};
	enum class MEMBER_TYPE {
		INSTANCE,
#undef STATIC
		STATIC
	};
	typedef HHuginn::HClass this_type;
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, int> field_indexes_t;
	typedef value_t ( *create_instance_t )( HClass const*, huginn::HThread*, values_t&, int );
	typedef HHuginn::value_t ( *constructor_t )( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	class HMethod;
	class HUnboundMethod;
	class HBoundMethod;
private:
	huginn::HRuntime* _runtime;
	type_id_t _typeId;
	identifier_id_t _identifierId;
	HClass const* _super;
	create_instance_t _createInstance;
	field_identifiers_t _fieldIdentifiers;
	field_indexes_t _staticFieldIndexes;
	field_indexes_t _fieldIndexes;
	values_t _fieldDefinitions;
	value_t _constructor;
	field_descriptions_t _fieldDescriptions;
	yaal::hcore::HString _doc;
	TYPE _type;
	HClass const* _origin;
public:
	HClass( huginn::HRuntime*, type_id_t, identifier_id_t, yaal::hcore::HString const&, ACCESS, TYPE = TYPE::BUILTIN, HClass const* = nullptr, create_instance_t = nullptr );
	HClass( huginn::HRuntime*, huginn::HObjectFactory*, type_id_t, identifier_id_t, yaal::hcore::HString const&, constructor_t ); /* from HObjectFactory constructor */
	HClass( huginn::HRuntime*, char const*, char const*, HClass const* ); /* ReversedTupleView, ReversedListView, ... */
	HClass( huginn::HRuntime*, HHuginn::TYPE, identifier_id_t, char const* ); /* *none*, *function_reference*, *observer*, ... */
	virtual ~HClass( void ) {
	}
	void redefine( HClass const*, field_definitions_t const& );
	void add_member( HHuginn::HFieldDefinition const&, MEMBER_TYPE = MEMBER_TYPE::INSTANCE );
	HClass const* super( void ) const {
		return ( _super );
	}
	HClass const* origin( void ) const {
		return ( _origin );
	}
	identifier_id_t const& identifier_id( void ) const {
		return ( _identifierId );
	}
	yaal::hcore::HString const& name( void ) const;
	type_id_t const& type_id( void ) const {
		return ( _typeId );
	}
	TYPE type( void ) const {
		return ( _type );
	}
	ACCESS access( void ) const;
	field_identifiers_t const& field_identifiers( void ) const {
		return ( _fieldIdentifiers );
	}
	values_t const& field_definitions( void ) const {
		return ( _fieldDefinitions );
	}
	int field_index( identifier_id_t const&, MEMBER_TYPE = MEMBER_TYPE::INSTANCE ) const;
	value_t const& field( int index_ ) const {
		return ( _fieldDefinitions[index_] );
	}
	function_t constructor_function( void ) const;
	values_t get_defaults( huginn::HThread*, int ) const;
	value_t get_default( huginn::HThread*, int, int ) const;
	function_t const& function( int ) const;
	bool is_kind_of( identifier_id_t const& ) const;
	bool is_complex( void ) const {
		return ( ! _fieldDefinitions.is_empty() );
	}
	bool is_overridden( HClass const* super_, int index_ ) const {
		return ( _fieldDefinitions[index_] != super_->_fieldDefinitions[index_] );
	}
	bool has_builtin_base( void ) const;
	void update_runtime( huginn::HRuntime* );
	void set_origin( HClass const* );
	huginn::HRuntime* runtime( void ) const {
		return ( _runtime );
	}
	HHuginn::value_t const& constructor( void ) const {
		return ( _constructor );
	}
	static value_t create_instance_default( HClass const*, huginn::HThread*, values_t&, int );
	value_t create_instance( huginn::HThread*, value_t*, values_t&, int ) const;
	yaal::hcore::HString const& doc( void ) const;
	yaal::hcore::HString const& doc( identifier_id_t ) const;
	void finalize_registration( huginn::HRuntime* );
private:
	value_t make_constructor( huginn::HObjectFactory*, HHuginn::ACCESS ) const;
	value_t make_constructor( huginn::HObjectFactory*, constructor_t ) const;
	value_t access_violation( huginn::HThread*, value_t*, values_t&, int ) const __attribute__((noreturn));
	value_t base_class_not_initialized( huginn::HThread*, value_t*, values_t&, int ) const;
	virtual void do_finalize_registration( huginn::HRuntime* );
	virtual value_t do_create_instance( huginn::HThread*, values_t&, int ) const;
	HClass( HClass const& ) = delete;
	HClass& operator = ( HClass const& ) = delete;
};

class HHuginn::HValue {
public:
	typedef HHuginn::HValue this_type;
private:
	HClass const* _class;
public:
	HValue( HClass const* );
	virtual ~HValue( void ) {
		return;
	}
	type_id_t const& type_id( void ) const {
		return ( _class->type_id() );
	}
	HClass const* get_class( void ) const {
		return ( _class );
	}
	value_t clone( huginn::HThread*, HHuginn::value_t*, int ) const;
	int field_index( identifier_id_t const& ) const;
	bool is_kind_of( identifier_id_t const& ) const;
	value_t field( HHuginn::value_t const& subject_, int index_ ) const {
		return ( do_field( subject_, index_ ) );
	}
	value_t const& field( int index_ ) const {
		return ( do_field( index_ ) );
	}
private:
	virtual value_t do_field( HHuginn::value_t const&, int ) const;
	virtual value_t const& do_field( int ) const;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const;
	HValue( HValue const& ) = delete;
	HValue& operator = ( HValue const& ) = delete;
};

class HHuginn::HClass::HMethod : public HHuginn::HValue {
	typedef HHuginn::HClass::HMethod this_type;
	typedef HHuginn::HValue base_type;
protected:
	HHuginn::function_t _function;
public:
	HMethod( HHuginn::HClass const*, HHuginn::function_t const& );
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
private:
	HMethod( HMethod const& ) = delete;
	HMethod& operator = ( HMethod const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HClass::HUnboundMethod : public HHuginn::HValue {
	typedef HHuginn::HClass::HUnboundMethod this_type;
	typedef HHuginn::HValue base_type;
protected:
	HHuginn::HClass const* _juncture;
	HHuginn::function_t _function;
public:
	HUnboundMethod( HHuginn::HClass const*, HHuginn::HClass const*, HHuginn::function_t const& );
	HHuginn::value_t call( huginn::HThread*, values_t&, int );
private:
	HUnboundMethod( HUnboundMethod const& ) = delete;
	HUnboundMethod& operator = ( HUnboundMethod const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HClass::HBoundMethod : public HHuginn::HValue {
	typedef HHuginn::HClass::HBoundMethod this_type;
	typedef HHuginn::HValue base_type;
private:
	HHuginn::function_t _function;
	HHuginn::value_t _objectHolder;
public:
	HBoundMethod( HHuginn::HClass const*, HHuginn::function_t const&, HHuginn::value_t const& );
	HHuginn::value_t call( huginn::HThread*, values_t&, int );
	HHuginn::value_t const& subject( void ) const {
		return ( _objectHolder );
	}
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
private:
	HBoundMethod( HBoundMethod const& ) = delete;
	HBoundMethod& operator = ( HBoundMethod const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HObject : public HHuginn::HValue {
public:
	typedef HHuginn::HObject this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HArray<value_t> fields_t;
private:
	fields_t _fields;
public:
	HObject( HClass const*, fields_t const& );
	virtual ~HObject( void );
	value_t& field_ref( int );
	HHuginn::value_t call_method( huginn::HThread*, HHuginn::value_t const&, HHuginn::identifier_id_t const&, HHuginn::values_t&, int ) const;
	HHuginn::value_t get_method( huginn::HThread*, HHuginn::value_t const&, HHuginn::identifier_id_t const&, int ) const;
	static HHuginn::value_t init_base( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t forward_call( HHuginn::function_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
private:
	HObject( HObject const& ) = delete;
	HObject& operator = ( HObject const& ) = delete;
private:
	HHuginn::function_t const& get_method( huginn::HThread*, HHuginn::identifier_id_t const&, int ) const;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual value_t do_field( HHuginn::value_t const&, int ) const override;
	value_t const& do_field( int ) const override;
};

/*! \brief Type hierarchy aware object reference.
 *
 * Sole purpose of this class is to handle `super' Huginn keyword.
 */
class HHuginn::HObjectReference : public HHuginn::HValue {
public:
	typedef HHuginn::HObjectReference this_type;
	typedef HHuginn::HValue base_type;
private:
	value_t _value;
	HClass const* _referenceClass;
public:
	HObjectReference( HHuginn::HClass const*, value_t const&, int, int, int );
	HObjectReference( HHuginn::HClass const*, value_t const&, HClass const* );
	int field_index( identifier_id_t const& ) const;
	value_t field( huginn::HThread*, int, int );
	HHuginn::HClass const* reference_class( void ) const {
		return ( _referenceClass );
	}
	HHuginn::value_t const& value( void ) {
		return ( _value );
	}
private:
	HObjectReference( HObjectReference const& ) = delete;
	HObjectReference& operator = ( HObjectReference const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

/*! \brief Huginn Value tagger.
 *
 * Sole purpose of this class is to help with support for named parameters and variadic functions.
 */
class HHuginn::HTaggedValue : public HHuginn::HValue {
public:
	typedef HHuginn::HTaggedValue this_type;
	typedef HHuginn::HValue base_type;
private:
	value_t _value;
public:
	HTaggedValue( value_t const&, HClass const* );
	value_t const& value( void ) const {
		return ( _value );
	}
	value_t& value( void ) {
		return ( _value );
	}
private:
	HTaggedValue( HTaggedValue const& ) = delete;
	HTaggedValue& operator = ( HTaggedValue const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

class HHuginn::HObserver : public HHuginn::HValue {
public:
	typedef HHuginn::HObserver this_type;
	typedef HHuginn::HValue base_type;
	typedef HHuginn::value_ref_t value_type;
private:
	value_type _value;
public:
	HObserver( HHuginn::HClass const*, HHuginn::value_t const& );
	HHuginn::value_t value( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HReference : public HHuginn::HValue {
public:
	typedef HHuginn::HReference this_type;
	typedef HHuginn::HValue base_type;
private:
	HHuginn::value_t& _value;
public:
	HReference( HHuginn::HClass const*, HHuginn::value_t& );
	HHuginn::value_t& value( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

class HHuginn::HTernaryEvaluator : public HHuginn::HValue {
public:
	typedef HHuginn::HTernaryEvaluator this_type;
	typedef HHuginn::HValue base_type;
private:
	expression_t _condition;
	expression_t _ifTrue;
	expression_t _ifFalse;
public:
	HTernaryEvaluator( expression_t const&, expression_t const&, expression_t const& );
	value_t execute( huginn::HThread* );
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

class HHuginn::HIterable : public HHuginn::HValue {
public:
	typedef HHuginn::HIterable this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HPointer<huginn::HIteratorInterface> iterator_t;
	static int long const npos = -1;
	HIterable( HClass const* );
	iterator_t iterator( huginn::HThread*, int );
	int long size( huginn::HThread*, int ) const;
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) = 0;
	virtual int long do_size( huginn::HThread*, int ) const = 0;
};

class HHuginn::HReferenceTracker {
public:
	typedef yaal::hcore::HArray<HHuginn::HNotifableReference*> observers_t;
	typedef yaal::hcore::HArray<void const*> ids_t;
protected:
	observers_t _observers;
public:
	HReferenceTracker( void );
	virtual ~HReferenceTracker() {}
	void notify( HHuginn::HNotifableReference* );
	void forget( HHuginn::HNotifableReference* );
	void invalidate( void const* );
	void invalidate( ids_t& );
	void invalidate( void );
};

class HHuginn::HNotifableReference {
private:
	HHuginn::HReferenceTracker* _owner;
public:
	HNotifableReference( HHuginn::HReferenceTracker* owner_ )
		: _owner( owner_ ) {
		_owner->notify( this );
	}
	virtual ~HNotifableReference( void ) {
		_owner->forget( this );
	}
	void invalidate( void ) {
		do_invalidate();
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual void do_invalidate( void ) = 0;
	virtual void const* do_id( void ) const = 0;
private:
	HNotifableReference( HNotifableReference const& ) = delete;
	HNotifableReference& operator = ( HNotifableReference const& ) = delete;
};

class HHuginn::HInvalidatingIterable : public HHuginn::HIterable, public HHuginn::HReferenceTracker {
public:
	HInvalidatingIterable( HClass const* );
	void skip( void const* );
};

class HHuginn::HBoolean : public HHuginn::HValue {
public:
	typedef HHuginn::HBoolean this_type;
	typedef HHuginn::HValue base_type;
	typedef bool value_type;
private:
	value_type _value;
public:
	HBoolean( HHuginn::HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	void flip( void ) {
		_value = ! _value;
		return;
	}
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HInteger : public HHuginn::HValue {
public:
	typedef HHuginn::HInteger this_type;
	typedef HHuginn::HValue base_type;
	typedef int long long value_type;
private:
	value_type _value;
public:
	HInteger( HHuginn::HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HReal : public HHuginn::HValue {
public:
	typedef HHuginn::HReal this_type;
	typedef HHuginn::HValue base_type;
	typedef double long value_type;
private:
	value_type _value;
public:
	HReal( HHuginn::HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HString : public HHuginn::HIterable {
public:
	typedef HHuginn::HString this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HString value_type;
private:
	value_type _value;
public:
	HString( HHuginn::HClass const*, value_type const& );
	value_type const& value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HCharacter : public HHuginn::HValue {
public:
	typedef HHuginn::HCharacter this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::code_point_t value_type;
private:
	value_type _value;
public:
	HCharacter( HHuginn::HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	void set( value_type );
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HNumber : public HHuginn::HValue {
public:
	typedef HHuginn::HNumber this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HNumber value_type;
private:
	value_type _value;
public:
	HNumber( HHuginn::HClass const*, value_type const& );
	value_type const& value( void ) const {
		return ( _value );
	}
	value_type& value( void ) {
		return ( _value );
	}
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HTuple : public HHuginn::HIterable {
public:
	typedef HHuginn::HTuple this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HArray<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HTuple( HHuginn::HClass const*, values_t&& );
	int long find( huginn::HThread*, int, HHuginn::value_t const&, int long = 0, int long = -1 ) const;
	value_t get( int long long );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HList : public HHuginn::HIterable {
public:
	typedef HHuginn::HList this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HArray<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HList( HHuginn::HClass const*, values_t&& );
	void push_back( value_t const& );
	void pop_back( void );
	void clear( void );
	int long find( huginn::HThread*, int, HHuginn::value_t const&, int long = 0, int long = -1 ) const;
	value_t get( int long long );
	value_t& get_ref( int long long );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HDeque : public HHuginn::HIterable {
public:
	typedef HHuginn::HDeque this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HDeque<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HDeque( HHuginn::HClass const*, values_t&& );
	void push_back( value_t const& );
	void pop_back( void );
	void push_front( value_t const& );
	void pop_front( void );
	void clear( void );
	int long find( huginn::HThread*, int, HHuginn::value_t const&, int long = 0, int long = -1 ) const;
	value_t get( int long long );
	value_t& get_ref( int long long );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HValueCompareHelper final {
	typedef bool (*compare_t)( huginn::HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
	huginn::HThread* _thread;
	int _position;
	compare_t _compare;
public:
	HValueCompareHelper( compare_t );
	void anchor( huginn::HThread* thread_, int position_ ) {
		_thread = thread_;
		_position = position_;
	}
	bool operator()( HHuginn::value_t const&, HHuginn::value_t const& ) const;
	void detach( void ) {
		_thread = nullptr;
		_position = 0;
	}
private:
	HValueCompareHelper( HValueCompareHelper const& ) = delete;
	HValueCompareHelper& operator = ( HValueCompareHelper const& ) = delete;
};

class HHuginn::HDict : public HHuginn::HInvalidatingIterable {
public:
	typedef HHuginn::HDict this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HMap<HHuginn::value_t, HHuginn::value_t, HValueCompareHelper&> values_prototype_t;
	typedef yaal::hcore::HPool<values_prototype_t::node_size::value> pool_t;
	typedef allocator::shared_pool<values_prototype_t::node_type> allocator_t;
	typedef yaal::hcore::HMap<HHuginn::value_t, HHuginn::value_t, HValueCompareHelper&, allocator_t> values_t;
private:
	mutable HValueCompareHelper _helper;
	values_t _data;
	HHuginn::HClass const* _keyType;
public:
	HDict( HHuginn::HClass const*, allocator_t const& );
	value_t get( huginn::HThread*, HHuginn::value_t const&, int );
	value_t& get_ref( huginn::HThread*, HHuginn::value_t const&, int );
	void insert( huginn::HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
	bool has_key( huginn::HThread*, HHuginn::value_t const&, int ) const;
	bool try_get( huginn::HThread*, HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ );
	void erase( huginn::HThread*, HHuginn::value_t const&, int );
	HHuginn::HClass const* key_type( void ) const;
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
	void verify_key_type( huginn::HThread*, HHuginn::HClass const*, int ) const;
	void update_key_type( huginn::HThread*, HHuginn::HClass const*, int );
	void clear( void );
	void anchor( huginn::HThread* thread_, int position_ ) const {
		_helper.anchor( thread_, position_ );
	}
	void detach( void ) const {
		_helper.detach();
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	HDict( HDict const& ) = delete;
	HDict& operator = ( HDict const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HOrder : public HHuginn::HInvalidatingIterable {
public:
	typedef HHuginn::HOrder this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HSet<HHuginn::value_t, HValueCompareHelper&> values_prototype_t;
	typedef yaal::hcore::HPool<values_prototype_t::node_size::value> pool_t;
	typedef allocator::shared_pool<values_prototype_t::node_type> allocator_t;
	typedef yaal::hcore::HSet<HHuginn::value_t, HValueCompareHelper&, allocator_t> values_t;
private:
	mutable HValueCompareHelper _helper;
	values_t _data;
	HHuginn::HClass const* _keyType;
public:
	HOrder( HHuginn::HClass const*, allocator_t const& );
	void insert( huginn::HThread*, HHuginn::value_t const&, int );
	bool has_key( huginn::HThread*, HHuginn::value_t const&, int ) const;
	void erase( huginn::HThread*, HHuginn::value_t const&, int );
	HHuginn::HClass const* key_type( void ) const;
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
	void verify_key_type( huginn::HThread*, HHuginn::HClass const*, int ) const;
	void update_key_type( huginn::HThread*, HHuginn::HClass const*, int );
	void clear( void );
	void anchor( huginn::HThread* thread_, int position_ ) const {
		_helper.anchor( thread_, position_ );
	}
	void detach( void ) const {
		_helper.detach();
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	HOrder( HOrder const& ) = delete;
	HOrder& operator = ( HOrder const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HValueHashHelper final {
public:
	typedef int long size_type;
private:
	huginn::HThread* _thread;
	int _position;
public:
	HValueHashHelper( void );
	void anchor( huginn::HThread* thread_, int position_ ) {
		_thread = thread_;
		_position = position_;
	}
	size_type operator()( HHuginn::value_t const& ) const;
	bool operator()( HHuginn::value_t const&, HHuginn::value_t const& ) const;
	void detach( void ) {
		_thread = nullptr;
		_position = 0;
	}
private:
	HValueHashHelper( HValueHashHelper const& ) = delete;
	HValueHashHelper& operator = ( HValueHashHelper const& ) = delete;
};

class HHuginn::HLookup : public HHuginn::HInvalidatingIterable {
public:
	typedef HHuginn::HLookup this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HHashMap<HHuginn::value_t, HHuginn::value_t, HValueHashHelper&, HValueHashHelper&> values_prototype_t;
	typedef yaal::hcore::HPool<values_prototype_t::node_size::value> pool_t;
	typedef allocator::shared_pool<values_prototype_t::node_type> allocator_t;
	typedef yaal::hcore::HHashMap<HHuginn::value_t, HHuginn::value_t, HValueHashHelper&, HValueHashHelper&, allocator_t> values_t;
private:
	mutable HValueHashHelper _helper;
	values_t _data;
public:
	HLookup( HHuginn::HClass const*, allocator_t const& );
	void insert( huginn::HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
	bool has_key( huginn::HThread*, HHuginn::value_t const&, int ) const;
	void erase( huginn::HThread*, HHuginn::value_t const&, int );
	value_t get( huginn::HThread*, HHuginn::value_t const&, int );
	value_t& get_ref( huginn::HThread*, HHuginn::value_t const&, int );
	bool try_get( huginn::HThread*, HHuginn::value_t const&, HHuginn::value_t&, int );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
	void update( huginn::HThread*, HHuginn::value_t const&, int );
	void clear( void );
	void anchor( huginn::HThread* thread_, int position_ ) const {
		_helper.anchor( thread_, position_ );
	}
	void detach( void ) const {
		_helper.detach();
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	HLookup( HLookup const& ) = delete;
	HLookup& operator = ( HLookup const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HSet : public HHuginn::HInvalidatingIterable {
public:
	typedef HHuginn::HSet this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HHashSet<HHuginn::value_t, HValueHashHelper&, HValueHashHelper&> values_prototype_t;
	typedef yaal::hcore::HPool<values_prototype_t::node_size::value> pool_t;
	typedef allocator::shared_pool<values_prototype_t::node_type> allocator_t;
	typedef yaal::hcore::HHashSet<HHuginn::value_t, HValueHashHelper&, HValueHashHelper&, allocator_t> values_t;
private:
	mutable HValueHashHelper _helper;
	values_t _data;
public:
	HSet( HHuginn::HClass const*, allocator_t const& );
	void insert( huginn::HThread*, HHuginn::value_t const&, int );
	bool has_key( huginn::HThread*, HHuginn::value_t const&, int ) const;
	void erase( huginn::HThread*, HHuginn::value_t const&, int );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
	void clear( void );
	void anchor( huginn::HThread* thread_, int position_ ) const {
		_helper.anchor( thread_, position_ );
	}
	void detach( void ) const {
		_helper.detach();
	}
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	HSet( HSet const& ) = delete;
	HSet& operator = ( HSet const& ) = delete;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HBlob : public HHuginn::HValue {
public:
	typedef HHuginn::HBlob this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HChunk value_type;
private:
	value_type _data;
public:
	HBlob( HHuginn::HClass const*, yaal::hcore::HChunk&& );
	int long get_size( void ) const;
	value_type const& value( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HFunctionReference : public HHuginn::HValue {
	typedef HHuginn::HFunctionReference this_type;
	typedef HHuginn::HValue base_type;
private:
	HHuginn::identifier_id_t _identifierId;
	/*! \brief Function definition holder.
	 *
	 * It has to be a ref count bumping pointer because lambdas are stored
	 * with help of HFunctionReference directly in run-time,
	 * so this holder must have shared ownership.
	 */
	HHuginn::function_t _function;
	yaal::hcore::HString _doc;
public:
	HFunctionReference( HHuginn::HClass const*, identifier_id_t, HHuginn::function_t const&, yaal::hcore::HString const& );
	identifier_id_t const& identifier_id( void ) const {
		return ( _identifierId );
	}
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
	void reset( function_t const& );
	yaal::hcore::HString const& doc( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HException : public HHuginn::HValue {
public:
	typedef HHuginn::HException this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _message;
	HIntrospecteeInterface::call_stack_t _callStack;
public:
	HException( huginn::HThread*, HHuginn::HClass const*, yaal::hcore::HString const& );
	HException( HHuginn::HClass const*, yaal::hcore::HString const&, HIntrospecteeInterface::call_stack_t const& );
	yaal::hcore::HString const& what( void ) const;
	yaal::hcore::HString where( void ) const;
	HIntrospecteeInterface::call_stack_t const& trace( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HHuginn::HEnumeral : public HHuginn::HValue {
public:
	typedef HHuginn::HEnumeral this_type;
	typedef HHuginn::HValue base_type;
	typedef int value_type;
private:
	HHuginn::identifier_id_t _identifier;
	value_type _value;
public:
	HEnumeral( HHuginn::HClass const*, identifier_id_t, value_type );
	HHuginn::identifier_id_t const& identifier( void ) const {
		return ( _identifier );
	}
	value_type value( void ) const {
		return ( _value );
	}
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

namespace huginn {

inline HHuginn::type_id_t type_id( HHuginn::TYPE typeTag_ ) {
	return ( HHuginn::type_id_t( static_cast<HHuginn::type_id_t::value_type>( typeTag_ ) ) );
}
inline HHuginn::TYPE type_tag( HHuginn::type_id_t const& typeId_ ) {
	return ( static_cast<HHuginn::TYPE>( typeId_.get() ) );
}
yaal::hcore::HString const& type_name( HHuginn::TYPE );
inline yaal::hcore::HString const& type_name( HHuginn::type_id_t const& type_ ) {
	return ( type_name( type_tag( type_ ) ) );
}

void initialize_packages( void );
void cleanup_packages( void );

}

inline bool operator == ( HHuginn::TYPE t1_, HHuginn::type_id_t const& t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t1_ ) == t2_.get() );
}
inline bool operator == ( HHuginn::type_id_t const& t1_, HHuginn::TYPE t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t2_ ) == t1_.get() );
}
inline bool operator != ( HHuginn::TYPE t1_, HHuginn::type_id_t const& t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t1_ ) != t2_.get() );
}
inline bool operator != ( HHuginn::type_id_t const& t1_, HHuginn::TYPE t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t2_ ) != t1_.get() );
}

yaal::hcore::HString to_string( HHuginn::value_t const& );
yaal::hcore::HString to_string( HHuginn::value_t const&, HHuginn* );

}

}

#endif /* #ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED */

