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
#include "hcore/hhashset.hxx"
#include "hcore/hhashmap.hxx"
#include "hcore/hqueue.hxx"
#include "hcore/hboundcall.hxx"
#include "hcore/htaggedpod.hxx"

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
class HRecursionDetector;
class HRuleAggregator;
typedef yaal::hcore::HExceptionT<HRecursionDetector> HRecursionDetectorException;
typedef yaal::hcore::HHashSet<HRuleBase const*> visited_t;
typedef yaal::hcore::HHashMap<HRuleBase const*, int> rule_use_t;
struct Position {};
typedef yaal::hcore::HTaggedPOD<int, Position> position_t;

class HRuleDescription {
public:
	typedef HRuleDescription this_type;
	typedef yaal::hcore::HHashMap<HRuleBase const*, yaal::hcore::HString> automatic_names_t;
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
	yaal::hcore::HString const& make_name_auto( HRuleBase const* );
	named_children_t const& children( void ) const;
	yaal::hcore::HString const& description( void ) const;
	void clear( void );
	void reset_names( void );
};

typedef yaal::hcore::HExceptionT<HRuleDescription> HRuleDescriptionException;

class HNamedRule {
public:
	typedef HNamedRule this_type;
	typedef yaal::hcore::HPointer<HRuleBase> ptr_t;
private:
	yaal::hcore::HString _name;
	ptr_t _rule;
public:
	HNamedRule( void )
		: _name(), _rule() {
		return;
	}
	HNamedRule( ptr_t rule_ )
		: _name(), _rule( rule_ ) {
		return;
	}
	HNamedRule( yaal::hcore::HString const&, ptr_t );
	HNamedRule( HRuleBase const& );
	bool operator ! ( void ) const {
		return ( ! _rule );
	}
	ptr_t const& operator->( void ) const {
		return ( _rule );
	}
	ptr_t& operator->( void ) {
		return ( _rule );
	}
	HRuleBase const* id( void ) const;
	yaal::hcore::HString const& name( void ) const;
	ptr_t const& rule( void ) const;
	ptr_t& rule( void );
	void reset( ptr_t const& );
	void describe( HRuleDescription&, rule_use_t const& ) const;
};

typedef yaal::hcore::HExceptionT<HNamedRule> HNamedRuleException;

class HRuleBase {
public:
	typedef HRuleBase this_type;
	typedef HNamedRule::ptr_t ptr_t;
	typedef yaal::hcore::HBoundCall<> action_t;
	typedef yaal::hcore::HBoundCall<void ( position_t )> action_position_t;
protected:
	action_t _action;
	action_position_t _actionPosition;
public:
	HRuleBase( void );
	HRuleBase( action_t const& );
	HRuleBase( action_position_t const& );
	HRuleBase( action_t const&, action_position_t const& ); /*!< for use in copy constructor of derived class */
	virtual ~HRuleBase( void ) {
		return;
	}
	ptr_t clone( void ) const;
	bool is_optional( void ) const;
	yaal::hcore::HString::const_iterator parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	void describe( HRuleDescription&, rule_use_t const& ) const;
	void detach( HRuleBase const*, visited_t&, bool& );
	void rule_use( rule_use_t& ) const;
	void detect_recursion( HRecursionDetector& ) const;
	void find_recursions( HRuleAggregator& );
	bool has_action( void ) const;
	static yaal::hcore::HString::const_iterator skip_space( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const = 0;
	virtual ptr_t do_clone( void ) const = 0;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const = 0;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& ) = 0;
	virtual void do_detect_recursion( HRecursionDetector& ) const = 0;
	virtual void do_find_recursions( HRuleAggregator& ) = 0;
	virtual bool do_has_action( void ) const;
	void add_execution_step( HExecutingParser*, yaal::hcore::HString::const_iterator, action_t const& ) const;
	void report_error( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString const& ) const;
	int position( HExecutingParser*, yaal::hcore::HString::const_iterator ) const;
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
	typedef yaal::hcore::HHashSet<yaal::hcore::HString> visited_t;
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
	HRule( action_t const& );
	HRule( action_position_t const& );
	HRule( yaal::hcore::HString const& );
	HRule( yaal::hcore::HString const&, action_t const& );
	HRule( yaal::hcore::HString const&, action_position_t const& );
	HRule( HRule const& );
	HRule( HRuleBase const& );
	HRule( HRuleBase const&, action_t const& );
	HRule( HRuleBase const&, action_position_t const& );
	HRule( ptr_t const& );
	HRule( yaal::hcore::HString const&, HRuleBase const& );
	HRule( yaal::hcore::HString const&, HRuleBase const&, action_t const& );
	HRule( yaal::hcore::HString const&, HRuleBase const&, action_position_t const& );
	HRule( yaal::hcore::HString const&, ptr_t const& );
	virtual ~HRule( void );
	HRule operator[]( action_t const& ) const;
	HRule operator[]( action_position_t const& ) const;
	HRule& operator %= ( HRuleBase const& );
	yaal::hcore::HString const& get_name( void ) const;
	HNamedRule const* get_named_rule( void ) const;
protected:
	HRule( yaal::hcore::HString const&, ptr_t const&, action_t const& );
	HRule( yaal::hcore::HString const&, ptr_t const&, action_position_t const& );
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HRule& operator = ( HRule const& );
	template<typename tType, typename... arg_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( arg_t&&... );
};

typedef yaal::hcore::HExceptionT<HRule, HRuleBaseException> HRuleException;

class HRecursiveRule : public HRuleBase, public yaal::hcore::HPointerFromThisInterface<HRecursiveRule> {
public:
	typedef HRecursiveRule this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual HRuleBase::ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
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

class HRuleRef : public HRuleBase, public yaal::hcore::HPointerFromThisInterface<HRuleRef> {
public:
	typedef HRuleRef this_type;
	typedef HRuleBase base_type;
private:
	typedef yaal::hcore::HPointer<HRuleBase, yaal::hcore::HPointerScalar, yaal::hcore::HPointerWeak> rule_ref_t;
	rule_ref_t _rule;
public:
	HRuleRef( HRuleBase::ptr_t );
	HRuleBase::ptr_t get_rule( void ) const;
protected:
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual HRuleBase::ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const M_DEBUG_CODE( __attribute__((__noreturn__)) );
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
};

class HFollows;
class HKleeneStar;
class HKleenePlus;
class HAlternative;
class HOptional;
class HAnd;
class HNot;

HFollows operator >> ( HRuleBase const&, HRuleBase const& );
HFollows operator >> ( HFollows const&, HRuleBase const& );
HFollows operator >> ( char const*, HRuleBase const& );
HFollows operator >> ( yaal::hcore::HString const&, HRuleBase const& );
HFollows operator >> ( HRuleBase const&, char const* );
HFollows operator >> ( HRuleBase const&, yaal::hcore::HString const& );
HFollows operator >> ( HFollows const&, char const* );
HFollows operator >> ( HFollows const&, yaal::hcore::HString const& );
HFollows operator >> ( char, HRuleBase const& );
HFollows operator >> ( HFollows const&, char );
HFollows operator >> ( HRuleBase const&, char );
HAlternative operator | ( HRuleBase const&, HRuleBase const& );
HAlternative operator | ( HAlternative const&, HRuleBase const& );
HAnd operator & ( HRuleBase const&, HRuleBase const& );
HAnd operator & ( HRuleBase const&, char );
HAnd operator & ( HRuleBase const&, char const* );
HAnd operator & ( HRuleBase const&, yaal::hcore::HString const& );
HNot operator ^ ( HRuleBase const&, HRuleBase const& );
HNot operator ^ ( HRuleBase const&, char );
HNot operator ^ ( HRuleBase const&, char const* );
HNot operator ^ ( HRuleBase const&, yaal::hcore::HString const& );
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
	virtual ~HFollows( void ) {
		return;
	}
	HFollows operator[]( action_t const& ) const;
	HFollows operator[]( action_position_t const& ) const;
protected:
	HFollows( rules_t const&, action_t const& );
	HFollows( rules_t const&, action_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HFollows( HRuleBase const&, HRuleBase const& );
	HFollows( HFollows const&, HRuleBase const& );
	HFollows& operator = ( HFollows const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::hcore::HString const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HFollows const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( char, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, char );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HFollows const&, char );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( char const*, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, char const* );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HRuleBase const&, yaal::hcore::HString const& );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HFollows const&, char const* );
	friend yaal::tools::executing_parser::HFollows yaal::tools::executing_parser::operator >> ( yaal::tools::executing_parser::HFollows const&, yaal::hcore::HString const& );
};

typedef yaal::hcore::HExceptionT<HFollows, HRuleBaseException> HFollowsException;

class HKleeneBase : public HRuleBase {
public:
	typedef HKleeneBase this_type;
	typedef HRuleBase base_type;
protected:
	HNamedRule _rule;
public:
	virtual ~HKleeneBase( void ) {
		return;
	}
protected:
	HKleeneBase( void );
	HKleeneBase( HRuleBase const& );
	HKleeneBase( HNamedRule const&, action_t const& );
	HKleeneBase( HNamedRule const&, action_position_t const& );
	HKleeneBase( HNamedRule const&, action_t const&, action_position_t const& ); /*!< for use in copy constructor of derived class */
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HKleeneBase( HKleeneBase const& );
	HKleeneBase& operator = ( HKleeneBase const& );
};

typedef yaal::hcore::HExceptionT<HKleeneBase, HRuleBaseException> HKleeneBaseException;

class HKleeneStar : public HKleeneBase {
public:
	typedef HKleeneStar this_type;
	typedef HKleeneBase base_type;
public:
	HKleeneStar( HKleeneStar const& );
	HKleeneStar operator[]( action_t const& ) const;
	HKleeneStar operator[]( action_position_t const& ) const;
	virtual ~HKleeneStar( void ) {
		return;
	}
protected:
	HKleeneStar( HNamedRule const&, action_t const& );
	HKleeneStar( HNamedRule const&, action_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual bool do_is_optional( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detect_recursion( HRecursionDetector& ) const;
private:
	HKleeneStar( HRuleBase const& );
	HKleeneStar& operator = ( HKleeneStar const& );
	friend yaal::tools::executing_parser::HKleeneStar yaal::tools::executing_parser::operator * ( yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HKleeneStar, HKleeneBaseException> HKleeneStarException;

class HKleenePlus : public HKleeneBase {
public:
	typedef HKleenePlus this_type;
	typedef HKleeneBase base_type;
public:
	HKleenePlus( HKleenePlus const& kleenePlus_ );
	HKleenePlus operator[]( action_t const& ) const;
	HKleenePlus operator[]( action_position_t const& ) const;
	virtual ~HKleenePlus( void ) {
		return;
	}
protected:
	HKleenePlus( HNamedRule const&, action_t const& );
	HKleenePlus( HNamedRule const&, action_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detect_recursion( HRecursionDetector& ) const;
private:
	HKleenePlus( HRuleBase const& rule_ );
	HKleenePlus& operator = ( HKleenePlus const& );
	friend yaal::tools::executing_parser::HKleenePlus yaal::tools::executing_parser::operator + ( yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HKleenePlus, HKleeneBaseException> HKleenePlusException;

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
	HAlternative operator[]( action_position_t const& ) const;
protected:
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HAlternative( HRuleBase const&, HRuleBase const& );
	HAlternative( HAlternative const&, HRuleBase const& );
	HAlternative( rules_t const&, action_t const& );
	HAlternative( rules_t const&, action_position_t const& );
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
	HOptional operator[]( action_position_t const& ) const;
protected:
	HOptional( HNamedRule const&, action_t const& );
	HOptional( HNamedRule const&, action_position_t const& );
	virtual bool do_is_optional( void ) const;
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HOptional( HRuleBase const& );
	HOptional& operator = ( HOptional const& );
	friend yaal::tools::executing_parser::HOptional yaal::tools::executing_parser::operator - ( yaal::tools::executing_parser::HRuleBase const& );
};

typedef yaal::hcore::HExceptionT<HOptional, HRuleBaseException> HOptionalException;

class HAnd : public HRuleBase {
public:
	typedef HAnd this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
	HNamedRule _and;
public:
	HAnd( HAnd const& );
	HAnd operator[]( action_t const& ) const;
	HAnd operator[]( action_position_t const& ) const;
protected:
	HAnd( HNamedRule const&, HNamedRule const&, action_t const& );
	HAnd( HNamedRule const&, HNamedRule const&, action_position_t const& );
	virtual bool do_is_optional( void ) const;
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HAnd( HRuleBase const&, HRuleBase const& );
	HAnd& operator = ( HAnd const& ) = delete;
	friend yaal::tools::executing_parser::HAnd yaal::tools::executing_parser::operator & ( yaal::tools::executing_parser::HRuleBase const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HAnd yaal::tools::executing_parser::operator & ( yaal::tools::executing_parser::HRuleBase const&, char );
	friend yaal::tools::executing_parser::HAnd yaal::tools::executing_parser::operator & ( yaal::tools::executing_parser::HRuleBase const&, char const* );
	friend yaal::tools::executing_parser::HAnd yaal::tools::executing_parser::operator & ( yaal::tools::executing_parser::HRuleBase const&, yaal::hcore::HString const& );
};

typedef yaal::hcore::HExceptionT<HAnd, HRuleBaseException> HAndException;

class HNot : public HRuleBase {
public:
	typedef HNot this_type;
	typedef HRuleBase base_type;
private:
	HNamedRule _rule;
	HNamedRule _not;
public:
	HNot( HNot const& );
	HNot operator[]( action_t const& ) const;
	HNot operator[]( action_position_t const& ) const;
protected:
	HNot( HNamedRule const&, HNamedRule const&, action_t const& );
	HNot( HNamedRule const&, HNamedRule const&, action_position_t const& );
	virtual bool do_is_optional( void ) const;
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_rule_use( rule_use_t& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
private:
	HNot( HRuleBase const&, HRuleBase const& );
	HNot& operator = ( HNot const& ) = delete;
	friend yaal::tools::executing_parser::HNot yaal::tools::executing_parser::operator ^ ( yaal::tools::executing_parser::HRuleBase const&, yaal::tools::executing_parser::HRuleBase const& );
	friend yaal::tools::executing_parser::HNot yaal::tools::executing_parser::operator ^ ( yaal::tools::executing_parser::HRuleBase const&, char );
	friend yaal::tools::executing_parser::HNot yaal::tools::executing_parser::operator ^ ( yaal::tools::executing_parser::HRuleBase const&, char const* );
	friend yaal::tools::executing_parser::HNot yaal::tools::executing_parser::operator ^ ( yaal::tools::executing_parser::HRuleBase const&, yaal::hcore::HString const& );
};

typedef yaal::hcore::HExceptionT<HNot, HRuleBaseException> HNotException;

class HReal : public HRuleBase {
public:
	typedef HReal this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( double )> action_double_t;
	typedef yaal::hcore::HBoundCall<void ( double, position_t )> action_double_position_t;
	typedef yaal::hcore::HBoundCall<void ( double long )> action_double_long_t;
	typedef yaal::hcore::HBoundCall<void ( double long, position_t )> action_double_long_position_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HNumber const& )> action_number_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HNumber const&, position_t )> action_number_position_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const&, position_t )> action_string_position_t;
	enum class PARSE {
		GREEDY,
		STRICT
	};
private:
	action_double_t _actionDouble;
	action_double_position_t _actionDoublePosition;
	action_double_long_t _actionDoubleLong;
	action_double_long_position_t _actionDoubleLongPosition;
	action_number_t _actionNumber;
	action_number_position_t _actionNumberPosition;
	action_string_t _actionString;
	action_string_position_t _actionStringPosition;
	PARSE _parse;
	mutable yaal::hcore::HString _cache;
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
	HReal operator[]( action_position_t const& ) const;
	HReal operator[]( action_double_t const& ) const;
	HReal operator[]( action_double_position_t const& ) const;
	HReal operator[]( action_double_long_t const& ) const;
	HReal operator[]( action_double_long_position_t const& ) const;
	HReal operator[]( action_number_t const& ) const;
	HReal operator[]( action_number_position_t const& ) const;
	HReal operator[]( action_string_t const& ) const;
	HReal operator[]( action_string_position_t const& ) const;
	HReal operator() ( PARSE ) const;
protected:
	HReal( action_t const&, PARSE = PARSE::GREEDY );
	HReal( action_position_t const&, PARSE = PARSE::GREEDY );
	HReal( action_double_t const&, PARSE = PARSE::GREEDY );
	HReal( action_double_position_t const&, PARSE = PARSE::GREEDY );
	HReal( action_double_long_t const&, PARSE = PARSE::GREEDY );
	HReal( action_double_long_position_t const&, PARSE = PARSE::GREEDY );
	HReal( action_number_t const&, PARSE = PARSE::GREEDY );
	HReal( action_number_position_t const&, PARSE = PARSE::GREEDY );
	HReal( action_string_t const&, PARSE = PARSE::GREEDY );
	HReal( action_string_position_t const&, PARSE = PARSE::GREEDY );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HReal( PARSE = PARSE::GREEDY );
	HReal& operator = ( HReal const& );
	friend HReal const& get_real_instance( void );
	template<typename tType, typename a0_t>
	friend yaal::hcore::HPointer<tType> yaal::hcore::make_pointer( a0_t const&& );
};

typedef yaal::hcore::HExceptionT<HReal, HRuleBaseException> HRealException;

extern M_YAAL_TOOLS_PUBLIC_API HReal const& real;

class HInteger : public HRuleBase {
public:
	typedef HInteger this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( int long long )> action_int_long_long_t;
	typedef yaal::hcore::HBoundCall<void ( int long long, position_t )> action_int_long_long_position_t;
	typedef yaal::hcore::HBoundCall<void ( int long )> action_int_long_t;
	typedef yaal::hcore::HBoundCall<void ( int long, position_t )> action_int_long_position_t;
	typedef yaal::hcore::HBoundCall<void ( int )> action_int_t;
	typedef yaal::hcore::HBoundCall<void ( int, position_t )> action_int_position_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HNumber const& )> action_number_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HNumber const&, position_t )> action_number_position_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const&, position_t )> action_string_position_t;
private:
	action_int_long_long_t _actionIntLongLong;
	action_int_long_long_position_t _actionIntLongLongPosition;
	action_int_long_t _actionIntLong;
	action_int_long_position_t _actionIntLongPosition;
	action_int_t _actionInt;
	action_int_position_t _actionIntPosition;
	action_number_t _actionNumber;
	action_number_position_t _actionNumberPosition;
	action_string_t _actionString;
	action_string_position_t _actionStringPosition;
	mutable yaal::hcore::HString _cache;
	typedef enum {
		START = 0,
		MINUS = 1,
		DIGIT = 2
	} real_paring_state_t;
public:
	HInteger( HInteger const& );
	HInteger operator[]( action_t const& ) const;
	HInteger operator[]( action_position_t const& ) const;
	HInteger operator[]( action_int_long_long_t const& ) const;
	HInteger operator[]( action_int_long_long_position_t const& ) const;
	HInteger operator[]( action_int_long_t const& ) const;
	HInteger operator[]( action_int_long_position_t const& ) const;
	HInteger operator[]( action_int_t const& ) const;
	HInteger operator[]( action_int_position_t const& ) const;
	HInteger operator[]( action_number_t const& ) const;
	HInteger operator[]( action_number_position_t const& ) const;
	HInteger operator[]( action_string_t const& ) const;
	HInteger operator[]( action_string_position_t const& ) const;
protected:
	HInteger( action_t const& );
	HInteger( action_position_t const& );
	HInteger( action_int_long_long_t const& );
	HInteger( action_int_long_long_position_t const& );
	HInteger( action_int_long_t const& );
	HInteger( action_int_long_position_t const& );
	HInteger( action_int_t const& );
	HInteger( action_int_position_t const& );
	HInteger( action_number_t const& );
	HInteger( action_number_position_t const& );
	HInteger( action_string_t const& );
	HInteger( action_string_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HInteger( void );
	HInteger& operator = ( HInteger const& );
	friend HInteger const& get_integer_instance( void );
};

typedef yaal::hcore::HExceptionT<HInteger, HRuleBaseException> HIntegerException;

extern M_YAAL_TOOLS_PUBLIC_API HInteger const& integer;

class HStringLiteral : public HRuleBase {
public:
	typedef HStringLiteral this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const&, position_t )> action_string_position_t;
private:
	action_string_t _actionString;
	action_string_position_t _actionStringPosition;
	mutable yaal::hcore::HString _cache;
public:
	HStringLiteral( HStringLiteral const& );
	HStringLiteral operator[]( action_t const& ) const;
	HStringLiteral operator[]( action_position_t const& ) const;
	HStringLiteral operator[]( action_string_t const& ) const;
	HStringLiteral operator[]( action_string_position_t const& ) const;
protected:
	HStringLiteral( action_t const& );
	HStringLiteral( action_position_t const& );
	HStringLiteral( action_string_t const& );
	HStringLiteral( action_string_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HStringLiteral( void );
	HStringLiteral& operator = ( HStringLiteral const& );
	friend HStringLiteral const& get_string_literal_instance( void );
};

typedef yaal::hcore::HExceptionT<HStringLiteral, HRuleBaseException> HStringLiteralException;

extern M_YAAL_TOOLS_PUBLIC_API HStringLiteral const& string_literal;

class HCharacterLiteral : public HRuleBase {
public:
	typedef HCharacterLiteral this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( char )> action_character_t;
	typedef yaal::hcore::HBoundCall<void ( char, position_t )> action_character_position_t;
private:
	action_character_t _actionCharacter;
	action_character_position_t _actionCharacterPosition;
	mutable yaal::hcore::HString _cache;
public:
	HCharacterLiteral( HCharacterLiteral const& );
	HCharacterLiteral operator[]( action_t const& ) const;
	HCharacterLiteral operator[]( action_position_t const& ) const;
	HCharacterLiteral operator[]( action_character_t const& ) const;
	HCharacterLiteral operator[]( action_character_position_t const& ) const;
protected:
	HCharacterLiteral( action_t const& );
	HCharacterLiteral( action_position_t const& );
	HCharacterLiteral( action_character_t const& );
	HCharacterLiteral( action_character_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HCharacterLiteral( void );
	HCharacterLiteral& operator = ( HCharacterLiteral const& );
	friend HCharacterLiteral const& get_character_literal_instance( void );
};

typedef yaal::hcore::HExceptionT<HCharacterLiteral, HRuleBaseException> HLiteralCharacterException;

extern M_YAAL_TOOLS_PUBLIC_API HCharacterLiteral const& character_literal;

class HCharacter : public HRuleBase {
public:
	typedef HCharacter this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( char )> action_character_t;
	typedef yaal::hcore::HBoundCall<void ( char, position_t )> action_character_position_t;
private:
	yaal::hcore::HString _characters;
	action_character_t _actionCharacter;
	action_character_position_t _actionCharacterPosition;
public:
	HCharacter( HCharacter const& character_ );
	virtual ~HCharacter( void ) {
		return;
	}
	HCharacter operator[]( action_t const& ) const;
	HCharacter operator[]( action_position_t const& ) const;
	HCharacter operator[]( action_character_t const& ) const;
	HCharacter operator[]( action_character_position_t const& ) const;
	HCharacter operator() ( yaal::hcore::HString const& ) const;
protected:
	HCharacter( yaal::hcore::HString const&, action_t const& );
	HCharacter( yaal::hcore::HString const&, action_position_t const& );
	HCharacter( yaal::hcore::HString const&, action_character_t const& );
	HCharacter( yaal::hcore::HString const&, action_character_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HCharacter( void );
	HCharacter& operator = ( HCharacter const& );
	friend HCharacter const& get_character_instance( void );
};

typedef yaal::hcore::HExceptionT<HCharacter, HRuleBaseException> HCharacterException;

extern M_YAAL_TOOLS_PUBLIC_API HCharacter const& character;

class HString : public HRuleBase {
public:
	typedef HString this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const&, position_t )> action_string_position_t;
private:
	yaal::hcore::HString _string;
	action_string_t _actionString;
	action_string_position_t _actionStringPosition;
public:
	HString( HString const& );
	virtual ~HString( void ) {
	}
	HString operator[]( action_t const& ) const;
	HString operator[]( action_position_t const& ) const;
	HString operator[]( action_string_t const& ) const;
	HString operator[]( action_string_position_t const& ) const;
	HString operator() ( yaal::hcore::HString const& ) const;
protected:
	HString( yaal::hcore::HString const&, action_t const& );
	HString( yaal::hcore::HString const&, action_position_t const& );
	HString( yaal::hcore::HString const&, action_string_t const& );
	HString( yaal::hcore::HString const&, action_string_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HString( yaal::hcore::HString const& );
	HString& operator = ( HString const& ) = delete;
	friend HString string( yaal::hcore::HString const&, HString::action_string_t const& );
};

typedef yaal::hcore::HExceptionT<HString, HRuleBaseException> HStringException;

HString string( yaal::hcore::HString const&, HString::action_string_t const& = HString::action_string_t() );

class HRegex : public HRuleBase {
public:
	typedef HRegex this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const& )> action_string_t;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const&, position_t )> action_string_position_t;
private:
	typedef yaal::hcore::HPointer<yaal::hcore::HRegex> regex_t;
	regex_t _regex;
	action_string_t _actionString;
	action_string_position_t _actionStringPosition;
public:
	HRegex( HRegex const& );
	virtual ~HRegex( void ) {
		return;
	}
	HRegex operator[]( action_t const& ) const;
	HRegex operator[]( action_position_t const& ) const;
	HRegex operator[]( action_string_t const& ) const;
	HRegex operator[]( action_string_position_t const& ) const;
protected:
	HRegex( regex_t const&, action_t const& );
	HRegex( regex_t const&, action_position_t const& );
	HRegex( regex_t const&, action_string_t const& );
	HRegex( regex_t const&, action_string_position_t const& );
	virtual ptr_t do_clone( void ) const;
	virtual yaal::hcore::HString::const_iterator do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) const;
	virtual void do_describe( HRuleDescription&, rule_use_t const& ) const;
	virtual void do_detach( HRuleBase const*, visited_t&, bool& );
	virtual void do_detect_recursion( HRecursionDetector& ) const;
	virtual void do_find_recursions( HRuleAggregator& );
	virtual bool do_has_action( void ) const;
private:
	HRegex( yaal::hcore::HString const& );
	HRegex( yaal::hcore::HString const&, action_t const& );
	HRegex( yaal::hcore::HString const&, action_position_t const& );
	HRegex( yaal::hcore::HString const&, action_string_t const& );
	HRegex( yaal::hcore::HString const&, action_string_position_t const& );
	HRegex& operator = ( HRegex const& );
	friend HRegex regex( yaal::hcore::HString const& );
	friend HRegex regex( yaal::hcore::HString const&, HRuleBase::action_t const& );
	friend HRegex regex( yaal::hcore::HString const&, HRuleBase::action_position_t const& );
	friend HRegex regex( yaal::hcore::HString const&, HRegex::action_string_t const& );
	friend HRegex regex( yaal::hcore::HString const&, HRegex::action_string_position_t const& );
	friend HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const& );
	friend HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRuleBase::action_t const& );
	friend HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRuleBase::action_position_t const& );
	friend HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRegex::action_string_t const& );
	friend HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRegex::action_string_position_t const& );
};

typedef yaal::hcore::HExceptionT<HRegex, HRuleBaseException> HRegexException;

HCharacter constant( char );
HCharacter constant( char, HRuleBase::action_t const& );
HCharacter constant( char, HRuleBase::action_position_t const& );
HCharacter constant( char, HCharacter::action_character_t const& );
HCharacter constant( char, HCharacter::action_character_position_t const& );
HCharacter characters( yaal::hcore::HString const& );
HCharacter characters( yaal::hcore::HString const&, HRuleBase::action_t const& );
HCharacter characters( yaal::hcore::HString const&, HRuleBase::action_position_t const& );
HCharacter characters( yaal::hcore::HString const&, HCharacter::action_character_t const& );
HCharacter characters( yaal::hcore::HString const&, HCharacter::action_character_position_t const& );
HString constant( yaal::hcore::HString const& );
HString constant( yaal::hcore::HString const&, HRuleBase::action_t const& );
HString constant( yaal::hcore::HString const&, HRuleBase::action_position_t const& );
HString constant( yaal::hcore::HString const&, HString::action_string_t const& );
HString constant( yaal::hcore::HString const&, HString::action_string_position_t const& );
HRegex regex( yaal::hcore::HString const& );
HRegex regex( yaal::hcore::HString const&, HRuleBase::action_t const& );
HRegex regex( yaal::hcore::HString const&, HRuleBase::action_position_t const& );
HRegex regex( yaal::hcore::HString const&, HRegex::action_string_t const& );
HRegex regex( yaal::hcore::HString const&, HRegex::action_string_position_t const& );
HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const& );
HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRuleBase::action_t const& );
HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRuleBase::action_position_t const& );
HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRegex::action_string_t const& );
HRule regex( yaal::hcore::HString const&, yaal::hcore::HString const&, HRegex::action_string_position_t const& );

}

class HExecutingParser : private trait::HNonCopyable {
public:
	typedef HExecutingParser this_type;
	typedef yaal::hcore::HBoundCall<> executor_t;
	typedef yaal::hcore::HArray<yaal::hcore::HString> messages_t;
	enum class INIT_MODE {
		VERIFY_GRAMMAR,
		TRUST_GRAMMAR
	};
	class HProxy {
		static void add_execution_step( HExecutingParser*, yaal::hcore::HString::const_iterator, executor_t const& );
		static void drop_execution_steps( HExecutingParser*, yaal::hcore::HString::const_iterator );
		static void report_error( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString const& );
		static int position( HExecutingParser*, yaal::hcore::HString::const_iterator );
		friend class executing_parser::HRuleBase;
	};
private:
	typedef executing_parser::HRuleBase::ptr_t grammar_t;
	typedef yaal::hcore::HPair<yaal::hcore::HString::const_iterator, executor_t> execution_step_t;
	typedef yaal::hcore::HArray<execution_step_t> execution_steps_t;
	grammar_t _grammar;
	execution_steps_t _excutors;
	bool _matched;
	int long _errorPosition;
	messages_t _errorMessages;
	yaal::hcore::HString::const_iterator _inputStart; /*!< Ephemeric! */
public:
	HExecutingParser( executing_parser::HRuleBase const&, INIT_MODE = INIT_MODE::VERIFY_GRAMMAR );
	bool operator()( yaal::hcore::HString const& );
	bool operator()( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	void operator()( void );
	void execute( void );
	bool parse( yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator );
	int error_position( void ) const;
	messages_t const& error_messages( void ) const;
private:
	void add_execution_step( yaal::hcore::HString::const_iterator, executor_t const& );
	void drop_execution_steps( yaal::hcore::HString::const_iterator );
	void report_error( yaal::hcore::HString::const_iterator, yaal::hcore::HString const& );
	int position( yaal::hcore::HString::const_iterator );
	void sanitize( void );
	HExecutingParser( HExecutingParser const& );
	HExecutingParser& operator = ( HExecutingParser const& );
	friend class HProxy;
};

typedef yaal::hcore::HExceptionT<HExecutingParser> HExecutingParserException;

}

}

#endif /* #ifndef YAAL_TOOLS_EXECUTINGPARSER_HXX_INCLUDED */

