/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hidentifierparser.hxx
 * \brief Declaration HIdentifierLexer class.
 */

#ifndef YAAL_TOOLS_HIDENTIFIERLEXER_HXX_INCLUDED
#define YAAL_TOOLS_HIDENTIFIERLEXER_HXX_INCLUDED 1

#include "tools/executingparser.hxx"

namespace yaal {

namespace tools {

class HIdentifierLexer : public executing_parser::HRuleBase {
public:
	typedef HIdentifierLexer this_type;
	typedef HRuleBase base_type;
	typedef yaal::hcore::HBoundCall<void ( yaal::hcore::HString const&, executing_parser::range_t )> action_string_range_t;
private:
	yaal::hcore::HString _name;
	action_string_range_t _actionStringPosition;
	yaal::hcore::HString _errorMessage;
public:
	HIdentifierLexer( HIdentifierLexer const& );
	virtual ~HIdentifierLexer( void ) {
		return;
	}
protected:
	virtual ptr_t do_clone( void ) const override;
	virtual yaal::hcore::HUTF8String::const_iterator do_parse( HExecutingParser*, yaal::hcore::HUTF8String::const_iterator const&, yaal::hcore::HUTF8String::const_iterator const& ) const override;
	virtual void do_describe( executing_parser::HRuleDescription&, executing_parser::rule_use_t const& ) const override;
	virtual void do_detach( HRuleBase const*, executing_parser::visited_t&, bool& ) override {
		return;
	}
	virtual bool do_detect_recursion( executing_parser::HRecursionDetector&, bool ) const override;
	virtual void do_find_recursions( executing_parser::HRuleAggregator& ) override {
		return;
	}
	virtual bool do_has_action( void ) const override;
private:
	HIdentifierLexer( yaal::hcore::HString const&, action_string_range_t const& );
	HIdentifierLexer& operator = ( HIdentifierLexer const& ) = delete;
	friend HIdentifierLexer identifier( yaal::hcore::HString const&, HIdentifierLexer::action_string_range_t const& );
	friend HIdentifierLexer identifier( HIdentifierLexer::action_string_range_t const& );
};

inline HIdentifierLexer identifier( yaal::hcore::HString const& name_, HIdentifierLexer::action_string_range_t const& action_ ) {
	return ( HIdentifierLexer( name_, action_ ) );
}

}

}

#endif /* #ifndef YAAL_TOOLS_HIDENTIFIERLEXER_HXX_INCLUDED */

