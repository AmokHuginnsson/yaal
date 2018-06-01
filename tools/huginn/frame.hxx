/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/frame.hxx
 * \brief Declaration of huginn::HFrame class.
 */

#ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;
class HFunction;

class HFrame {
public:
	typedef HFrame this_type;
	typedef yaal::hcore::HStack<HHuginn::value_t> values_t;
	typedef yaal::hcore::HStack<int> instruction_pointers_t;
	typedef yaal::hcore::HArray<HHuginn::identifier_id_t> identifiers_t;
	typedef yaal::hcore::HResource<HHuginn::values_t> values_holder_t;
	typedef yaal::hcore::HArray<values_holder_t> value_cache_t;
	enum class TYPE {
		SCOPE,
		LOOP,
		TRY_CATCH,
		FUNCTION
	};
	enum class STATE {
		NORMAL,
		RETURN,
		BREAK,
		CONTINUE,
		EXCEPTION,
		RUNTIME_EXCEPTION
	};
	enum class ACCESS {
		VALUE,
		REFERENCE,
		BOUND_CALL
	};
private:
	/*!
	 * A pointer to the thread on which behave this frame was initialized.
	 */
	HThread* _thread;

	/*!
	 * Pointer to parent frame of this frame.
	 */
	HFrame* const _parent;

	/*!
	 * If current frame is initialized for a call
	 * to user defined method in user defined `class`, then
	 * '_object' holds `this` reference, otherwise it is nullptr.
	 */
	HHuginn::value_t* _object;

	/*!
	 * Upcast level, a number of consecutive uses of `super`
	 * that is in effect for '_object' at current frame.
	 */
	int _upCast;

	/*!
	 * A (linear) store for *local* variable values defined
	 * and available in this scope (frame) so far.
	 */
	HHuginn::values_t _variables;

	/*!
	 * Identifiers for the variables defined in current scope (frame) so far.
	 */
	identifiers_t _variableIdentifiers; /*!< Used for introspection only. */

	/*
	 * Instruction pointers must be kept per frame (and not in HThread)
	 * because new frame (new scope) can be created from within an expression
	 * by means of function call.
	 *
	 * Instruction pointers must be kept in form of stack to support subexpressions,
	 * e.g.: boolean short-circuits and ternary operators.
	 */
	instruction_pointers_t _instructionPointers;

	/*!
	 * Value stack used by expression evaluator.
	 */
	values_t _values;

	/*!
	 * Used exclusively to speed up function call parameter construction
	 * through HThread::value_cache() and HFrame::value_cache().
	 */
	value_cache_t _valueCache;

	/*! Number of argument sets in _valueCache.
	 */
	int _valueCacheSize;

	/*!
	 * Temporary store for expression result value used to pass expression results
	 * across frames and functions.
	 */
	HHuginn::value_t _result;

	/*!
	 * A frame number in the stack.
	 */
	int _number;

	TYPE _type;
	STATE _state;
	HStatement const* _statement;
	int _position;
public:
	HFrame( HThread*, HFrame* );
	void init( TYPE, HStatement const*, HHuginn::value_t* = nullptr, int = 0 );
	void reshape( HThread*, int );
	HHuginn::value_t get_field( ACCESS, int );
	HHuginn::value_t const& get_variable_value( HStatement::statement_id_t, int );
	HHuginn::value_t get_variable_reference( HStatement::statement_id_t, int );
	HHuginn::value_t make_variable( HStatement::statement_id_t, int );
	HHuginn::value_t get_this( void );
	HHuginn::value_t get_super( int );
	void note_variable( HHuginn::identifier_id_t );
	void note_variable( HHuginn::identifier_id_t, HStatement::statement_id_t, int );
	void commit_variable( HHuginn::value_t const&, int );
	bool can_continue( void ) const {
		return ( _state == STATE::NORMAL );
	}
	void break_execution( STATE );
	void continue_execution( void );
	int number( void ) const {
		return ( _number );
	}
	HFrame* parent( void ) {
		return ( _parent );
	}
	HThread* thread( void ) const {
		return ( _thread );
	}
	bool is_loop( void ) const {
		return ( _type == TYPE::LOOP );
	}
	bool has_catch( void ) const {
		return ( _type == TYPE::TRY_CATCH );
	}
	TYPE type( void ) const {
		return ( _type );
	}
	STATE state( void ) const {
		return ( _state );
	}
	HHuginn::value_t result( void ) const {
		return ( _result );
	}
	void set_result( HHuginn::value_t&& );
	values_t& values( void ) {
		return ( _values );
	}
	void reset( void );
	/*! \brief Cleanup before unwinding caused by C++ exception.
	 */
	void cleanup( void );
	int& ip( void ) {
		return ( _instructionPointers.top() );
	}
	void start_expression( void ) {
		_instructionPointers.push( 0 );
	}
	void end_expression( void ) {
		_instructionPointers.pop();
	}
	HStatement const* statement( void ) const {
		return ( _statement );
	}
	int file_id( void ) const {
		return ( _statement->file_id() );
	}
	void set_position( int position_ ) {
		_position = position_;
	}
	int position( void ) const {
		return ( _position );
	}
	HHuginn::values_t& variable_values( void ) {
		return ( _variables );
	}
	identifiers_t const& variable_identifiers( void ) const {
		return ( _variableIdentifiers );
	}
protected:
	HHuginn::values_t& value_cache( void ) {
		if (  _valueCacheSize == static_cast<int>( _valueCache.get_size() ) ) {
			_valueCache.emplace_back( yaal::hcore::make_resource<HHuginn::values_t>() );
			_valueCache.back()->reserve( _variables.get_capacity() );
		}
		HHuginn::values_t& vals( *_valueCache[_valueCacheSize] );
		++ _valueCacheSize;
		return ( vals );
	}
	void invalidate_value_cache( void ) {
		-- _valueCacheSize;
		_valueCache[_valueCacheSize]->clear();
		return;
	}
	friend class HArguments;
private:
	HFrame( HFrame const& ) = delete;
	HFrame& operator = ( HFrame const& ) = delete;
	HHuginn::value_t* object( void ) const;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED */

