/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  tools/hhuginn.hxx - this file is integral part of `yaal' project.

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

namespace yaal {

namespace hcore {

template<typename key_t, typename TAG>
struct hash<hcore::HTaggedPOD<key_t, TAG>> {
	int long operator () ( HTaggedPOD<key_t, TAG> const& key_ ) const {
		return ( hash<key_t>()( key_.get() ) );
	}
};

}

namespace tools {

namespace huginn {

class HSource;
class HFrame;
class HThread;
class HExpression;
class HScope;
class HFunction;
/*! \brief ObjectFactory holds built-in type definitions local to HHuginn instance.
 */
class HObjectFactory;
struct OCompiler;
class HRuntime;

namespace ERR_CODE {
enum {
	OP_NOT_SUM = 0,
	OP_NOT_SUB,
	OP_NOT_MUL,
	OP_NOT_DIV,
	OP_NOT_EXP,
	OP_NOT_CMP,
	OPS_NOT_BOOL,
	OP_NOT_BOOL,
	IDX_NOT_INT
};
}

extern M_YAAL_TOOLS_PUBLIC_API char const* _errMsgHHuginn_[];

}

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
	class HIterable;
	typedef yaal::hcore::HPointer<HIterable> iterable_t;
	typedef yaal::hcore::HPointer<huginn::HScope> scope_t;
	class HClass;
	typedef yaal::hcore::HPointer<HClass> class_t;
	class HObserver;
	class HReference;
	class HFunctionReference;
	class HValue;
	typedef yaal::hcore::HPointer<HValue> value_t;
	class HBoolean;
	class HInteger;
	class HReal;
	class HString;
	class HCharacter;
	class HNumber;
	class HList;
	class HDeque;
	typedef yaal::hcore::HPointer<HList> list_t;
	class HDict;
	class HOrder;
	class HLookup;
	class HSet;
	class HObject;
	class HException;
	class HObjectReference;
	class HTernaryEvaluator;
	typedef yaal::hcore::HPointer<huginn::HExpression> expression_t;
	typedef yaal::hcore::HArray<HHuginn::expression_t> expressions_t;
	class HErrorCoordinate;
	class HFieldDefinition;
	typedef yaal::hcore::HArray<HFieldDefinition> field_definitions_t;
	typedef yaal::hcore::HArray<identifier_id_t> field_identifiers_t;
	typedef yaal::hcore::HArray<value_t> values_t;
	typedef yaal::hcore::HPointer<huginn::HFrame> frame_t;
	typedef yaal::hcore::HBoundCall<value_t ( huginn::HThread*, value_t*, values_t const&, int )> function_t;
	enum class TYPE {
		NONE,
		BOOLEAN,
		INTEGER,
		REAL,
		STRING,
		NUMBER,
		CHARACTER,
		LIST,
		DEQUE,
		DICT,
		ORDER,
		LOOKUP,
		SET,
		OBSERVER,
		REFERENCE,
		FUNCTION_REFERENCE,
		OBJECT_REFERENCE,
		METHOD,
		UNKNOWN,
		NOT_BOOLEAN
	};
	struct COMPILER;
	/*! \brief Bits used for specyfying how compiler should work.
	 */
	typedef yaal::hcore::HBitFlag<COMPILER> compiler_setup_t;
	struct COMPILER {
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const DEFAULT;
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const BE_STRICT;
		static M_YAAL_TOOLS_PUBLIC_API compiler_setup_t const BE_SLOPPY;
	};
	class HHuginnRuntimeException;
	typedef yaal::hcore::HResource<huginn::HSource> source_t;
	typedef yaal::hcore::HResource<huginn::OCompiler> compiler_t;
	typedef yaal::hcore::HResource<huginn::HRuntime> runtime_t;
private:
	enum class STATE {
		EMPTY,
		LOADED,
		PREPROCESSED,
		PARSED,
		COMPILED
	};
	STATE _state;
	runtime_t _runtime;
	source_t _source;
	compiler_t _compiler;
	HExecutingParser _engine;
	yaal::hcore::HString _errorMessage;
	int _errorPosition;
	typedef std::atomic<bool> flag_t;
	static flag_t _grammarVerified;
	yaal::hcore::HStreamInterface::ptr_t _inputStream;
	yaal::hcore::HStreamInterface* _inputStreamRaw;
	yaal::hcore::HStreamInterface::ptr_t _outputStream;
	yaal::hcore::HStreamInterface* _outputStreamRaw;
	yaal::hcore::HStreamInterface::ptr_t _errorStream;
	yaal::hcore::HStreamInterface* _errorStreamRaw;
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
	 * \return True iff compilation finished with no errors.
	 */
	bool compile( compiler_setup_t compilerSetup_ = COMPILER::DEFAULT );

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
	void dump_vm_state( yaal::hcore::HStreamInterface& );

	/*! \brief Add argument for main() function.
	 *
	 * \param value_ - value of argument being added.
	 */
	void add_argument( yaal::hcore::HString const& value_ );

	/*! \brief Remove all argument currently held for main() function.
	 */
	void clear_arguments( void );
	void dump_preprocessed_source( yaal::hcore::HStreamInterface& );
	int error_position( void ) const;
	HErrorCoordinate error_coordinate( void ) const;
	HErrorCoordinate get_coordinate( int ) const;
	yaal::hcore::HString source_name( void ) const;
	yaal::hcore::HString where( int ) const;
	yaal::hcore::HString error_message( void ) const;
	void set_input_stream( yaal::hcore::HStreamInterface& );
	void set_input_stream( yaal::hcore::HStreamInterface::ptr_t );
	void set_output_stream( yaal::hcore::HStreamInterface& );
	void set_output_stream( yaal::hcore::HStreamInterface::ptr_t );
	void set_error_stream( yaal::hcore::HStreamInterface& );
	void set_error_stream( yaal::hcore::HStreamInterface::ptr_t );
	executing_parser::HRule make_engine( void );
	yaal::hcore::HStreamInterface& input_stream( void );
	yaal::hcore::HStreamInterface& output_stream( void );
	yaal::hcore::HStreamInterface& error_stream( void );
	yaal::hcore::HString get_snippet( int, int ) const;
	void register_class( class_t );
	void register_function( identifier_id_t );
	static void disable_grammar_verification( void );
private:
	void finalize_compilation( compiler_setup_t );
	HClass const* commit_class( identifier_id_t );
	char const* error_message( int ) const;
	HHuginn( HHuginn const& ) = delete;
	HHuginn& operator = ( HHuginn const& ) = delete;
};

typedef yaal::hcore::HExceptionT<HHuginn> HHuginnException;

class HHuginn::HErrorCoordinate {
public:
	typedef HErrorCoordinate this_type;
private:
	int _line;
	int _column;
public:
	HErrorCoordinate( int line_, int column_ )
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
	int _position;
public:
	HHuginnRuntimeException( yaal::hcore::HString const&, int );
	yaal::hcore::HString const& message( void ) const;
	int position( void ) const;
};

class HHuginn::HFieldDefinition {
	yaal::hcore::HString _name;
	value_t _value;
public:
	HFieldDefinition( yaal::hcore::HString const& name_, value_t const& value_ )
		: _name( name_ )
		, _value( value_ ) {
		return;
	}
	yaal::hcore::HString const& name( void ) const {
		return ( _name );
	}
	value_t const& value( void ) const {
		return ( _value );
	}
};

class HHuginn::HClass {
public:
	typedef HHuginn::HClass this_type;
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, int> field_indexes_t;
	class HMethod;
	class HBoundMethod;
private:
	type_id_t _typeId;
	identifier_id_t _identifierId;
	HClass const* _super;
	field_identifiers_t _fieldIdentifiers;
	field_indexes_t _fieldIndexes;
	values_t _fieldDefinitions;
	huginn::HRuntime* _runtime;
public:
	HClass( huginn::HRuntime*, type_id_t, identifier_id_t, HClass const*, field_definitions_t const& );
	HClass( HHuginn::TYPE );
	virtual ~HClass( void ) {
	}
	HClass const* super( void ) const {
		return ( _super );
	}
	identifier_id_t identifier_id( void ) const {
		return ( _identifierId );
	}
	yaal::hcore::HString const& name( void ) const;
	type_id_t type_id( void ) const {
		return ( _typeId );
	}
	field_identifiers_t const& field_identifiers( void ) const {
		return ( _fieldIdentifiers );
	}
	int field_index( identifier_id_t ) const;
	value_t const& field( int index_ ) const {
		return ( _fieldDefinitions[index_] );
	}
	values_t get_defaults( void ) const;
	function_t const& function( int ) const;
	bool is_kind_of( identifier_id_t ) const;
	bool is_complex( void ) const {
		return ( ! _fieldDefinitions.is_empty() );
	}
	huginn::HRuntime* runtime( void ) const {
		return ( _runtime );
	}
	value_t create_instance( huginn::HThread*, value_t*, values_t const&, int ) const;
private:
	virtual value_t do_create_instance( huginn::HThread*, values_t const&, int ) const;
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
	type_id_t type_id( void ) const {
		return ( _class->type_id() );
	}
	HClass const* get_class( void ) const {
		return ( _class );
	}
	value_t clone( huginn::HRuntime* ) const;
	int field_index( identifier_id_t ) const;
	value_t field( HHuginn::value_t const& subject_, int index_ ) const {
		return ( do_field( subject_, index_ ) );
	}
private:
	virtual value_t do_field( HHuginn::value_t const&, int ) const;
	virtual value_t do_clone( huginn::HRuntime* ) const;
	HValue( HValue const& ) = delete;
	HValue& operator = ( HValue const& ) = delete;
};

class HHuginn::HClass::HMethod : public HHuginn::HValue {
	typedef HHuginn::HClass::HMethod this_type;
	typedef HHuginn::HValue base_type;
private:
	HHuginn::function_t _function;
public:
	HMethod( HHuginn::function_t const& );
	HHuginn::function_t const& function( void ) const;
private:
	HMethod( HMethod const& ) = delete;
	HMethod& operator = ( HMethod const& ) = delete;
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HClass::HBoundMethod : public HHuginn::HClass::HMethod {
	typedef HHuginn::HClass::HBoundMethod this_type;
	typedef HHuginn::HClass::HMethod base_type;
private:
	HHuginn::value_t _objectHolder;
public:
	HBoundMethod( HMethod const&, HHuginn::value_t const& );
	HHuginn::value_t* object( void );
private:
	HBoundMethod( HBoundMethod const& ) = delete;
	HBoundMethod& operator = ( HBoundMethod const& ) = delete;
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HObject : public HHuginn::HValue {
public:
	typedef HHuginn::HObject this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HArray<value_t> fields_t;
private:
	fields_t _fields;
public:
	HObject( HClass const* );
	HObject( HClass const*, fields_t const& );
	virtual ~HObject( void );
	bool is_kind_of( HHuginn::identifier_id_t ) const;
	value_t& field_ref( int );
	HHuginn::value_t call_method( huginn::HThread*, HHuginn::value_t const&, yaal::hcore::HString const&, HHuginn::values_t const&, int ) const;
private:
	HObject( HObject const& ) = delete;
	HObject& operator = ( HObject const& ) = delete;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
	virtual value_t do_field( HHuginn::value_t const&, int ) const override;
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
	value_t _object;
	HClass const* _class;
public:
	HObjectReference( value_t const&, int, bool, int );
	HObjectReference( value_t const&, HClass const* );
	int field_index( identifier_id_t ) const;
	value_t field( int );
private:
	HObjectReference( HObjectReference const& ) = delete;
	HObjectReference& operator = ( HObjectReference const& ) = delete;
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HObserver : public HHuginn::HValue {
public:
	typedef HHuginn::HObserver this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HPointerObserver<HHuginn::HValue> value_type;
private:
	value_type _value;
public:
	HObserver( HHuginn::value_t const& );
	HHuginn::value_t value( void ) const;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HReference : public HHuginn::HValue {
public:
	typedef HHuginn::HReference this_type;
	typedef HHuginn::HValue base_type;
private:
	HHuginn::value_t& _value;
public:
	HReference( HHuginn::value_t& );
	HHuginn::value_t& value( void ) const;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
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
	virtual value_t do_clone( huginn::HRuntime* ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

class HHuginn::HIterable : public HHuginn::HValue {
public:
	typedef HHuginn::HIterable this_type;
	typedef HHuginn::HValue base_type;
	class HIterator;
	HIterable( HClass const* );
	HIterator iterator( void );
	int long size( void ) const;
protected:
	virtual HIterator do_iterator( void ) = 0;
	virtual int long do_size( void ) const = 0;
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
	void to_string( void ) const;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
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
	void to_character( void ) const;
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
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
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
	/* There is no direct conversion to character. */
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
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
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HCharacter : public HHuginn::HValue {
public:
	typedef HHuginn::HCharacter this_type;
	typedef HHuginn::HValue base_type;
	typedef char value_type;
private:
	value_type _value;
public:
	HCharacter( HHuginn::HClass const*, value_type );
	value_type value( void ) const {
		return ( _value );
	}
	void set( value_type );
	void to_character( void ) const;
	void to_integer( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
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
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HList : public HHuginn::HIterable {
public:
	typedef HHuginn::HList this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HArray<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HList( HHuginn::HClass const* );
	HList( HHuginn::HClass const*, values_t const& );
	void push_back( value_t const& );
	void pop_back( void );
	void clear( void );
	value_t get( int long long );
	value_t& get_ref( int long long );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HDeque : public HHuginn::HIterable {
public:
	typedef HHuginn::HDeque this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HDeque<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HDeque( HHuginn::HClass const* );
	HDeque( HHuginn::HClass const*, values_t const& );
	void push_back( value_t const& );
	void pop_back( void );
	void push_front( value_t const& );
	void pop_front( void );
	void clear( void );
	value_t get( int long long );
	value_t& get_ref( int long long );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HDict : public HHuginn::HIterable {
public:
	typedef HHuginn::HDict this_type;
	typedef HHuginn::HIterable base_type;
	typedef bool (*cmp_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HMap<HHuginn::value_t, HHuginn::value_t, cmp_t> values_t;
private:
	values_t _data;
	HHuginn::HClass const* _keyType;
public:
	HDict( HHuginn::HClass const* );
	HDict( HHuginn::HClass const*, values_t const&, HHuginn::HClass const* );
	value_t get( HHuginn::value_t const&, int );
	value_t& get_ref( HHuginn::value_t const&, int );
	void insert( HHuginn::value_t const&, HHuginn::value_t const&, int );
	bool has_key( HHuginn::value_t const&, int ) const;
	bool try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ );
	void erase( HHuginn::value_t const&, int );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	void verify_key_type( HHuginn::HClass const*, int ) const;
	HDict( HDict const& ) = delete;
	HDict& operator = ( HDict const& ) = delete;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HOrder : public HHuginn::HIterable {
public:
	typedef HHuginn::HOrder this_type;
	typedef HHuginn::HIterable base_type;
	typedef bool (*cmp_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HSet<HHuginn::value_t, cmp_t> values_t;
private:
	values_t _data;
	HHuginn::HClass const* _keyType;
public:
	HOrder( HHuginn::HClass const* );
	HOrder( HHuginn::HClass const*, values_t const&, HHuginn::HClass const* );
	void insert( HHuginn::value_t const&, int );
	bool has_key( HHuginn::value_t const&, int ) const;
	void erase( HHuginn::value_t const&, int );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	void verify_key_type( HHuginn::HClass const*, int ) const;
	HOrder( HOrder const& ) = delete;
	HOrder& operator = ( HOrder const& ) = delete;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HLookup : public HHuginn::HIterable {
public:
	typedef HHuginn::HLookup this_type;
	typedef HHuginn::HIterable base_type;
	typedef int long (*hash_t)( HHuginn::value_t const& );
	typedef bool (*equals_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HHashMap<HHuginn::value_t, HHuginn::value_t, hash_t, equals_t> values_t;
private:
	values_t _data;
public:
	HLookup( HHuginn::HClass const* );
	HLookup( HHuginn::HClass const*, values_t const& );
	void insert( HHuginn::value_t const&, HHuginn::value_t const& );
	bool has_key( HHuginn::value_t const& ) const;
	void erase( HHuginn::value_t const& );
	value_t get( HHuginn::value_t const&, int );
	value_t& get_ref( HHuginn::value_t const& );
	bool try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_ );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	HLookup( HLookup const& ) = delete;
	HLookup& operator = ( HLookup const& ) = delete;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HSet : public HHuginn::HIterable {
public:
	typedef HHuginn::HSet this_type;
	typedef HHuginn::HIterable base_type;
	typedef int long (*hash_t)( HHuginn::value_t const& );
	typedef bool (*equals_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HHashSet<HHuginn::value_t, hash_t, equals_t> values_t;
private:
	values_t _data;
public:
	HSet( HHuginn::HClass const* );
	HSet( HHuginn::HClass const*, values_t const& );
	void insert( HHuginn::value_t const& );
	bool has_key( HHuginn::value_t const& ) const;
	void erase( HHuginn::value_t const& );
	values_t const& value( void ) const {
		return ( _data );
	}
	values_t& value( void ) {
		return ( _data );
	}
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	HSet( HSet const& ) = delete;
	HSet& operator = ( HSet const& ) = delete;
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
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
public:
	HFunctionReference( identifier_id_t, HHuginn::function_t const& );
	identifier_id_t identifier_id( void ) const {
		return ( _identifierId );
	}
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

class HHuginn::HException : public HHuginn::HValue {
public:
	typedef HHuginn::HException this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _message;
	yaal::hcore::HString _where;
public:
	HException( HHuginn::HClass const*, yaal::hcore::HString const& );
	yaal::hcore::HString const& what( void ) const;
	yaal::hcore::HString const& where( void ) const;
	void set_where( yaal::hcore::HString const& );
private:
	virtual value_t do_clone( huginn::HRuntime* ) const override;
};

namespace huginn {

extern HHuginn::HClass const _noneClass_;

inline HHuginn::type_id_t type_id( HHuginn::TYPE type_ ) {
	return ( HHuginn::type_id_t( static_cast<HHuginn::type_id_t::value_type>( type_ ) ) );
}
yaal::hcore::HString const& type_name( HHuginn::TYPE );
inline yaal::hcore::HString const& type_name( HHuginn::type_id_t type_ ) {
	return ( type_name( static_cast<HHuginn::TYPE>( type_.get() ) ) );
}

}

inline bool operator == ( HHuginn::TYPE t1_, HHuginn::type_id_t t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t1_ ) == t2_.get() );
}
inline bool operator == ( HHuginn::type_id_t t1_, HHuginn::TYPE t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t2_ ) == t1_.get() );
}
inline bool operator != ( HHuginn::TYPE t1_, HHuginn::type_id_t t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t1_ ) != t2_.get() );
}
inline bool operator != ( HHuginn::type_id_t t1_, HHuginn::TYPE t2_ ) {
	return ( static_cast<HHuginn::type_id_t::value_type>( t2_ ) != t1_.get() );
}

yaal::hcore::HString to_string( HHuginn::value_t const& );

}

}

#endif /* #ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED */

