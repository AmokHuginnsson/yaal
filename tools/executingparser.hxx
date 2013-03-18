/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/executingparser.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/executingparser.hxx
 * Dleclaration of executing_parser namespace.
 */

#ifndef YAAL_TOOLS_EXECUTINGPARSER_HXX_INCLUDED
#define YAAL_TOOLS_EXECUTINGPARSER_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hnumber.hxx"
#include "hcore/harray.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hboundcall.hxx"

namespace yaal {

namespace tools {

/*! \brief Namespace containing classes composing executing parser infrastructure.
 */
namespace executing_parser {

class HRule {
public:
	typedef HRule this_type;
	typedef yaal::hcore::HBoundCall<> action_t;
	typedef yaal::hcore::HBoundCall<> executor_t;
	typedef yaal::hcore::HArray<executor_t> executors_t;
	typedef yaal::hcore::HPointer<HRule> ptr_t;
protected:
	ptr_t _rule;
	action_t _action;
	executors_t _excutors;
	bool _matched;
public:
	HRule( void );
public:
	HRule( HRule const& rule_ );
	virtual ~HRule( void )
		{}
	HRule operator[]( action_t action_ );
	bool operator()( yaal::hcore::HString const& input_ );
	bool operator()( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
	void operator()( void );
	void execute( void );
	void cancel_execution( void );
	yaal::hcore::HString::const_iterator parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
	ptr_t clone( void ) const;
	bool is_optional( void ) const;
private:
	HRule( ptr_t rule_, action_t action_ );
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
	virtual void do_execute( void );
	virtual void do_cancel_execution( void );
	virtual ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	static yaal::hcore::HString::const_iterator skip_space( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
};

typedef yaal::hcore::HExceptionT<HRule> HRuleException;

class HFollows : public HRule {
	typedef yaal::hcore::HList<HRule::ptr_t> rules_t;
	rules_t _rules;
public:
	typedef HFollows this_type;
	typedef HRule base_type;
	HFollows( HRule const& predecessor_, HRule const& successor_ );
	HFollows( HFollows const& follows_ );
	HFollows( HFollows const& predecessors_, HRule const& successor_ );
	virtual ~HFollows( void )
		{}
protected:
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
	virtual void do_cancel_execution( void );
	virtual void do_execute( void );
};

typedef yaal::hcore::HExceptionT<HFollows, HRuleException> HFollowsException;

class HKleeneStar : public HRule {
public:
	typedef HKleeneStar this_type;
	typedef HRule base_type;
	HKleeneStar( HRule const& rule_ );
	HKleeneStar( HKleeneStar const& kleeneStar_ );
	virtual ~HKleeneStar( void )
		{}
protected:
	virtual ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
};

typedef yaal::hcore::HExceptionT<HKleeneStar, HRuleException> HKleeneStarException;

class HKleenePlus : public HRule {
public:
	typedef HKleenePlus this_type;
	typedef HRule base_type;
	HKleenePlus( HRule const& rule_ );
	HKleenePlus( HKleenePlus const& kleenePlus_ );
	virtual ~HKleenePlus( void )
		{}
protected:
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
};

typedef yaal::hcore::HExceptionT<HKleenePlus, HRuleException> HKleenePlusException;

class HAlternative : public HRule {
	typedef yaal::hcore::HList<HRule::ptr_t> rules_t;
	rules_t _rules;
public:
	typedef HAlternative this_type;
	typedef HRule base_type;
	HAlternative( HRule const& choice1_, HRule const& choice2_ );
	HAlternative( HAlternative const& alternative_ );
	HAlternative( HAlternative const& alternative_, HRule const& choice_ );
protected:
	virtual ptr_t do_clone( void ) const;
};

typedef yaal::hcore::HExceptionT<HAlternative, HRuleException> HAlternativeException;

class HOptional : public HRule {
protected:
	virtual bool do_is_optional( void ) const;
};

typedef yaal::hcore::HExceptionT<HOptional, HRuleException> HOptionalException;

HFollows operator >> ( HRule const& predecessor_, HRule const& successor_ );
HFollows operator >> ( HFollows const& predecessors_, HRule const& successor_ );
HAlternative operator | ( HRule const& choice1_, HRule const& choice2_ );
HAlternative operator | ( HAlternative const& alternative_, HRule const& choice_ );
HKleeneStar operator* ( HRule const& rule_ );
HKleenePlus operator+ ( HRule const& rule_ );

class HReal : public HRule {
	typedef yaal::hcore::HBoundCall<void ( double )> action_double_t;
	typedef yaal::hcore::HBoundCall<void ( double long )> action_double_long_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HNumber const& )> action_number_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	action_double_t _actionDouble;
	action_double_long_t _actionDoubleLong;
	action_number_t _actionNumber;
	action_string_t _actionString;
	yaal::hcore::HString _cache;
	typedef enum {
		START = 0,
		MINUS = 1,
		INTEGRAL = 2,
		DOT = 3,
		DECIMAL = 4
	} real_paring_state_t;
public:
	typedef HReal this_type;
	typedef HRule base_type;
	HReal( void );
	HReal( action_double_t actionDouble_, action_double_long_t actionDoubleLong_,
			action_number_t actionNumber_, action_string_t actionString_ );
	HReal( HReal const& real_ );
	using HRule::operator[];
	HReal operator[]( action_double_t const& action_ );
	HReal operator[]( action_double_long_t const& action_ );
	HReal operator[]( action_number_t const& action_ );
	HReal operator[]( action_string_t const& action_ );
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
protected:
	virtual ptr_t do_clone( void ) const;
};

extern HReal real;

typedef yaal::hcore::HExceptionT<HReal, HRuleException> HRealException;

class HInteger : public HRule {
	typedef yaal::hcore::HBoundCall<void ( int long )> action_int_long_t;
	typedef yaal::hcore::HBoundCall<void ( int )> action_int_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HNumber const& )> action_number_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	action_int_long_t _actionIntLong;
	action_int_t _actionInt;
	action_number_t _actionNumber;
	action_string_t _actionString;
	yaal::hcore::HString _cache;
	typedef enum {
		START = 0,
		MINUS = 1,
		DIGIT = 2
	} real_paring_state_t;
public:
	typedef HInteger this_type;
	typedef HRule base_type;
	HInteger( void );
	HInteger( action_int_long_t actionDouble_, action_int_t actionDoubleLong_,
			action_number_t actionNumber_, action_string_t actionString_ );
	HInteger( HInteger const& real_ );
	using HRule::operator[];
	HInteger operator[]( action_int_long_t const& action_ );
	HInteger operator[]( action_int_t const& action_ );
	HInteger operator[]( action_number_t const& action_ );
	HInteger operator[]( action_string_t const& action_ );
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
protected:
	virtual ptr_t do_clone( void ) const;
};

extern HInteger integer;

typedef yaal::hcore::HExceptionT<HInteger, HRuleException> HIntegerException;

class HCharacter : public HRule {
	typedef yaal::hcore::HBoundCall<void ( char )> action_t;
	char _character;
	action_t _action;
public:
	typedef HCharacter this_type;
	typedef HRule base_type;
	HCharacter( char character_ = 0 );
	HCharacter( char character_, action_t action_ );
	HCharacter( HCharacter const& character_ );
	virtual ~HCharacter( void )
		{}
	using HRule::operator[];
	HCharacter operator[]( action_t const& action_ );
	HCharacter operator() ( char character_ );
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
protected:
	virtual ptr_t do_clone( void ) const;
};
extern HCharacter character;

typedef yaal::hcore::HExceptionT<HCharacter, HRuleException> HCharacterException;

HFollows operator >> ( char character_, HRule const& successor_ );
HFollows operator >> ( HRule const& successor_, char );

class HString : public HRule {
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_t;
	yaal::hcore::HString _string;
	action_t _action;
public:
	typedef HString this_type;
	typedef HRule base_type;
	HString( yaal::hcore::HString const& string_ = yaal::hcore::HString() );
	HString( yaal::hcore::HString const& string_, action_t action_ );
	HString( HString const& string_ );
	virtual ~HString( void )
		{}
	using HRule::operator[];
	HString operator[]( action_t const& action_ );
	HString operator() ( yaal::hcore::HString const& string_ );
	virtual yaal::hcore::HString::const_iterator do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ );
protected:
	virtual ptr_t do_clone( void ) const;
};

extern HString string;

typedef yaal::hcore::HExceptionT<HString, HRuleException> HStringException;

HFollows operator >> ( char const* string_, HRule const& successor_ );
HFollows operator >> ( yaal::hcore::HString const& string_, HRule const& successor_ );
HFollows operator >> ( HRule const& predecessor_, char const* string_ );
HFollows operator >> ( HRule const& predecessor_, yaal::hcore::HString const& string_ );

}

}

}

#endif /* #ifndef YAAL_TOOLS_EXECUTINGPARSER_HXX_INCLUDED */

