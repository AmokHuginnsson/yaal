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
#include "hcore/hstreaminterface.hxx"
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
	class HScope;
	typedef yaal::hcore::HPointer<HScope> scope_t;
	class HIf;
	class HWhile;
	class HForeach;
	class HClass;
	class HMethod;
	class HFunction;
	class HReference;
	class HValue;
	typedef yaal::hcore::HPointer<HValue> value_t;
	class HInteger;
	class HFloat;
	class HString;
	class HCharacter;
	class HList;
	typedef yaal::hcore::HPointer<HList> list_t;
	class HMap;
	class HExpression;
	typedef yaal::hcore::HPointer<HExpression> expression_t;
	class HBooleanExpression;
	typedef yaal::hcore::HPointer<HBooleanExpression> boolean_expression_t;
	class HErrorCoordinate;
private:
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::HFunction> functions_t;
	typedef yaal::hcore::HMap<int, int> skips_t;
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
	yaal::hcore::HString _sourceName;
	yaal::hcore::HChunk _source;
	int _sourceSize;
	yaal::hcore::HChunk _preprocessedSource;
	int _preprocessedSourceSize;
	skips_t _skips;
	list_t _arguments;
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
	void create_function( void );
	void add_argument( yaal::hcore::HString const& );
	void call( yaal::hcore::HString const& );
	value_t returned_value( void ) const;
	void dump_preprocessed_source( yaal::hcore::HStreamInterface& );
	int error_position( void ) const;
	HErrorCoordinate error_coordinate( void ) const;
private:
};

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
};

class HHuginn::HIterable : public HHuginn::HValue {
public:
	typedef HHuginn::HIterable this_type;
	typedef HHuginn::HValue base_type;
};

class HHuginn::HInteger : public HHuginn::HValue {
public:
	typedef HHuginn::HInteger this_type;
	typedef HHuginn::HValue base_type;
private:
	int long long _value;
public:
	HInteger( int long long );
};

class HHuginn::HFloat : public HHuginn::HValue {
public:
	typedef HHuginn::HFloat this_type;
	typedef HHuginn::HValue base_type;
private:
	double long _value;
public:
	HFloat( double long );
};

class HHuginn::HString : public HHuginn::HValue {
public:
	typedef HHuginn::HString this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _value;
public:
	HString( yaal::hcore::HString const& );
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
};

class HHuginn::HMap : public HHuginn::HIterable {
public:
	typedef HHuginn::HMap this_type;
	typedef HHuginn::HIterable base_type;
};

class HHuginn::HExpression : public HHuginn::HObject {
public:
	typedef HHuginn::HExpression this_type;
	typedef HHuginn::HObject base_type;
};

class HHuginn::HBooleanExpression : public HHuginn::HObject {
public:
	typedef HHuginn::HBooleanExpression this_type;
	typedef HHuginn::HObject base_type;
};

class HHuginn::HScope : public HHuginn::HObject {
public:
	typedef HHuginn::HScope this_type;
	typedef HHuginn::HObject base_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::value_t> variables_t;
private:
	variables_t _variables;
	HHuginn::HScope* _parent;
public:
	HScope( HScope* );
private:
	HScope( HScope const& );
	HScope& operator = ( HScope const& );
};

class HHuginn::HIf : public HHuginn::HScope {
public:
	typedef HHuginn::HIf this_type;
	typedef HHuginn::HScope base_type;
private:
	boolean_expression_t _condition;
	HExecutingParser::executor_t _ifClause;
	HExecutingParser::executor_t _elseClause;
public:
	HIf( boolean_expression_t, HExecutingParser::executor_t, HExecutingParser::executor_t );
};

class HHuginn::HWhile : public HHuginn::HScope {
public:
	typedef HHuginn::HWhile this_type;
	typedef HHuginn::HScope base_type;
private:
	boolean_expression_t _condition;
	HExecutingParser::executor_t _loop;
};

class HHuginn::HForeach : public HHuginn::HScope {
public:
	typedef HHuginn::HForeach this_type;
	typedef HHuginn::HScope base_type;
private:
	iterable_t _container;
	HExecutingParser::executor_t _loop;
};

class HHuginn::HFunction : public HHuginn::HScope {
public:
	typedef HHuginn::HFunction this_type;
	typedef HHuginn::HScope base_type;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HHUGINN_HXX_INCLUDED */

