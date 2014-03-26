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
#include "hcore/hregex.hxx"
#include "hcore/harray.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hset.hxx"
#include "hcore/hmap.hxx"
#include "hcore/hqueue.hxx"
#include "hcore/hboundcall.hxx"

namespace yaal {

namespace tools {

class HExecutingParser;

/*! \brief Namespace containing classes composing executing parser infrastructure.
 *
 * Executing parser component, besides Parser and Executor part,
 * defines embedded grammar declaration language, again!,
 * this language is purely functional, there is no concept of state or values here.
 *
 * Once a grammar rule is declared it cannot be modified unless first declaration was empty
 * and following definition uses %= oparator to denote creation of recursive rule,
 * still %= operator can be used only once per rule.
 */
namespace executing_parser {

class HGrammarDescription;
class HRuleBase;
class HNamedRule;
typedef yaal::hcore::HSet<HRuleBase const*> visited_t;
typedef yaal::hcore::HMap<HRuleBase const*, int> rule_use_t;

class HRuleDescription {
public:
	typedef HRuleDescription this_type;
	typedef yaal::hcore::HMap<HRuleBase const*, yaal::hcore::HString> automatic_names_t;
private:
	typedef yaal::hcore::HArray<HNamedRule const*> named_children_t;
	named_children_t _children;
	yaal::hcore::HString _description;
	automatic_names_t _automaticNames;
public:
	HRuleDescription( void );
	void desc( yaal::hcore::HString const& );
	void add( HNamedRule const* );
	yaal::hcore::HString const& make_name( HNamedRule const& );
	named_children_t const& children( void ) const;
	yaal::hcore::HString const& description( void ) const;
	void clear( void ); 
};

class HNamedRule {
public:
	typedef HNamedRule this_type;
	typedef yaal::hcore::HPointer<HRuleBase> ptr_t;
private:
	yaal::hcore::HString _name;
	ptr_t _rule;
public:
	HNamedRule( void )
		: _name(), _rule()
		{	}
	HNamedRule( ptr_t rule_ )
		: _name(), _rule( rule_ )
		{	}
	HNamedRule( yaal::hcore::HString const&, ptr_t );
	HNamedRule( HRuleBase const& );
	bool operator ! ( void ) const
		{ return ( ! _rule ); }
	ptr_t const& operator->( void ) const
		{ return ( _rule ); }
	ptr_t& operator->( void )
		{ return ( _rule ); }
	HRuleBase const* id( void ) const;
	yaal::hcore::HString const& name( void ) const;
	ptr_t const& rule( void ) const;
	ptr_t& rule( void );
	void reset( ptr_t const& );
	void describe( HRuleDescription&, rule_use_t const& ) const;
};

class HRuleBase {
public:
	typedef HRuleBase this_type;
	typedef HNamedRule::ptr_t ptr_t;
	typedef yaal::hcore::HBoundCall<> action_t;
protected:
	action_t _action;
public:
	HRuleBase( void );
	HRuleBase( action_t const& );
	virtual ~HRuleBase( void )
		{}
	ptr_t clone( void ) const;
	bool is_optional( void ) const;
	yaal::hcore::HString::const_iterator parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	void describe( HRuleDescription&, rule_use_t const& ) const;
	void detach( HRuleBase const*, visited_t& );
	void rule_use( rule_use_t& ) const;
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) = 0;
	virtual ptr_t do_clone( void ) const = 0;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const = 0;
	virtual void do_detach( HRuleBase const*, visited_t& ) = 0;
	static yaal::hcore::HString::const_iterator skip_space( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	friend class yaal::tools::HExecutingParser;
private:
	HRuleBase& operator = ( HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HRuleBase> HRuleBaseException;

class HGrammarDescription {
public:
	typedef HGrammarDescription this_type;
private:
	typedef yaal::hcore::HArray<yaal::hcore::HString> rules_t;
	typedef yaal::hcore::HQueue<HNamedRule const*> rule_order_t;
	rules_t _rules;
	visited_t _visited;
	rule_order_t _ruleOrder;
public:
	typedef rules_t::const_iterator const_iterator;
	HGrammarDescription( HRuleBase const& );
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	bool is_empty( void ) const;
private:
	friend class HRuleBase;
	friend class HRecursiveRule;
	friend class HFollows;
};

typedef yaal::hcore::HExceptionT<HGrammarDescription> HGrammarDescriptionException;

/*! \brief Abstract rule holder.
 *
 * Any rule can be possibly a recursive rule.
 */
class HRule : public HRuleBase {
public:
	typedef HRule this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
	bool _completelyDefined;
public:
	HRule( void );
	HRule( yaal::hcore::HString const& );
	HRule( HRule const& );
	HRule( HRuleBase const& );
	HRule( ptr_t const& );
	HRule( yaal::hcore::HString const&, HRuleBase const& );
	HRule( yaal::hcore::HString const&, ptr_t const& );
	virtual ~HRule( void );
	HRule operator[]( action_t const& ) const;
	HRule& operator %= ( HRuleBase const& );
	yaal::hcore::HString const& get_name( void ) const;
protected:
	HRule( yaal::hcore::HString const&, ptr_t const&, action_t const& );
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HRule& operator = ( HRule const& );
	template<typename tType, typename a0_t, typename a1_t, typename a2_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( a0_t const&, a1_t const&, a2_t const& );
};

typedef yaal::hcore::HExceptionT<HRule, HRuleBaseException> HRuleException;

class HRecursiveRule : public HRuleBase, public yaal::hcore::HPointerFromThisInterface<HRecursiveRule> {
public:
	typedef HRecursiveRule this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual HRuleBase::ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HRecursiveRule( void );
	void set_rule( HRuleBase::ptr_t const& );
	HRecursiveRule( HRecursiveRule const& );
	HRecursiveRule& operator = ( HRecursiveRule const& );
	friend class HRule;
	friend class HRuleRef;
	friend yaal::hcore::HPointer<HRecursiveRule> yaal::hcore::make_pointer<HRecursiveRule>( void );
};

typedef yaal::hcore::HExceptionT<HRecursiveRule, HRuleBaseException> HRecursiveRuleException;

class HRuleRef : public HRuleBase, public yaal::hcore::HPointerFromThisInterface<HRecursiveRule> {
public:
	typedef HRuleRef this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HPointer<HRuleBase, yaal::hcore::HPointerScalar, yaal::hcore::HPointerWeak> rule_ref_t;
	rule_ref_t _rule;
public:
	HRuleRef( HRuleBase::ptr_t );
	HRuleBase const* get_rule( void ) const;
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual HRuleBase::ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const __attribute__((__noreturn__));
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
};

class HFollows;
class HKleeneStar;
class HKleenePlus;
class HAlternative;
class HOptional;

HFollows operator >> ( HRuleBase const&, HRuleBase const& );
HFollows operator >> ( HFollows const&, HRuleBase const& );
HFollows operator >> ( char const*, HRuleBase const& );
HFollows operator >> ( yaal::hcore::HString const&, HRuleBase const& );
HFollows operator >> ( HRuleBase const&, char const* );
HFollows operator >> ( HRuleBase const&, yaal::hcore::HString const& );
HFollows operator >> ( char character_, HRuleBase const& successor_ );
HFollows operator >> ( HRuleBase const& successor_, char );
HAlternative operator | ( HRuleBase const&, HRuleBase const& );
HAlternative operator | ( HAlternative const&, HRuleBase const& );
HKleeneStar operator* ( HRuleBase const& );
HKleenePlus operator+ ( HRuleBase const& );
HOptional operator - ( HRuleBase const& );

class HFollows : public HRuleBase {
public:
	typedef HFollows this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HList<HNamedRule> rules_t;
	rules_t _rules;
public:
	HFollows( HFollows const& );
	virtual ~HFollows( void )
		{}
	HFollows operator[]( action_t const& ) const;
protected:
	HFollows( rules_t const&, action_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HFollows( HRuleBase const&, HRuleBase const& );
	HFollows( HFollows const&, HRuleBase const& );
	HFollows& operator = ( HFollows const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::hcore::HString const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HFollows const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( char, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, char );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( char const*, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, char const* );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, yaal::hcore::HString const& );
};

typedef yaal::hcore::HExceptionT<HFollows, HRuleBaseException> HFollowsException;

class HKleeneStar : public HRuleBase {
public:
	typedef HKleeneStar this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
public:
	HKleeneStar( HKleeneStar const& );
	HKleeneStar operator[]( action_t const& ) const;
	virtual ~HKleeneStar( void )
		{}
protected:
	HKleeneStar( HNamedRule const&, action_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HKleeneStar( HRuleBase const& );
	HKleeneStar& operator = ( HKleeneStar const& );
	friend yaal::tools::executing_parser::HKleeneStar yaal::tools::executing_parser::operator * ( yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HKleeneStar, HRuleBaseException> HKleeneStarException;

class HKleenePlus : public HRuleBase {
public:
	typedef HKleenePlus this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
public:
	HKleenePlus( HKleenePlus const& kleenePlus_ );
	HKleenePlus operator[]( action_t const& ) const;
	virtual ~HKleenePlus( void )
		{}
protected:
	HKleenePlus( HNamedRule const&, action_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HKleenePlus( HRuleBase const& rule_ );
	HKleenePlus& operator = ( HKleenePlus const& );
	friend yaal::tools::executing_parser::HKleenePlus yaal::tools::executing_parser::operator + ( yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HKleenePlus, HRuleBaseException> HKleenePlusException;

class HAlternative : public HRuleBase {
public:
	typedef HAlternative this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HList<HNamedRule> rules_t;
	rules_t _rules;
public:
	HAlternative( HAlternative const& );
	HAlternative operator[]( action_t const& ) const;
protected:
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HAlternative( HRuleBase const&, HRuleBase const& );
	HAlternative( HAlternative const&, HRuleBase const& );
	HAlternative( rules_t const&, action_t const& );
	HAlternative& operator = ( HAlternative const& );
	friend yaal::tools::executing_parser::HAlternative yaal::tools::executing_parser::operator | ( yaal::tools::executing_parser::HRuleBase const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HAlternative yaal::tools::executing_parser::operator | ( yaal::tools::executing_parser::HAlternative const&, yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HAlternative, HRuleBaseException> HAlternativeException;

class HOptional : public HRuleBase {
public:
	typedef HOptional this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
public:
	HOptional( HOptional const& );
	HOptional operator[]( action_t const& ) const;
protected:
	HOptional( HNamedRule const&, action_t const& );
	virtual bool do_is_optional( void ) const;
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HOptional( HRuleBase const& );
	HOptional& operator = ( HOptional const& );
	friend yaal::tools::executing_parser::HOptional yaal::tools::executing_parser::operator - ( yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HOptional, HRuleBaseException> HOptionalException;

class HReal : public HRuleBase {
public:
	typedef HReal this_type;
	typedef HRuleBase base_type;
private:
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
	HReal( HReal const& );
	HReal operator[]( action_t const& ) const;
	HReal operator[]( action_double_t const& ) const;
	HReal operator[]( action_double_long_t const& ) const;
	HReal operator[]( action_number_t const& ) const;
	HReal operator[]( action_string_t const& ) const;
protected:
	HReal( action_t const& );
	HReal( action_double_t const& );
	HReal( action_double_long_t const& );
	HReal( action_number_t const& );
	HReal( action_string_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HReal( void );
	HReal& operator = ( HReal const& );
	friend HReal const& get_real_instance( void );
	template<typename tType, typename a0_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( a0_t const& );
};

extern M_YAAL_TOOLS_PUBLIC_API HReal const& real;

typedef yaal::hcore::HExceptionT<HReal, HRuleBaseException> HRealException;

class HInteger : public HRuleBase {
public:
	typedef HInteger this_type;
	typedef HRuleBase base_type;
private:
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
	HInteger( HInteger const& );
	HInteger operator[]( action_t const& ) const;
	HInteger operator[]( action_int_long_t const& ) const;
	HInteger operator[]( action_int_t const& ) const;
	HInteger operator[]( action_number_t const& ) const;
	HInteger operator[]( action_string_t const& ) const;
protected:
	HInteger( action_t const& );
	HInteger( action_int_long_t const& );
	HInteger( action_int_t const& );
	HInteger( action_number_t const& );
	HInteger( action_string_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HInteger( void );
	HInteger& operator = ( HInteger const& );
	friend HInteger const& get_integer_instance( void );
};

extern M_YAAL_TOOLS_PUBLIC_API HInteger const& integer;

typedef yaal::hcore::HExceptionT<HInteger, HRuleBaseException> HIntegerException;

class HCharacter : public HRuleBase {
public:
	typedef HCharacter this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HBoundCall<void ( char )> action_char_t;
	char _character;
	action_char_t _actionChar;
public:
	HCharacter( HCharacter const& character_ );
	virtual ~HCharacter( void )
		{}
	HCharacter operator[]( action_t const& ) const;
	HCharacter operator[]( action_char_t const& ) const;
	HCharacter operator() ( char ) const;
protected:
	HCharacter( char, action_t const& );
	HCharacter( char, action_char_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HCharacter( char character_ = 0 );
	HCharacter& operator = ( HCharacter const& );
	friend HCharacter const& get_character_instance( void );
};

extern M_YAAL_TOOLS_PUBLIC_API HCharacter const& character;

typedef yaal::hcore::HExceptionT<HCharacter, HRuleBaseException> HCharacterException;

class HString : public HRuleBase {
public:
	typedef HString this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	yaal::hcore::HString _string;
	action_string_t _actionString;
public:
	HString( HString const& );
	virtual ~HString( void )
		{}
	HString operator[]( action_t const& ) const;
	HString operator[]( action_string_t const& ) const;
	HString operator() ( yaal::hcore::HString const& ) const;
protected:
	HString( yaal::hcore::HString const&, action_t const& );
	HString( yaal::hcore::HString const&, action_string_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HString( yaal::hcore::HString const& );
	HString& operator = ( HString const& );
	friend HString string( yaal::hcore::HString const& );
};

HString string( yaal::hcore::HString const& );

typedef yaal::hcore::HExceptionT<HString, HRuleBaseException> HStringException;

class HRegex : public HRuleBase {
public:
	typedef HRegex this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	typedef yaal::hcore::HPointer<yaal::hcore::HRegex> regex_t;
	regex_t _regex;
	action_string_t _actionString;
public:
	HRegex( HRegex const& );
	virtual ~HRegex( void )
		{}
	HRegex operator[]( action_t const& ) const;
	HRegex operator[]( action_string_t const& ) const;
protected:
	HRegex( regex_t const&, action_t const& );
	HRegex( regex_t const&, action_string_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t& );
private:
	HRegex( yaal::hcore::HString const& );
	HRegex& operator = ( HRegex const& );
	friend HRegex regex( yaal::hcore::HString const& );
};

typedef yaal::hcore::HExceptionT<HRegex, HRuleBaseException> HRegexException;

HCharacter constant( char );
HString constant( yaal::hcore::HString const& );
HRegex regex( yaal::hcore::HString const& );

}

class HExecutingParser : private trait::HNonCopyable {
public:
	typedef HExecutingParser this_type;
	typedef yaal::hcore::HBoundCall<> executor_t;
private:
	typedef executing_parser::HRuleBase::ptr_t grammar_t;
	typedef yaal::hcore::HPair<yaal::hcore::HString::const_iterator, executor_t> execution_step_t;
	typedef yaal::hcore::HArray<execution_step_t> execution_steps_t;
	grammar_t _grammar;
	execution_steps_t _excutors;
	bool _matched;
public:
	HExecutingParser( executing_parser::HRuleBase const& );
	bool operator()( yaal::hcore::HString const& );
	bool operator()( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	void operator()( void );
	void execute( void );
	void sanitize( void );
	yaal::hcore::HString::const_iterator parse( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	void add_execution_step( yaal::hcore::HString::const_iterator, executor_t const& );
	void drop_execution_steps( yaal::hcore::HString::const_iterator );
};

typedef yaal::hcore::HExceptionT<HExecutingParser> HExecutingParserException;

}

}

#endif /* #ifndef YAAL_TOOLS_EXECUTINGPARSER_HXX_INCLUDED */

