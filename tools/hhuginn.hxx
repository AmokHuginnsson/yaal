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
	class HScope;
	class HIf;
	class HWhile;
	class HClass;
	class HMethod;
	class HFunction;
	class HReference;
	class HValue;
	class HInteger;
	class HFloat;
	class HString;
	class HList;
	class HMap;
private:
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::HFunction> functions_t;
	functions_t _functions;
	HExecutingParser _engine;
public:
	void parse( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	void parse( yaal::hcore::HStreamInterface const& );
	void execute( void );
	void create_function( void );
	void call( yaal::hcore::HString const& );
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
	typedef yaal::hcore::HPointer<HValue> ptr_t;
};

class HHuginn::HInteger : public HHuginn::HValue {
public:
	typedef HHuginn::HInteger this_type;
	typedef HHuginn::HValue base_type;
private:
	int long long _value;
};

class HHuginn::HFloat : public HHuginn::HValue {
public:
	typedef HHuginn::HFloat this_type;
	typedef HHuginn::HValue base_type;
private:
	double long _value;
};

class HHuginn::HString : public HHuginn::HValue {
public:
	typedef HHuginn::HString this_type;
	typedef HHuginn::HValue base_type;
private:
	yaal::hcore::HString _value;
};

class HHuginn::HList : public HHuginn::HValue {
public:
	typedef HHuginn::HList this_type;
	typedef HHuginn::HValue base_type;
};

class HHuginn::HMap : public HHuginn::HValue {
public:
	typedef HHuginn::HMap this_type;
	typedef HHuginn::HValue base_type;
};

class HHuginn::HScope : public HHuginn::HObject {
public:
	typedef HHuginn::HScope this_type;
	typedef HHuginn::HObject base_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HHuginn::HValue::ptr_t> variables_t;
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
	HExecutingParser::executor_t _condition;
	HExecutingParser::executor_t _ifClause;
	HExecutingParser::executor_t _elseClause;
public:
	HIf( HExecutingParser::executor_t, HExecutingParser::executor_t, HExecutingParser::executor_t );
};

class HHuginn::HWhile : public HHuginn::HScope {
public:
	typedef HHuginn::HWhile this_type;
	typedef HHuginn::HScope base_type;
private:
	HExecutingParser::executor_t _condition;
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

