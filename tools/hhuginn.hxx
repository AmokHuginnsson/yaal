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
#include "hcore/hhashmap.hxx"
#include "hcore/hstack.hxx"
#include "hcore/hstreaminterface.hxx"
#include "hcore/hthread.hxx"
#include "tools/executingparser.hxx"

namespace yaal {

namespace tools {

/*! \brief Huginn programming language implementation.
 */
class HHuginn {
public:
	typedef HHuginn this_type;
	typedef yaal::hcore::HPointer<HHuginn> ptr_t;
	class HObject;
	typedef yaal::hcore::HPointer<HObject> object_t;
	class HIterable;
	typedef yaal::hcore::HPointer<HIterable> iterable_t;
	class HStatement;
	typedef yaal::hcore::HPointer<HStatement> statement_t;
	class HScope;
	typedef yaal::hcore::HPointer<HScope> scope_t;
	class HIf;
	class HWhile;
	class HFor;
	class HSwitch;
	class HBreak;
	class HReturn;
	class HClass;
	class HMethod;
	class HFunction;
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
	typedef yaal::hcore::HPointer<HList> list_t;
	class HMap;
	class HExpression;
	class HBooleanEvaluator;
	typedef yaal::hcore::HPointer<HExpression> expression_t;
	class HErrorCoordinate;
	typedef yaal::hcore::HArray<statement_t> statement_list_t;
	typedef yaal::hcore::HArray<value_t> values_t;
	class HFrame;
	typedef yaal::hcore::HPointer<HFrame> frame_t;
	class HThread;
	typedef yaal::hcore::HBoundCall<value_t ( HThread*, values_t const&, int )> function_t;
	typedef yaal::hcore::HPointer<HThread> thread_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HThread::id_t, thread_t> threads_t;
	enum class OPERATOR {
		PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,
		POWER,
		ABSOLUTE,
		PARENTHESIS,
		ASSIGN,
		SUBSCRIPT,
		NEGATE,
		FUNCTION_CALL,
		FUNCTION_ARGUMENT,
		EQUALS,
		NOT_EQUALS,
		LESS,
		GREATER,
		LESS_OR_EQUAL,
		GREATER_OR_EQUAL,
		BOOLEAN_AND,
		BOOLEAN_OR,
		BOOLEAN_XOR,
		BOOLEAN_NOT,
		NONE
	};
	enum class TYPE {
		NONE,
		BOOLEAN,
		INTEGER,
		REAL,
		STRING,
		NUMBER,
		CHARACTER,
		LIST,
		MAP,
		REFERENCE,
		FUNCTION_REFERENCE,
		UNKNOWN,
		NOT_BOOLEAN
	};
	struct OPositionedOperator {
		OPERATOR _operator;
		int _position;
		OPositionedOperator( OPERATOR operator_, int position_ )
			: _operator( operator_ ), _position( position_ ) {
			return;
		}
	};
	typedef yaal::hcore::HStack<OPositionedOperator> operations_t;
	class HHuginnRuntimeException;
private:
	class HSource {
		typedef HSource this_type;
		typedef yaal::hcore::HMap<int, int> skips_t;
		yaal::hcore::HString _name;
		yaal::hcore::HChunk _orig;
		int _origSize;
		yaal::hcore::HChunk _preprocessed;
		int _preprocessedSize;
		skips_t _skips;
	public:
		HSource( void );
		void load( yaal::hcore::HStreamInterface& );
		void preprocess( void );
		char const* error_message( int ) const;
		int error_position( int ) const;
		HHuginn::HErrorCoordinate error_coordinate( int ) const;
		yaal::hcore::HString const& name( void ) const;
		yaal::hcore::HString::const_iterator begin( void ) const;
		yaal::hcore::HString::const_iterator end( void ) const;
		void dump_preprocessed( yaal::hcore::HStreamInterface& ) const;
	};
	struct OCompiler {
		typedef void ( HHuginn::HExpression::* expression_action_t ) ( HFrame*, int );
		typedef yaal::hcore::HArray<yaal::hcore::HString> parameter_names_t;
		typedef yaal::hcore::HArray<expression_t> expressions_t;
		typedef yaal::hcore::HStack<expressions_t> expressions_stack_t;
		struct OContext {
			HHuginn* _huginn;
			scope_t _scope;
			expressions_stack_t _expressionsStack;
			OContext( HHuginn* );
			OContext( HHuginn*, scope_t const&, expression_t const& );
			expression_t const& expression( void ) const;
			expression_t& expression( void );
			void clear( void );
			OContext( OContext const& ) = default;
			OContext& operator = ( OContext const& ) = default;
		};
		struct OCompilationFrame {
			typedef yaal::hcore::HArray<OContext> contexts_t;
			OContext _context;
			contexts_t _contextsChain;
			scope_t _else;
			yaal::hcore::HString _forIdentifier;
			int _forPosition;
			OCompilationFrame( HHuginn* );
			void clear( void );
		};
		typedef yaal::hcore::HStack<OCompilationFrame> compilation_stack_t;
		typedef yaal::hcore::HStack<TYPE> type_stack_t;
		HHuginn* _huginn;
		yaal::hcore::HString _functionName;
		parameter_names_t _parameters;
		expressions_t _defaultValues;
		int _lastDefaultValuePosition;
		compilation_stack_t _compilationStack;
		operations_t _operations;
		type_stack_t _valueTypes;
		OPERATOR _lastDereferenceOperator;
		OCompiler( HHuginn* );
		void set_function_name( yaal::hcore::HString const&, executing_parser::position_t );
		void set_for_identifier( yaal::hcore::HString const&, executing_parser::position_t );
		void add_paramater( yaal::hcore::HString const&, executing_parser::position_t );
		void verify_default_argument( executing_parser::position_t );
		static bool is_numeric( TYPE );
		static bool is_numeric_congruent( TYPE );
		static bool is_summable( TYPE );
		static bool is_comparable( TYPE );
		static bool is_comparable_congruent( TYPE );
		static bool is_boolean_congruent( TYPE );
		static bool is_unknown( TYPE );
		static bool is_reference_congruent( TYPE );
		static bool is_integer_congruent( TYPE );
		static bool are_congruous( TYPE, TYPE );
		static TYPE congruent( TYPE, TYPE );
		scope_t& current_scope( void );
		expression_t& current_expression( void );
		void reset_expression( void );
		void start_subexpression( executing_parser::position_t );
		void add_subexpression( OPERATOR, executing_parser::position_t );
		void commit_boolean( OPERATOR, executing_parser::position_t );
		void create_scope( executing_parser::position_t );
		void commit_scope( executing_parser::position_t );
		void commit_if_clause( executing_parser::position_t );
		void commit_else_clause( executing_parser::position_t );
		void add_return_statement( executing_parser::position_t );
		void add_break_statement( executing_parser::position_t );
		void add_while_statement( executing_parser::position_t );
		void add_for_statement( executing_parser::position_t );
		void add_if_statement( executing_parser::position_t );
		void add_switch_statement( executing_parser::position_t );
		void add_default_value( executing_parser::position_t );
		void commit_expression( executing_parser::position_t );
		void mark_expression_position( executing_parser::position_t );
		void make_reference( executing_parser::position_t );
		void defer_get_reference( yaal::hcore::HString const&, executing_parser::position_t );
		void defer_make_variable( yaal::hcore::HString const&, executing_parser::position_t );
		void defer_oper( char, executing_parser::position_t );
		void defer_str_oper( yaal::hcore::HString const&, executing_parser::position_t );
		void defer_oper_direct( OPERATOR, executing_parser::position_t );
		void dispatch_action( OPERATOR, executing_parser::position_t );
		void dispatch_plus( executing_parser::position_t );
		void dispatch_mul( executing_parser::position_t );
		void dispatch_power( executing_parser::position_t );
		void dispatch_compare( executing_parser::position_t );
		void dispatch_equals( executing_parser::position_t );
		void dispatch_boolean( expression_action_t const&, executing_parser::position_t );
		void dispatch_assign( executing_parser::position_t );
		void dispatch_subscript( executing_parser::position_t );
		void dispatch_function_call( executing_parser::position_t );
		void defer_action( expression_action_t const&, executing_parser::position_t );
		void defer_store_direct( value_t const&, executing_parser::position_t );
		void defer_store_real( double long, executing_parser::position_t );
		void defer_store_integer( int long long, executing_parser::position_t );
		void defer_store_string( yaal::hcore::HString const&, executing_parser::position_t );
		void defer_store_number( yaal::hcore::HString const&, executing_parser::position_t );
		void defer_store_character( char, executing_parser::position_t );
		void defer_store_boolean( bool, executing_parser::position_t );
		void defer_store_none( executing_parser::position_t );
		void defer_make_list( executing_parser::position_t );
	private:
		OCompiler( OCompiler const& ) = delete;
		OCompiler& operator = ( OCompiler const& ) = delete;
	};
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
	functions_t _functions;
	HExecutingParser _engine;
	HSource _source;
	OCompiler _compiler;
	threads_t _threads;
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
	/*! \brief Store source in internal buffer.
	 *
	 * \param  stream_ - stream that contains program source.
	 */
	void load( yaal::hcore::HStreamInterface& stream_ );
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
	/*! \brief Dump Huginn Virtual Machine state.
	 */
	value_t call( yaal::hcore::HString const&, values_t const&, int );
	value_t result( void ) const;
	void dump_vm_state( yaal::hcore::HStreamInterface& );
	HFrame* current_frame( void );
	void create_function( executing_parser::position_t );
	void add_argument( yaal::hcore::HString const& );
	void dump_preprocessed_source( yaal::hcore::HStreamInterface& );
	int error_position( void ) const;
	HErrorCoordinate error_coordinate( void ) const;
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
private:
	void register_builtins( void );
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

class HHuginn::HObject {
public:
	typedef HHuginn::HObject this_type;
	virtual ~HObject( void ) {}
};

class HHuginn::HStatement : public HHuginn::HObject {
public:
	typedef HHuginn::HStatement this_type;
	typedef HHuginn::HObject base_type;
public:
	HStatement( void );
	virtual ~HStatement( void ) {
		return;
	}
	void execute( HHuginn::HThread* ) const;
protected:
	virtual void do_execute( HHuginn::HThread* ) const {}
};

class HHuginn::HExpression : public HHuginn::HStatement {
public:
	typedef HHuginn::HExpression this_type;
	typedef HHuginn::HStatement base_type;
	enum class SUBSCRIPT {
		VALUE,
		REFERENCE
	};
private:
	typedef yaal::hcore::HBoundCall<void ( HFrame* )> execution_step_t;
	typedef yaal::hcore::HArray<execution_step_t> execution_steps_t;
	execution_steps_t _executionSteps;
	int _position;
	HHuginn* _huginn;
public:
	HExpression( HHuginn*, int = 0 );
	int position( void ) const;
	void set_position( int );
	void add_execution_step( execution_step_t const& );
	void pop_execution_step( void );
	void merge( HExpression& );
	void oper( OPERATOR, HFrame*, int );
	void close_parenthesis( HFrame*, int );
	void plus( HFrame*, int );
	void minus( HFrame*, int );
	void mul( HFrame*, int );
	void div( HFrame*, int );
	void mod( HFrame*, int );
	void negate( HFrame*, int );
	void function_call( HFrame*, int );
	void get_reference( yaal::hcore::HString const&, HFrame*, int );
	void make_variable( yaal::hcore::HString const&, HFrame*, int );
	void set_variable( HFrame*, int );
	void subscript( SUBSCRIPT, HFrame*, int );
	void power( HFrame*, int );
	void equals( HFrame*, int );
	void not_equals( HFrame*, int );
	void less( HFrame*, int );
	void greater( HFrame*, int );
	void less_or_equal( HFrame*, int );
	void greater_or_equal( HFrame*, int );
	void boolean_and( HFrame*, int );
	void boolean_or( HFrame*, int );
	void boolean_xor( HFrame*, int );
	void boolean_not( HFrame*, int );
	void store_direct( value_t const&, HFrame*, int );
	void store_real( double long, HFrame*, int );
	void store_integer( int long long, HFrame*, int );
	void store_string( yaal::hcore::HString const&, HFrame*, int );
	void store_number( yaal::hcore::HString const&, HFrame*, int );
	void store_character( char, HFrame*, int );
	void store_boolean( bool, HFrame*, int );
	void store_none( HFrame*, int );
protected:
	virtual void do_execute( HHuginn::HThread* ) const;
private:
	HExpression( HExpression const& ) = delete;
	HExpression& operator = ( HExpression const& ) = delete;
};

class HHuginn::HValue : public HHuginn::HObject {
public:
	typedef HHuginn::HValue this_type;
	typedef HHuginn::HObject base_type;
	typedef yaal::hcore::HBoundCall<void ( HValue* )> method_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, method_t> methods_t;
private:
	TYPE const _type;
	methods_t _methods;
public:
	HValue( void );
	HValue( TYPE );
	TYPE type( void ) const;
	yaal::hcore::HString const& type_name( void ) const;
	static yaal::hcore::HString const& type_name( TYPE );
	static value_t subscript( HExpression::SUBSCRIPT, value_t&, value_t const&, int );
	static value_t add( value_t const&, value_t const&, int );
	static value_t sub( value_t const&, value_t const&, int );
	static value_t mul( value_t const&, value_t const&, int );
	static value_t div( value_t const&, value_t const&, int );
	static value_t mod( value_t const&, value_t const&, int );
	static value_t pow( value_t const&, value_t const&, int );
	static value_t abs( value_t const&, int );
	static value_t neg( value_t const&, int );
	static bool equals( value_t const&, value_t const&, int );
	static bool less( value_t const&, value_t const&, int );
	static bool less_low( value_t const&, value_t const& );
	static bool greater( value_t const&, value_t const&, int );
	static bool less_or_equal( value_t const&, value_t const&, int );
	static bool greater_or_equal( value_t const&, value_t const&, int );
	static value_t boolean_xor( value_t const&, value_t const&, int );
	static value_t boolean_not( value_t const&, int );
	static value_t string( value_t const&, int );
	static value_t integer( value_t const&, int );
	static value_t real( value_t const&, int );
	static value_t boolean( value_t const&, int );
	static value_t character( value_t const&, int );
	static value_t number( value_t const&, int );
};

class HHuginn::HFrame {
public:
	typedef HHuginn::HFrame this_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::value_t> variables_t;
	typedef yaal::hcore::HStack<HHuginn::value_t> values_t;
	enum class STATE {
		NORMAL,
		RETURN,
		BREAK,
		CONTINUE,
		EXCEPTION
	};
private:
	HThread* _thread;
	HFrame* const _parent;
	variables_t _variables;
	operations_t _operations;
	values_t _values;
	HHuginn::value_t _result;
	int const _number;
	bool _loop;
	STATE _state;
public:
	HFrame( HThread*, HFrame*, bool, bool );
	value_t make_variable( yaal::hcore::HString const&, int );
	void set_variable( yaal::hcore::HString const&, HHuginn::value_t const&, int );
	value_t get_reference( yaal::hcore::HString const&, int );
	value_t try_reference( yaal::hcore::HString const&, int );
	bool can_continue( void ) const;
	void break_execution( STATE );
	int number( void ) const;
	HFrame* parent( void );
	HThread* thread( void ) const;
	bool is_loop( void ) const;
	HHuginn::value_t result( void ) const;
	void set_result( HHuginn::value_t const& );
	operations_t& operations( void );
	values_t& values( void );
	void reset( void );
private:
	HFrame( HFrame const& ) = delete;
	HFrame& operator = ( HFrame const& ) = delete;
};

class HHuginn::HThread {
public:
	typedef HHuginn::HThread this_type;
	typedef yaal::hcore::HStack<HHuginn::frame_t> frames_t;
private:
	frames_t _frames;
	yaal::hcore::HThread::id_t _id;
	HHuginn* _huginn;
public:
	HThread( HHuginn*, yaal::hcore::HThread::id_t );
	void create_function_frame( void );
	void create_loop_frame( void );
	void create_scope_frame( void );
	void pop_frame( void );
	HFrame* current_frame( void );
	HFrame const* current_frame( void ) const;
	void break_execution( HFrame::STATE, HHuginn::value_t const& = HHuginn::value_t(), int = 0 );
	bool can_continue( void ) const;
	yaal::hcore::HThread::id_t id( void ) const;
	HHuginn& huginn( void );
private:
	HThread( HThread const& ) = delete;
	HThread& operator = ( HThread const& ) = delete;
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
};

class HHuginn::HBooleanEvaluator : public HHuginn::HValue {
public:
	typedef HHuginn::HBooleanEvaluator this_type;
	typedef HHuginn::HValue base_type;
	typedef yaal::hcore::HArray<expression_t> expressions_t;
private:
	expressions_t _expressions;
	OPERATOR _operator;
public:
	HBooleanEvaluator( expressions_t const&, OPERATOR );
	bool execute( HThread* );
};

class HHuginn::HIterable : public HHuginn::HValue {
public:
	typedef HHuginn::HIterable this_type;
	typedef HHuginn::HValue base_type;
	class HIterator;
	HIterable( TYPE );
	HIterator iterator( void );
protected:
	virtual HIterator do_iterator( void ) = 0;
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
};

class HHuginn::HString : public HHuginn::HIterable {
public:
	typedef HHuginn::HString this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _value;
public:
	HString( yaal::hcore::HString const& );
	yaal::hcore::HString const& value( void ) const;
	yaal::hcore::HString& value( void );
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
protected:
	virtual HIterator do_iterator( void );
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
};

class HHuginn::HList : public HHuginn::HIterable {
public:
	typedef HHuginn::HList this_type;
	typedef HHuginn::HIterable base_type;
private:
	typedef yaal::hcore::HDeque<HHuginn::value_t> values_t;
	values_t _data;
public:
	HList( void );
	void push_back( value_t const& );
	int long size( void ) const;
	value_t get( int long long );
	value_t get_ref( int long long );
protected:
	virtual HIterator do_iterator( void );
};

class HHuginn::HMap : public HHuginn::HIterable {
public:
	typedef HHuginn::HMap this_type;
	typedef HHuginn::HIterable base_type;
	typedef bool (*cmp_t)( HHuginn::value_t const&, HHuginn::value_t const& );
	typedef yaal::hcore::HMap<HHuginn::value_t, HHuginn::value_t, cmp_t> values_t;
private:
	values_t _data;
	HHuginn::TYPE _keyType;
public:
	HMap( void );
	int long size( void ) const;
	value_t get( HHuginn::value_t const&, int );
	value_t get_ref( HHuginn::value_t const&, int );
protected:
	virtual HIterator do_iterator( void );
private:
	void verify_key_type( HHuginn::TYPE, int );
};

class HHuginn::HScope : public HHuginn::HStatement {
public:
	typedef HHuginn::HScope this_type;
	typedef HHuginn::HStatement base_type;
private:
	HHuginn::statement_list_t _statements;
public:
	HScope( void );
	virtual ~HScope( void ) {
		return;
	}
	void add_statement( statement_t );
protected:
	virtual void do_execute( HHuginn::HThread* ) const;
private:
	HScope( HScope const& ) = delete;
	HScope( HScope&& ) = delete;
	HScope& operator = ( HScope const& ) = delete;
};

class HHuginn::HReturn : public HHuginn::HStatement {
public:
	typedef HHuginn::HReturn this_type;
	typedef HHuginn::HStatement base_type;
private:
	expression_t _expression;
public:
	HReturn( expression_t const& );
protected:
	virtual void do_execute( HHuginn::HThread* ) const;
private:
	HReturn( HReturn const& ) = delete;
	HReturn& operator = ( HReturn const& ) = delete;
};

class HHuginn::HBreak : public HHuginn::HStatement {
public:
	typedef HHuginn::HBreak this_type;
	typedef HHuginn::HStatement base_type;
public:
	HBreak( void );
protected:
	virtual void do_execute( HHuginn::HThread* ) const;
private:
	HBreak( HBreak const& ) = delete;
	HBreak& operator = ( HBreak const& ) = delete;
};

class HHuginn::HIf : public HHuginn::HStatement {
public:
	typedef HHuginn::HIf this_type;
	typedef HHuginn::HStatement base_type;
	typedef HHuginn::OCompiler::OCompilationFrame::contexts_t if_clauses_t;
private:
	if_clauses_t _ifClauses;
	scope_t _elseClause;
public:
	HIf( if_clauses_t const&, scope_t const& );
protected:
	virtual void do_execute( HThread* ) const;
};

class HHuginn::HSwitch : public HHuginn::HStatement {
public:
	typedef HHuginn::HSwitch this_type;
	typedef HHuginn::HStatement base_type;
	typedef HHuginn::OCompiler::OCompilationFrame::contexts_t cases_t;
private:
	expression_t _expression;
	cases_t _cases;
	scope_t _default;
public:
	HSwitch( expression_t const&, cases_t const&, scope_t const& );
protected:
	virtual void do_execute( HThread* ) const;
};

class HHuginn::HWhile : public HHuginn::HStatement {
public:
	typedef HHuginn::HWhile this_type;
	typedef HHuginn::HStatement base_type;
private:
	expression_t _condition;
	scope_t _loop;
public:
	HWhile( expression_t const&, scope_t const& );
protected:
	virtual void do_execute( HThread* ) const;
};

class HHuginn::HFor : public HHuginn::HStatement {
public:
	typedef HHuginn::HFor this_type;
	typedef HHuginn::HStatement base_type;
private:
	yaal::hcore::HString _variableName;
	expression_t _source;
	scope_t _loop;
	int _position;
public:
	HFor( yaal::hcore::HString const&, expression_t const&, scope_t const&, int );
protected:
	virtual void do_execute( HThread* ) const;
};

class HHuginn::HFunction {
public:
	typedef HHuginn::HFunction this_type;
	typedef HHuginn::OCompiler::parameter_names_t parameter_names_t;
	typedef HHuginn::OCompiler::expressions_t expressions_t;
private:
	yaal::hcore::HString _name;
	parameter_names_t _parameterNames;
	expressions_t _defaultValues;
	HHuginn::scope_t _scope;
public:
	HFunction( yaal::hcore::HString const&, parameter_names_t const&, HHuginn::scope_t const&, expressions_t const& );
	HFunction( HFunction&& ) = default;
	value_t execute( HThread*, values_t const&, int ) const;
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
};

}

}

#endif /* #ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED */

