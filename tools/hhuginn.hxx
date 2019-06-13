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
class HExpression;
class HScope;
/*! \brief ObjectFactory holds built-in type definitions local to HHuginn instance.
 */
struct OCompiler;
class HRuntime;
class HPackageFactory;
class HClass;
class HValue;
class HIterable;

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
	typedef yaal::hcore::HPointer<huginn::HIterable> iterable_t;
	typedef yaal::hcore::HPointer<huginn::HScope> scope_t;
	typedef yaal::hcore::HPointer<huginn::HClass> class_t;
	typedef yaal::tools::huginn::HValueReference<huginn::HValue> value_t;
	typedef yaal::tools::huginn::HValueReferenceObserver<huginn::HValue> value_ref_t;
	typedef yaal::hcore::HPointer<huginn::HExpression> expression_t;
	typedef yaal::hcore::HArray<HHuginn::expression_t> expressions_t;
	class HCoordinate;
	class HCallSite;
	typedef yaal::hcore::HArray<HCallSite> call_stack_t;
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
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const OPTIMIZE;
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
		PACKAGE,
		HIDDEN
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
	call_stack_t _trace;
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
	HHuginn( compiler_setup_t = COMPILER::DEFAULT );
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
	call_stack_t const& trace( void ) const;
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
	executing_parser::HRule make_engine( huginn::HRuntime* = nullptr, compiler_setup_t = COMPILER::DEFAULT );
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
	value_t value( yaal::hcore::HNumber&& );
	value_t value( yaal::hcore::HString const& );
	value_t value( yaal::hcore::HString&& );
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
	void finalize( void );
private:
	HHuginn( huginn::HRuntime* );
	huginn::HRuntime const& runtime( void ) const;
	void finalize_compilation( void );
	huginn::HClass const* commit_class( identifier_id_t );
	huginn::HSource& current_source( void );
	huginn::HSource const& current_source( void ) const;
	char const* error_message( int ) const;
	void consume_error( HHuginnRuntimeException const&, int );
	HHuginn( HHuginn const& ) = delete;
	HHuginn& operator = ( HHuginn const& ) = delete;
	friend yaal::hcore::HString to_string( HHuginn::value_t const&, HHuginn* );
	friend class huginn::HPackageFactory;
};

typedef yaal::hcore::HExceptionT<HHuginn> HHuginnException;

class HIntrospecteeInterface {
public:
	class HVariableView {
		yaal::hcore::HString _name;
		HHuginn::value_ref_t _view;
	public:
		HVariableView( yaal::hcore::HString const&, HHuginn::value_t const& );
		yaal::hcore::HString const& name( void ) const;
		HHuginn::value_t value( void ) const;
	};
	typedef yaal::hcore::HArray<HVariableView> variable_views_t;
public:
	virtual ~HIntrospecteeInterface( void ) {}
	HHuginn::call_stack_t get_call_stack( void );
	variable_views_t get_locals( int );
protected:
	virtual HHuginn::call_stack_t do_get_call_stack( void ) = 0;
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

class HHuginn::HCallSite {
	yaal::hcore::HString _file;
	HHuginn::HCoordinate _coordinate;
	yaal::hcore::HString _context;
public:
	HCallSite( yaal::hcore::HString const&, int, int, yaal::hcore::HString const& );
	yaal::hcore::HString const& file( void ) const;
	int line( void ) const;
	int column( void ) const;
	yaal::hcore::HString const& context( void ) const;
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

