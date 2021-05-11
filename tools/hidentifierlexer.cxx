/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hidentifierlexer.hxx"
#include "hcore/unicode.hxx"
#include "tools/util.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace yaal {

namespace tools {

namespace {

typedef yaal::hcore::HArray<bool> flags_t;
static code_point_t const CODE_POINT_CUT_OFF( 0x10000u );

inline void common_specific( flags_t& flags_ ) {
	flags_[ static_cast<int>( unicode::CODE_POINT::N_ARY_PRODUCT.get() ) ] = true;
	flags_[ static_cast<int>( unicode::CODE_POINT::N_ARY_SUMMATION.get() ) ] = true;
	flags_[ static_cast<int>( unicode::CODE_POINT::SQUARE_ROOT.get() ) ] = true;
}

flags_t make_heads( void ) {
	flags_t heads;
	heads.reserve( static_cast<int>( CODE_POINT_CUT_OFF.get() ) );
	for ( code_point_t cp( 0 ); cp < CODE_POINT_CUT_OFF; ++ cp ) {
		heads.push_back(
			is_letter( cp )
			|| is_greek( cp )
			|| ( cp == '_'_ycp )
		);
	}
	common_specific( heads );
	return heads;
}

flags_t make_tails( void ) {
	flags_t tails;
	tails.reserve( static_cast<int>( CODE_POINT_CUT_OFF.get() ) );
	for ( code_point_t cp( 0 ); cp < CODE_POINT_CUT_OFF; ++ cp ) {
		tails.push_back(
			is_letter( cp )
			|| is_digit( cp )
			|| is_greek( cp )
			|| is_subscript( cp )
			|| ( cp == '_'_ycp )
		);
	}
	common_specific( tails );
	return tails;
}

static flags_t const _heads_( make_heads() );
static flags_t const _tails_( make_tails() );

inline bool is_identifer_head( code_point_t const& codePoint_ ) {
	return ( ( codePoint_ < CODE_POINT_CUT_OFF ) && _heads_[static_cast<int>( codePoint_.get() )] );
}

inline bool is_identifer_tail( code_point_t const& codePoint_ ) {
	return ( ( codePoint_ < CODE_POINT_CUT_OFF ) && _tails_[static_cast<int>( codePoint_.get() )] );
}

}

HIdentifierLexer::HIdentifierLexer( HIdentifierLexer const& identifier_ )
	: HRuleBase( identifier_._action, identifier_._actionPosition, identifier_._skipWS )
	, _name( identifier_._name )
	, _actionStringPosition( identifier_._actionStringPosition )
	, _actionString( identifier_._actionString )
	, _errorMessage( identifier_._errorMessage ) {
	return;
}

HIdentifierLexer::HIdentifierLexer( yaal::hcore::HString const& name_, action_string_range_t const& action_ )
	: HRuleBase( true )
	, _name( name_ )
	, _actionStringPosition( action_ )
	, _actionString()
	, _errorMessage( "expected "_ys.append( article( _name ) ).append( " " ).append( _name ) ) {
	return;
}

HIdentifierLexer::HIdentifierLexer( yaal::hcore::HString const& name_, action_string_t const& action_ )
	: HRuleBase( true )
	, _name( name_ )
	, _actionStringPosition()
	, _actionString( action_ )
	, _errorMessage( "expected "_ys.append( article( _name ) ).append( " " ).append( _name ) ) {
	return;
}

HIdentifierLexer::ptr_t HIdentifierLexer::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HIdentifierLexer>( *this ) );
	M_EPILOG
}

hcore::HUTF8String::const_iterator HIdentifierLexer::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	bool matched( false );
	if ( scan != last_ ) {
		if ( is_identifer_head( *scan ) ) {
			++ scan;
			while ( is_identifer_tail( *scan ) ) {
				++ scan;
			}
		}
		if ( scan != first_ ) {
			executing_parser::range_t rng( range( executingParser_, first_, scan ) );
			if ( !! _actionStringPosition ) {
				add_execution_step( executingParser_, call( _actionStringPosition, hcore::HUTF8String( first_, scan ), rng ) );
			} else if ( !! _actionString ) {
				add_execution_step( executingParser_, call( _actionString, hcore::HUTF8String( first_, scan ) ) );
			}
			matched = true;
		}
	} else {
		scan = first_;
	}
	if ( ! matched ) {
		report_error( executingParser_, scan, _errorMessage );
		scan = first_;
	}
	return scan;
	M_EPILOG
}

void HIdentifierLexer::do_describe( executing_parser::HRuleDescription& rd_, executing_parser::rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( _name );
	return;
	M_EPILOG
}

bool HIdentifierLexer::do_detect_recursion( executing_parser::HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

bool HIdentifierLexer::do_has_action( void ) const {
	return ( HRuleBase::do_has_action() || ( !! _actionStringPosition ) || ( !! _actionString ) );
}

}

}

