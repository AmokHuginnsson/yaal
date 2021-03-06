/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/frame.hxx
 * \brief Declaration of huginn::HFrame class.
 */

#ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED 1

#include "tools/huginn/object.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;
class HFunction;
class HStatement;

class HFrame {
public:
	typedef HFrame this_type;
	typedef yaal::hcore::HStack<HHuginn::value_t> values_t;
	typedef yaal::hcore::HStack<int> instruction_pointers_t;
	typedef yaal::hcore::HArray<HHuginn::identifier_id_t> identifiers_t;
	typedef yaal::hcore::HResource<HHuginn::values_t> values_holder_t;
	typedef yaal::hcore::HArray<values_holder_t> value_cache_t;
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

	HStatement const* _statement;
	int _position;
public:
	HFrame( HThread*, HFrame* );
	void init( HStatement const*, HHuginn::value_t* = nullptr, int = 0 );
	void set_thread( HThread* );
	void reshape( void );
	HHuginn::value_t get_field( int index_ ) {
		M_PROLOG
		M_ASSERT( _object && !! *_object );
		return ( static_cast<HObject*>( _object->raw() )->field( *_object, index_ ) );
		M_EPILOG
	}

	HHuginn::value_t& get_field_ref( int index_ ) {
		M_PROLOG
		M_ASSERT( _object && !! *_object );
		return ( static_cast<HObject*>( _object->raw() )->field_ref( index_ ) );
		M_EPILOG
	}

	HHuginn::value_t const& get_variable_value( int index_ ) {
		M_PROLOG
		return ( _variables[index_] );
		M_EPILOG
	}

	HHuginn::value_t& make_variable( int index_ ) {
		M_PROLOG
		M_ASSERT( index_ <= static_cast<int>( _variables.get_size() ) );
		/*
		 * It is very difficult to remove following if() statement due to
		 * loop statements in Huginn language.
		 * For `for' and `while' loop statement variables that are local to their scope ({...})
		 * shall be created during first iteration and only updated during subsequent iterations.
		 *
		 * Current implementation has interesting side effect that local variables
		 * in all but last of iterations are destroyed in order of their definition
		 * and not in reverse order of their definition as it would be expected.
		 */
		if ( index_ == static_cast<int>( _variables.get_size() ) ) {
			M_ASSERT( _variables.get_capacity() >= ( _variables.get_size() + 1 ) );
			_variables.push_back( HHuginn::value_t() );
		}
		return ( _variables[index_] );
		M_EPILOG
	}

	HHuginn::value_t& get_variable_reference( int index_ ) {
		M_PROLOG
		return ( _variables[index_] );
		M_EPILOG
	}

	HHuginn::value_t get_this( void ) {
		M_PROLOG
		M_ASSERT( _object && !! *_object );
		return ( *_object );
		M_EPILOG
	}

	HHuginn::value_t get_super( int );
	void note_variable( HHuginn::identifier_id_t );
	void note_variable( HHuginn::identifier_id_t, int );
	int number( void ) const {
		return ( _number );
	}
	HFrame* parent( void ) {
		return ( _parent );
	}
	HThread* thread( void ) const {
		return ( _thread );
	}
	HHuginn::value_t& result( void ) {
		return ( _result );
	}
	void set_result( HHuginn::value_t&& );
	values_t& values( void ) {
		return ( _values );
	}
	void reset( void );
	void reset_expression_state( void );
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
	int get_variable_count( void ) const {
		return ( static_cast<int>( _variables.get_size() ) );
	}
	void pop_local_variables( int to_ ) {
		int activeVariableCount( get_variable_count() );
		while ( activeVariableCount > to_ ) {
			-- activeVariableCount;
			_variables.pop_back();
		}
		return;
	}
	int file_id( void ) const;
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
		return vals;
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
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FRAME_HXX_INCLUDED */

