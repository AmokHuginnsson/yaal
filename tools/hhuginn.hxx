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

namespace tools {

namespace huginn {

class HSource;
class HFrame;
class HThread;
class HExpression;
class HScope;
class HFunction;
class HObjectFactory;
struct OCompiler;

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
	class HType {
	public:
		typedef HType this_type;
		typedef HType const* type_t;
		typedef yaal::hcore::HResource<HType const> type_holder_t;
		typedef yaal::hcore::HTaggedPOD<int, HType> id_t;
		typedef std::atomic<id_t::value_type> id_generator_t;
		typedef yaal::hcore::HHashMap<yaal::hcore::HString, type_holder_t> type_dict_t;
	private:
		yaal::hcore::HString _name;
		id_t _id;
		static id_generator_t _idGenerator;
		static type_dict_t _builtin;
	public:
		yaal::hcore::HString const& name( void ) const {
			return ( _name );
		}
		id_t id( void ) const {
			return ( _id );
		}
		static type_t register_type( yaal::hcore::HString const&, HHuginn* );
		static int builtin_type_count( void );
	private:
		HType( yaal::hcore::HString const&, int );
		HType( HType const& ) = delete;
		HType& operator = ( HType const& ) = delete;
		HType( HType&& ) = delete;
		HType& operator = ( HType&& ) = delete;
	};
	typedef HType::type_t type_t;
	typedef yaal::hcore::HPointer<HHuginn> ptr_t;
	class HIterable;
	typedef yaal::hcore::HPointer<HIterable> iterable_t;
	typedef yaal::hcore::HPointer<huginn::HScope> scope_t;
	class HClass;
	typedef yaal::hcore::HPointer<HClass> class_t;
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
	typedef yaal::hcore::HArray<value_t> values_t;
	typedef yaal::hcore::HPointer<huginn::HFrame> frame_t;
	typedef yaal::hcore::HBoundCall<value_t ( huginn::HThread*, HObject*, values_t const&, int )> function_t;
	typedef yaal::hcore::HPointer<huginn::HThread> thread_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HThread::id_t, thread_t> threads_t;
	struct TYPE {
		static type_t const NONE;
		static type_t const BOOLEAN;
		static type_t const INTEGER;
		static type_t const REAL;
		static type_t const STRING;
		static type_t const NUMBER;
		static type_t const CHARACTER;
		static type_t const LIST;
		static type_t const DEQUE;
		static type_t const DICT;
		static type_t const ORDER;
		static type_t const LOOKUP;
		static type_t const SET;
		static type_t const REFERENCE;
		static type_t const FUNCTION_REFERENCE;
		static type_t const OBJECT_REFERENCE;
		static type_t const METHOD;
		static type_t const UNKNOWN;
		static type_t const NOT_BOOLEAN;
	};
	class HHuginnRuntimeException;
	typedef yaal::hcore::HResource<huginn::HSource> source_t;
	typedef yaal::hcore::HResource<huginn::OCompiler> compiler_t;
private:
	typedef yaal::hcore::HResource<huginn::HObjectFactory> object_factory_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, class_t> classes_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, value_t> packages_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, function_t> functions_t;
	struct STATE {
		typedef enum {
			EMPTY,
			LOADED,
			PREPROCESSED,
			PARSED,
			COMPILED
		} state_t;
	};
	STATE::state_t _state;
	HType::id_generator_t _idGenerator;
	HType::type_dict_t _userTypeDict;
	value_t _none;
	value_t _true;
	value_t _false;
	classes_t _classes;
	functions_t _functions;
	object_factory_t _objectFactory;
	source_t _source;
	compiler_t _compiler;
	HExecutingParser _engine;
	threads_t _threads;
	packages_t _packages;
	list_t _argv;
	value_t _result;
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
	 * \return True iff compilation finished with no errors.
	 */
	bool compile( void );

	/*! \brief Execute compiled program.
	 *
	 * \return True iff execution finished with no errors.
	 */
	bool execute( void );

	/*! \brief Call (execute) Huginn function.
	 *
	 * \param name_ - name of the function to call.
	 * \param argv_ - array of arguments that shall be passed to function call.
	 * \param position_ - call context information for error reporting.
	 * \return Result returned by called Huginn function.
	 */
	value_t call( yaal::hcore::HString const& name_, values_t const& argv_, int posiiton_ );

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
	huginn::HThread* current_thread( void );
	huginn::HFrame* current_frame( void );
	void create_function( executing_parser::position_t );
	void create_class( executing_parser::position_t );

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
	function_t get_function( yaal::hcore::HString const& );
	class_t get_class( yaal::hcore::HString const& );
	value_t get_package( yaal::hcore::HString const& );
	yaal::hcore::HString get_snippet( int, int ) const;
	void register_class( class_t );
	static void disable_grammar_verification( void );
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
private:
	void finalize_compilation( void );
	HClass const* commit_class( yaal::hcore::HString const& );
	void register_builtins( void );
	char const* error_message( int ) const;
	friend type_t HType::register_type( yaal::hcore::HString const&, HHuginn* );
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

class HHuginn::HValue {
public:
	typedef HHuginn::HValue this_type;
private:
	type_t const _type;
public:
	HValue( type_t );
	virtual ~HValue( void ) {
		return;
	}
	type_t type( void ) const;
	value_t clone( HHuginn* ) const;
private:
	virtual value_t do_clone( HHuginn* ) const;
	HValue( HValue const& ) = delete;
	HValue& operator = ( HValue const& ) = delete;
};

class HHuginn::HClass {
public:
	typedef HHuginn::HClass this_type;
	typedef yaal::hcore::HArray<yaal::hcore::HString> field_names_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, int> field_indexes_t;
	class HMethod;
	class HBoundMethod;
private:
	type_t _type;
	HClass const* _super;
	field_names_t _fieldNames;
	field_indexes_t _fieldIndexes;
	values_t _fieldDefinitions;
	HHuginn* _huginn;
public:
	HClass( HHuginn*, type_t, HClass const*, field_names_t const&, values_t const& );
	HClass const* super( void ) const;
	virtual ~HClass( void ) {
	}
	yaal::hcore::HString const& name( void ) const;
	type_t type( void ) const;
	field_names_t const& field_names( void ) const;
	int field_index( yaal::hcore::HString const& ) const;
	values_t get_defaults( void ) const;
	function_t const& function( int ) const;
	bool is_kind_of( yaal::hcore::HString const& ) const;
	HHuginn* huginn( void ) const;
	value_t create_instance( huginn::HThread*, HObject*, values_t const&, int ) const;
private:
	virtual value_t do_create_instance( huginn::HThread*, values_t const&, int ) const;
	HClass( HClass const& ) = delete;
	HClass& operator = ( HClass const& ) = delete;
};

class HHuginn::HClass::HMethod : public HHuginn::HValue {
	typedef HHuginn::HClass::HMethod this_type;
	typedef HHuginn::HValue base_type;
private:
	HHuginn::function_t _function;
	HObject* _object;
public:
	HMethod( HHuginn::function_t const& );
	HHuginn::function_t const& function( void ) const;
	HObject* object( void ) const;
	void set_object( HObject* );
private:
	HMethod( HMethod const& ) = delete;
	HMethod& operator = ( HMethod const& ) = delete;
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HClass::HBoundMethod : public HHuginn::HClass::HMethod {
	typedef HHuginn::HClass::HBoundMethod this_type;
	typedef HHuginn::HClass::HMethod base_type;
private:
	HHuginn::value_t _objectHolder;
public:
	HBoundMethod( HMethod const& );
	void set_object( HObject* ) = delete;
private:
	HBoundMethod( HBoundMethod const& ) = delete;
	HBoundMethod& operator = ( HBoundMethod const& ) = delete;
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HObject : public HHuginn::HValue, public yaal::hcore::HPointerFromThisInterface<HHuginn::HObject> {
public:
	typedef HHuginn::HObject this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HArray<value_t> fields_t;
private:
	HClass const* _class;
	fields_t _fields;
public:
	HObject( HClass const* );
	HObject( HClass const*, fields_t const& );
	virtual ~HObject( void );
	int field_index( yaal::hcore::HString const& ) const;
	value_t& field_ref( int );
	value_t field( int, bool ) const;
	HClass const* get_class( void ) const;
	bool is_kind_of( yaal::hcore::HString const& ) const;
	HHuginn::value_t call_method( huginn::HThread*, yaal::hcore::HString const&, HHuginn::values_t const&, int ) const;
private:
	void reset_methods( void );
	HObject( HObject const& ) = delete;
	HObject& operator = ( HObject const& ) = delete;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

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
	int field_index( yaal::hcore::HString const& ) const;
	value_t field( int, bool );
private:
	HObjectReference( HObjectReference const& ) = delete;
	HObjectReference& operator = ( HObjectReference const& ) = delete;
	virtual value_t do_clone( HHuginn* ) const override;
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
	virtual value_t do_clone( HHuginn* ) const override;
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
	virtual value_t do_clone( HHuginn* ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

class HHuginn::HIterable : public HHuginn::HObject {
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
private:
	bool _value;
public:
	HBoolean( bool );
	bool value( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HInteger : public HHuginn::HValue {
public:
	typedef HHuginn::HInteger this_type;
	typedef HHuginn::HValue base_type;
private:
	int long long _value;
public:
	HInteger( int long long );
	int long long value( void ) const;
	void to_character( void ) const;
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HReal : public HHuginn::HValue {
public:
	typedef HHuginn::HReal this_type;
	typedef HHuginn::HValue base_type;
private:
	double long _value;
public:
	HReal( double long );
	double long value( void ) const;
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
	/* There is no direct convertion to character. */
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HString : public HHuginn::HIterable {
public:
	typedef HHuginn::HString this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _value;
public:
	HString( HHuginn::HClass const*, yaal::hcore::HString const& );
	yaal::hcore::HString const& value( void ) const;
	yaal::hcore::HString& value( void );
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HCharacter : public HHuginn::HValue {
public:
	typedef HHuginn::HCharacter this_type;
	typedef HHuginn::HValue base_type;
private:
	char _value;
public:
	HCharacter( char );
	char value( void ) const;
	void to_character( void ) const;
	void to_integer( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HNumber : public HHuginn::HValue {
public:
	typedef HHuginn::HNumber this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HNumber _value;
public:
	HNumber( yaal::hcore::HNumber const& );
	yaal::hcore::HNumber const& value( void ) const;
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
private:
	virtual value_t do_clone( HHuginn* ) const override;
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
	value_t get_ref( int long long );
	values_t const& value( void ) const;
	values_t& value( void );
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HDeque : public HHuginn::HIterable {
public:
	typedef HHuginn::HDeque this_type;
	typedef HHuginn::HIterable base_type;
	typedef yaal::hcore::HDeque<HHuginn::value_t> values_t;
private:
	values_t _data;
public:
	HDeque( void );
	HDeque( values_t const& );
	void push_back( value_t const& );
	void pop_back( void );
	void push_front( value_t const& );
	void pop_front( void );
	void clear( void );
	value_t get( int long long );
	value_t get_ref( int long long );
	values_t const& value( void ) const;
	values_t& value( void );
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HDict : public HHuginn::HIterable {
public:
	typedef HHuginn::HDict this_type;
	typedef HHuginn::HIterable base_type;
	typedef bool (*cmp_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HMap<HHuginn::value_t, HHuginn::value_t, cmp_t> values_t;
private:
	values_t _data;
	type_t _keyType;
public:
	HDict( void );
	HDict( values_t const&, type_t );
	value_t get( HHuginn::value_t const&, int );
	value_t get_ref( HHuginn::value_t const&, int );
	void insert( HHuginn::value_t const&, HHuginn::value_t const&, int );
	bool has_key( HHuginn::value_t const&, int ) const;
	bool try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ );
	void erase( HHuginn::value_t const&, int );
	values_t const& value( void ) const;
	values_t& value( void );
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	void verify_key_type( HHuginn::type_t, int ) const;
	HDict( HDict const& ) = delete;
	HDict& operator = ( HDict const& ) = delete;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HOrder : public HHuginn::HIterable {
public:
	typedef HHuginn::HOrder this_type;
	typedef HHuginn::HIterable base_type;
	typedef bool (*cmp_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HSet<HHuginn::value_t, cmp_t> values_t;
private:
	values_t _data;
	type_t _keyType;
public:
	HOrder( void );
	HOrder( values_t const&, type_t );
	void insert( HHuginn::value_t const&, int );
	bool has_key( HHuginn::value_t const&, int ) const;
	void erase( HHuginn::value_t const&, int );
	values_t const& value( void ) const;
	values_t& value( void );
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	void verify_key_type( HHuginn::type_t, int ) const;
	HOrder( HOrder const& ) = delete;
	HOrder& operator = ( HOrder const& ) = delete;
private:
	virtual value_t do_clone( HHuginn* ) const override;
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
	HLookup( void );
	HLookup( values_t const& );
	void insert( HHuginn::value_t const&, HHuginn::value_t const& );
	bool has_key( HHuginn::value_t const& ) const;
	void erase( HHuginn::value_t const& );
	value_t get( HHuginn::value_t const&, int );
	value_t get_ref( HHuginn::value_t const& );
	bool try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_ );
	values_t const& value( void ) const;
	values_t& value( void );
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	HLookup( HLookup const& ) = delete;
	HLookup& operator = ( HLookup const& ) = delete;
private:
	virtual value_t do_clone( HHuginn* ) const override;
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
	HSet( void );
	HSet( values_t const& );
	void insert( HHuginn::value_t const& );
	bool has_key( HHuginn::value_t const& ) const;
	void erase( HHuginn::value_t const& );
	values_t const& value( void ) const;
	values_t& value( void );
protected:
	virtual HIterator do_iterator( void ) override;
	virtual int long do_size( void ) const override;
private:
	HSet( HSet const& ) = delete;
	HSet& operator = ( HSet const& ) = delete;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HFunctionReference : public HHuginn::HValue {
	typedef HHuginn::HFunctionReference this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _name;
	HHuginn::function_t _function;
public:
	HFunctionReference( yaal::hcore::HString const&, HHuginn::function_t const& );
	yaal::hcore::HString const& name( void ) const;
	HHuginn::function_t const& function( void ) const;
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

class HHuginn::HException : public HHuginn::HObject {
public:
	typedef HHuginn::HException this_type;
	typedef HHuginn::HObject base_type;
private:
	yaal::hcore::HString _message;
	yaal::hcore::HString _where;
public:
	HException( HHuginn::HClass const*, yaal::hcore::HString const& );
	yaal::hcore::HString const& what( void ) const;
	yaal::hcore::HString const& where( void ) const;
	void set_where( yaal::hcore::HString const& );
private:
	virtual value_t do_clone( HHuginn* ) const override;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED */

