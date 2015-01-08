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
	class HReturn;
	class HClass;
	class HMethod;
	class HFunction;
	typedef yaal::hcore::HPointer<HFunction> function_t;
	class HReference;
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
	typedef yaal::hcore::HPointer<HExpression> expression_t;
	class HBooleanExpression;
	typedef yaal::hcore::HPointer<HBooleanExpression> boolean_expression_t;
	class HErrorCoordinate;
	typedef yaal::hcore::HArray<statement_t> statement_list_t;
	typedef yaal::hcore::HArray<value_t> values_t;
	class HFrame;
	typedef yaal::hcore::HPointer<HFrame> frame_t;
	class HThread;
	typedef yaal::hcore::HPointer<HThread> thread_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HThread::id_t, thread_t> threads_t;
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
		typedef void ( HHuginn::HExpression::* expression_action_t ) ( void );
		typedef yaal::hcore::HStack<scope_t> scope_stack_t;
		HHuginn* _huginn;
		yaal::hcore::HString _functionName;
		function_t _functionScope;
		expression_t _expression;
		scope_stack_t _scopeStack;
		statement_list_t _statementList;
		OCompiler( HHuginn* );
		void set_function_name( yaal::hcore::HString const& );
		void create_scope( void );
		void commit_scope( void );
		void add_return_statement( void );
		void commit_expression( void );
		void defer_function_call( yaal::hcore::HString const& );
		void defer_get_variable( yaal::hcore::HString const& );
		void defer_oper( char );
		void defer_str_oper( yaal::hcore::HString const& );
		void defer_action( expression_action_t const& );
		void defer_store_real( double long );
		void defer_store_integer( int long long );
		void defer_store_string( yaal::hcore::HString const& );
		void defer_store_character( char );
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
	 */
	void compile( void );
	/*! \brief Execute compiled program.
	 */
	void execute( void );
	/*! \brief Dump Huginn Virtual Machine state.
	 */
	value_t call( yaal::hcore::HString const&, values_t const& );
	value_t result( void ) const;
	void dump_vm_state( yaal::hcore::HStreamInterface& );
	HFrame* current_frame( void );
	void create_function( void );
	void add_argument( yaal::hcore::HString const& );
	value_t returned_value( void ) const;
	void dump_preprocessed_source( yaal::hcore::HStreamInterface& );
	int error_position( void ) const;
	HErrorCoordinate error_coordinate( void ) const;
	yaal::hcore::HString error_message( void ) const;
	executing_parser::HRule make_engine( void );
private:
	char const* error_message( int ) const;
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

class HHuginn::HObject {
public:
	typedef HHuginn::HObject this_type;
	virtual ~HObject( void ) {}
};

class HHuginn::HValue : public HHuginn::HObject {
public:
	typedef HHuginn::HValue this_type;
	typedef HHuginn::HObject base_type;
	enum class TYPE {
		NONE,
		BOOLEAN,
		INTEGER,
		REAL,
		STRING,
		NUMBER,
		CHARACTER,
		LIST,
		MAP
	};
	typedef yaal::hcore::HBoundCall<void ( HValue* )> method_t;
	typedef yaal::hcore::HHashMap<yaal::hcore::HString, method_t> methods_t;
private:
	TYPE const _type;
	methods_t _methods;
public:
	HValue( void );
	HValue( TYPE );
	TYPE type( void ) const;
	static yaal::hcore::HString const& type_name( TYPE );
	static value_t subscript( value_t const&, value_t const& );
	static value_t add( value_t const&, value_t const& );
	static value_t sub( value_t const&, value_t const& );
	static value_t mul( value_t const&, value_t const& );
	static value_t div( value_t const&, value_t const& );
	static value_t mod( value_t const&, value_t const& );
	static value_t pow( value_t const&, value_t const& );
	static value_t abs( value_t const& );
	static value_t neg( value_t const& );
	static bool equals( value_t const&, value_t const& );
	static bool less( value_t const&, value_t const& );
	static bool greater( value_t const&, value_t const& );
	static bool less_or_equal( value_t const&, value_t const& );
	static bool greater_or_equal( value_t const&, value_t const& );
	static value_t boolean_and( value_t const&, value_t const& );
	static value_t boolean_or( value_t const&, value_t const& );
	static value_t boolean_xor( value_t const&, value_t const& );
	static value_t boolean_not( value_t const& );
	static value_t string( value_t const& );
	static value_t integer( value_t const& );
	static value_t real( value_t const& );
	static value_t character( value_t const& );
	static value_t number( value_t const& );
};

class HHuginn::HFrame {
public:
	typedef HHuginn::HFrame this_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::value_t> variables_t;
	enum class STATE {
		NORMAL,
		RETURN,
		BREAK,
		CONTINUE,
		EXCEPTION
	};
private:
	variables_t _variables;
	HHuginn::value_t _result;
	int const _number;
	HFrame* const _parent;
	bool _loop;
	STATE _state;
public:
	HFrame( HFrame*, bool, bool );
	void set_variable( yaal::hcore::HString const&, HHuginn::value_t const& );
	value_t& get_variable( yaal::hcore::HString const& );
	bool can_continue( void ) const;
	void break_execution( STATE );
	int number( void ) const;
	HFrame* parent( void );
	bool is_loop( void ) const;
	HHuginn::value_t result( void ) const;
	void set_result( HHuginn::value_t const& );
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
public:
	HThread( yaal::hcore::HThread::id_t );
	void create_function_frame( void );
	void create_loop_frame( void );
	void create_scope_frame( void );
	void pop_frame( void );
	HFrame* current_frame( void );
	HFrame const* current_frame( void ) const;
	void break_execution( HFrame::STATE, HHuginn::value_t const& = HHuginn::value_t(), int = meta::max_signed<int>::value );
	bool can_continue( void ) const;
	yaal::hcore::HThread::id_t id( void ) const;
};

class HHuginn::HIterable : public HHuginn::HValue {
public:
	typedef HHuginn::HIterable this_type;
	typedef HHuginn::HValue base_type;
	HIterable( TYPE );
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

class HHuginn::HString : public HHuginn::HValue {
public:
	typedef HHuginn::HString this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _value;
public:
	HString( yaal::hcore::HString const& );
	yaal::hcore::HString const& value( void ) const;
	yaal::hcore::HString& value( void );
	void to_character( void ) const;
	void to_integer( void ) const;
	void to_number( void ) const;
	void to_real( void ) const;
	void to_string( void ) const;
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
};

class HHuginn::HMap : public HHuginn::HIterable {
public:
	typedef HHuginn::HMap this_type;
	typedef HHuginn::HIterable base_type;
private:
	typedef yaal::hcore::HMap<HHuginn::value_t, HHuginn::value_t> values_t;
	values_t _data;
public:
	HMap( void );
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
private:
	typedef yaal::hcore::HArray<HExecutingParser::executor_t> execution_steps_t;
	typedef yaal::hcore::HStack<OPERATOR> operations_t;
	typedef yaal::hcore::HStack<HHuginn::value_t> values_t;
	execution_steps_t _executionSteps;
	operations_t _operations;
	values_t _values;
	HHuginn* _huginn;
public:
	HExpression( HHuginn* );
	HHuginn::value_t result( void ) const;
	void add_execution_step( HExecutingParser::executor_t const& );
	void oper( OPERATOR );
	void close_parenthesis( void );
	void plus_minus( void );
	void mul_div_mod( void );
	void add_arg( void );
	void function_call( yaal::hcore::HString const& );
	void function_call_exec( void );
	void get_variable( yaal::hcore::HString const& );
	void subscript( void );
	void power( void );
	void equals( void );
	void not_equals( void );
	void less( void );
	void greater( void );
	void less_or_equal( void );
	void greater_or_equal( void );
	void boolean_and( void );
	void boolean_or( void );
	void boolean_xor( void );
	void boolean_not( void );
	void store_real( double long );
	void store_integer( int long long );
	void store_string( yaal::hcore::HString const& );
	void store_character( char );
protected:
	virtual void do_execute( HHuginn::HThread* ) const;
private:
	HExpression( HExpression const& ) = delete;
	HExpression& operator = ( HExpression const& ) = delete;
};

class HHuginn::HBooleanExpression : public HHuginn::HObject {
public:
	typedef HHuginn::HBooleanExpression this_type;
	typedef HHuginn::HObject base_type;
};

class HHuginn::HScope : public HHuginn::HStatement {
public:
	typedef HHuginn::HScope this_type;
	typedef HHuginn::HStatement base_type;
private:
	HHuginn::statement_list_t _statements;
	HHuginn::HScope* _parent;
public:
	HScope( HScope* );
	HScope( HScope&& ) = default;
	virtual ~HScope( void ) {
		return;
	}
	void add_statement( statement_t );
protected:
	virtual void do_execute( HHuginn::HThread* ) const;
private:
	HScope( HScope const& );
	HScope& operator = ( HScope const& );
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

class HHuginn::HIf : public HHuginn::HStatement {
public:
	typedef HHuginn::HIf this_type;
	typedef HHuginn::HStatement base_type;
private:
	boolean_expression_t _condition;
	HExecutingParser::executor_t _ifClause;
	HExecutingParser::executor_t _elseClause;
public:
	HIf( boolean_expression_t, HExecutingParser::executor_t, HExecutingParser::executor_t );
};

class HHuginn::HWhile : public HHuginn::HStatement {
public:
	typedef HHuginn::HWhile this_type;
	typedef HHuginn::HStatement base_type;
private:
	boolean_expression_t _condition;
	HExecutingParser::executor_t _loop;
protected:
};

class HHuginn::HFor : public HHuginn::HStatement {
public:
	typedef HHuginn::HFor this_type;
	typedef HHuginn::HStatement base_type;
private:
	iterable_t _container;
	HExecutingParser::executor_t _loop;
protected:
};

class HHuginn::HFunction : public HHuginn::HScope {
public:
	typedef HHuginn::HFunction this_type;
	typedef HHuginn::HScope base_type;
	typedef yaal::hcore::HArray<yaal::hcore::HString> argument_names_t;
private:
	yaal::hcore::HString _name;
	argument_names_t _argumentNames;
public:
	HFunction( yaal::hcore::HString const& );
	HFunction( HFunction&& ) = default;
	using HStatement::execute;
	value_t execute( HThread*, values_t const& ) const;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED */

