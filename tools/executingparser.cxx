/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	executingparser.cxx - this file is integral part of `yaal' project.

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

#include <cctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "executingparser.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace executing_parser {

HRuleBase::HRuleBase( void )
	: _matched( false ), _action(), _excutors()
	{}

HRuleBase::HRuleBase( action_t const& action_ )
	: _matched( false ), _action( action_ ), _excutors()
	{}

bool HRuleBase::operator()( yaal::hcore::HString const& input_ ) {
	M_PROLOG
	return ( parse( input_.begin(), input_.end() ) != input_.end() );
	M_EPILOG
}

bool HRuleBase::operator()( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	return ( parse( first_, last_ ) != last_ );
	M_EPILOG
}

void HRuleBase::operator()( void ) {
	M_PROLOG
	execute();
	return;
	M_EPILOG
}

void HRuleBase::execute( void ) {
	M_PROLOG
	do_execute();
	return;
	M_EPILOG
}

yaal::hcore::HString::const_iterator HRuleBase::parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator it( do_parse( first_, last_ ) );
	_matched = it != first_;
	return ( it );
	M_EPILOG
}

HRuleBase::ptr_t HRuleBase::clone( void ) const {
	M_PROLOG
	return ( do_clone() );
	M_EPILOG
}

bool HRuleBase::is_optional( void ) const {
	M_PROLOG
	return ( do_is_optional() );
	M_EPILOG
}

void HRuleBase::do_execute( void ) {
	M_PROLOG
	M_ENSURE( _matched );
	for ( executors_t::iterator it( _excutors.begin() ), end( _excutors.end() ); it != end; ++ it )
		(*it)();
	return;
	M_EPILOG
}

yaal::hcore::HString::const_iterator HRuleBase::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator ) {
	M_PROLOG
	return ( first_ );
	M_EPILOG
}

bool HRuleBase::do_is_optional( void ) const
	{ return ( false ); }

yaal::hcore::HString::const_iterator HRuleBase::skip_space( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	while ( ( first_ != last_ ) && isspace( *first_ ) )
		++ first_;
	return ( first_ );
	M_EPILOG
}

HRule::HRule( void )
	: HRuleBase(), _rule()
	{}

HRule::HRule( HRule const& rule_ )
	: HRuleBase( rule_._action ), _rule( rule_._rule->clone() ) {
}

HRule::HRule( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_.clone() ) {
}

HRule::HRule( ptr_t const& rule_ )
	: _rule( rule_ )
	{}

HRule::HRule( ptr_t const& rule_, action_t const& action_ )
	: HRuleBase( action_ ), _rule( rule_ )
	{}

HRule HRule::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HRule( clone(), action_ ) );
	M_EPILOG
}

HRule::ptr_t HRule::do_clone( void ) const {
	M_PROLOG
	M_ENSURE( !! _rule );
	return ( ! _action ? _rule->clone() : ptr_t( new HRule( _rule->clone(), _action ) ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HRule::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator ret( !! _rule ? _rule->parse( first_, last_ ) : first_ );
	if ( ( ret != first_ ) && !! _action )
		_excutors.push_back( _action );
	return ( ret );
	M_EPILOG
}

void HRule::do_execute( void ) {
	M_PROLOG
	M_ENSURE( _matched );
	if ( !! _rule )
		_rule->execute();
	for ( executors_t::iterator it( _excutors.begin() ), end( _excutors.end() ); it != end; ++ it )
		(*it)();
	return;
	M_EPILOG
}

bool HRule::do_is_optional( void ) const
	{ return ( _rule->is_optional() ); }

HFollows::HFollows( HRuleBase const& predecessor_, HRuleBase const& successor_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	_rules.push_back( predecessor_.clone() );
	_rules.push_back( successor_.clone() );
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows const& follows_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( follows_._rules.begin() ), end( follows_._rules.end() ); it != end; ++ it )
		_rules.push_back( (*it)->clone() );
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows const& predecessors_, HRuleBase const& successor_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( predecessors_._rules.begin() ), end( predecessors_._rules.end() ); it != end; ++ it )
		_rules.push_back( (*it)->clone() );
	_rules.push_back( successor_.clone() );
	return;
	M_EPILOG
}

HRule HFollows::operator[]( action_t const& action_ ) const {
	M_PROLOG
	return ( HRule( clone(), action_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HFollows::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HFollows( *this ) ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HFollows::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator orig( first_ );
	bool matched( true );
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		yaal::hcore::HString::const_iterator old( first_ );
		first_ = (*it)->parse( first_, last_ );
		if ( ( first_ == old ) && ( !(*it)->is_optional() ) ) {
			matched = false;
			break;
		} else if ( !! _action && ( first_ != old ) )
			_excutors.push_back( _action );
	}
	return ( matched ? first_ : orig );
	M_EPILOG
}

void HFollows::do_execute( void ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it )
		(*it)->execute();
	return;
	M_EPILOG
}

HKleeneStar::HKleeneStar( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_.clone() )
	{}

HKleeneStar::HKleeneStar( ptr_t rule_, action_t action_ )
	: HRuleBase( action_ ), _rule( rule_ )
	{}

HKleeneStar::HKleeneStar( HKleeneStar const& kleeneStar_ )
	: HRuleBase( kleeneStar_._action ), _rule( kleeneStar_._rule->clone() )
	{}

HKleeneStar HKleeneStar::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HKleeneStar( _rule, action_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HKleeneStar::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HKleeneStar( *this ) ) );
	M_EPILOG
}

bool HKleeneStar::do_is_optional( void ) const
	{ return ( true ); }

yaal::hcore::HString::const_iterator HKleeneStar::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator old( last_ );
	while ( ( first_ != last_ ) && ( first_ != old ) ) {
		first_ = _rule->parse( old = first_, last_ );
		if ( !! _action && ( first_ != old ) )
			_excutors.push_back( _action );
	}
	return ( first_ );
	M_EPILOG
}

void HKleeneStar::do_execute( void ) {
	M_PROLOG
	M_ENSURE( _matched );
	if ( !! _rule )
		_rule->execute();
	for ( executors_t::iterator it( _excutors.begin() ), end( _excutors.end() ); it != end; ++ it )
		(*it)();
	return;
	M_EPILOG
}

HKleenePlus::HKleenePlus( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_.clone() )
	{}

HKleenePlus::HKleenePlus( ptr_t rule_, action_t action_ )
	: HRuleBase( action_ ), _rule( rule_->clone() )
	{}

HKleenePlus HKleenePlus::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HKleenePlus( _rule, action_ ) );
	M_EPILOG
}

HKleenePlus::HKleenePlus( HKleenePlus const& kleenePlus_ )
	: HRuleBase( kleenePlus_._action ), _rule( kleenePlus_._rule->clone() )
	{}

HRuleBase::ptr_t HKleenePlus::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HKleenePlus( *this ) ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HKleenePlus::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator old( last_ );
	while ( ( first_ != last_ ) && ( first_ != old ) ) {
		first_ = _rule->parse( old = first_, last_ );
		if ( !! _action && ( first_ != old ) )
			_excutors.push_back( _action );
	}
	return ( first_ );
	M_EPILOG
}

void HKleenePlus::do_execute( void ) {
	M_PROLOG
	M_ENSURE( _matched );
	if ( !! _rule )
		_rule->execute();
	for ( executors_t::iterator it( _excutors.begin() ), end( _excutors.end() ); it != end; ++ it )
		(*it)();
	return;
	M_EPILOG
}

HAlternative::HAlternative( HRuleBase const& choice1_, HRuleBase const& choice2_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	_rules.push_back( choice1_.clone() );
	_rules.push_back( choice2_.clone() );
	return;
	M_EPILOG
}

HAlternative::HAlternative( HAlternative const& alternative_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( alternative_._rules.begin() ), end( alternative_._rules.end() ); it != end; ++ it )
		_rules.push_back( (*it)->clone() );
	return;
	M_EPILOG
}

HAlternative::HAlternative( HAlternative const& alternative_, HRuleBase const& choice_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( alternative_._rules.begin() ), end( alternative_._rules.end() ); it != end; ++ it )
		_rules.push_back( (*it)->clone() );
	_rules.push_back( choice_.clone() );
	return;
	M_EPILOG
}

HRuleBase::ptr_t HAlternative::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HAlternative( *this ) ) );
	M_EPILOG
}

bool HOptional::do_is_optional( void ) const
	{ return ( true ); }

HFollows operator >> ( HRuleBase const& predecessor_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, successor_ ) );
	M_EPILOG
}

HFollows operator >> ( HFollows const& predecessors_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( predecessors_, successor_ ) );
	M_EPILOG
}

HAlternative operator | ( HRuleBase const& choice1_, HRuleBase const& choice2_ ) {
	M_PROLOG
	return ( HAlternative( choice1_, choice2_ ) );
	M_EPILOG
}

HAlternative operator | ( HAlternative const& alternative_, HRuleBase const& choice_ ) {
	M_PROLOG
	return ( HAlternative( alternative_, choice_ ) );
	M_EPILOG
}

HKleeneStar operator* ( HRuleBase const& rule_ ) {
	M_PROLOG
	return ( HKleeneStar( rule_ ) );
	M_EPILOG
}

HKleenePlus operator+ ( HRuleBase const& rule_ ) {
	M_PROLOG
	return ( HKleenePlus( rule_ ) );
	M_EPILOG
}

HReal::HReal( void )
	: HRuleBase(), _actionDouble(), _actionDoubleLong(),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_t action_ )
	: HRuleBase( action_ ), _actionDouble(), _actionDoubleLong(),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_double_t action_ )
	: HRuleBase(), _actionDouble( action_ ), _actionDoubleLong(),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_double_long_t action_ )
	: HRuleBase(), _actionDouble(), _actionDoubleLong( action_ ),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_number_t action_ )
	: HRuleBase(), _actionDouble(), _actionDoubleLong(),
	_actionNumber( action_ ), _actionString(), _cache()
	{}

HReal::HReal( action_string_t action_ )
	: HRuleBase(), _actionDouble(), _actionDoubleLong(),
	_actionNumber(), _actionString( action_ ), _cache()
	{}

HReal::HReal( HReal const& real_ )
	: HRuleBase( real_._action ), _actionDouble( real_._actionDouble ), _actionDoubleLong( real_._actionDoubleLong ),
	_actionNumber( real_._actionNumber ), _actionString( real_._actionString ), _cache( real_._cache )
	{}

HReal HReal::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_number_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HReal::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator scan( skip_space( first_, last_ ) );
	_cache.clear();
	real_paring_state_t state( START );
	while ( scan != last_ ) {
		bool stop( false );
		char ch( *scan );
		switch ( state ) {
			case ( START ): {
				if ( isdigit( ch ) )
					state = INTEGRAL;
				else if ( ch == '-' )
					state = MINUS;
				else if ( ch == '.' )
					state = DOT;
				else
					stop = true;
			} break;
			case ( MINUS ):
			case ( INTEGRAL ): {
				if ( isdigit( ch ) )
					state = INTEGRAL;
				else if ( ch == '.' )
					state = DOT;
				else
					stop = true;
			} break;
			case ( DOT ):
			case ( DECIMAL ): {
				if ( isdigit( ch ) )
					state = DECIMAL;
				else
					stop = true;
			} break;
			default: {
				M_ASSERT( ! "invalid hardcoded state" );
			}
		}
		if ( stop )
			break;
		_cache.push_back( *scan );
		++ scan;
	}
	if ( state >= INTEGRAL ) {
		first_ = scan;
		if ( !! _actionDouble ) {
			double d( lexical_cast<double>( _cache ) );
			_excutors.push_back( call( _actionDouble, d ) );
		} else if ( !! _actionDoubleLong ) {
			double long dl( lexical_cast<double long>( _cache ) );
			_excutors.push_back( call( _actionDoubleLong, dl ) );
		} else if ( !! _actionNumber ) {
			_excutors.push_back( call( _actionNumber, _cache ) );
		} else if ( !! _actionString ) {
			_excutors.push_back( call( _actionString, _cache ) );
		} else if ( !! _action )
			_excutors.push_back( call( _action ) );
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HReal::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HReal( *this ) ) );
	M_EPILOG
}

HReal real;

HInteger::HInteger( void )
	: HRuleBase(), _actionIntLong(), _actionInt(),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_t action_ )
	: HRuleBase( action_ ), _actionIntLong(), _actionInt(),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_int_long_t action_ )
	: HRuleBase(), _actionIntLong( action_ ), _actionInt(),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_int_t action_ )
	: HRuleBase(), _actionIntLong(), _actionInt( action_ ),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_number_t action_ )
	: HRuleBase(), _actionIntLong(), _actionInt(),
	_actionNumber( action_ ), _actionString(), _cache()
	{}

HInteger::HInteger( action_string_t action_ )
	: HRuleBase(), _actionIntLong(), _actionInt(),
	_actionNumber(), _actionString( action_ ), _cache()
	{}

HInteger::HInteger( HInteger const& integer_ )
	: HRuleBase( integer_._action ), _actionIntLong( integer_._actionIntLong ), _actionInt( integer_._actionInt ),
	_actionNumber( integer_._actionNumber ), _actionString( integer_._actionString ), _cache( integer_._cache )
	{}

HInteger HInteger::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_number_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HInteger::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator scan( skip_space( first_, last_ ) );
	_cache.clear();
	real_paring_state_t state( START );
	while ( scan != last_ ) {
		bool stop( false );
		char ch( *scan );
		switch ( state ) {
			case ( START ): {
				if ( isdigit( ch ) )
					state = DIGIT;
				else if ( ch == '-' )
					state = MINUS;
				else
					stop = true;
			} break;
			case ( MINUS ):
			case ( DIGIT ): {
				if ( isdigit( ch ) )
					state = DIGIT;
				else
					stop = true;
			} break;
			default: {
				M_ASSERT( ! "invalid hardcoded state" );
			}
		}
		if ( stop )
			break;
		_cache.push_back( *scan );
		++ scan;
	}
	if ( state >= DIGIT ) {
		first_ = scan;
		if ( !! _actionIntLong ) {
			int long il( lexical_cast<int long>( _cache ) );
			_excutors.push_back( call( _actionIntLong, il ) );
		} else if ( !! _actionInt ) {
			int i( lexical_cast<int>( _cache ) );
			_excutors.push_back( call( _actionInt, i ) );
		} else if ( !! _actionNumber ) {
			_excutors.push_back( call( _actionNumber, _cache ) );
		} else if ( !! _actionString ) {
			_excutors.push_back( call( _actionString, _cache ) );
		} else if ( !! _action )
			_excutors.push_back( call( _action ) );
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HInteger::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HInteger( *this ) ) );
	M_EPILOG
}

HInteger integer;

HCharacter::HCharacter( char character_ )
	: HRuleBase(), _character( character_ ), _actionChar()
	{}

HCharacter::HCharacter( char character_, action_t action_ )
	: HRuleBase( action_ ), _character( character_ ), _actionChar()
	{}

HCharacter::HCharacter( char character_, action_char_t action_ )
	: HRuleBase(), _character( character_ ), _actionChar( action_ )
	{}

HCharacter::HCharacter( HCharacter const& character_ )
	: HRuleBase( character_._action ), _character( character_._character ), _actionChar( character_._actionChar )
	{}

HCharacter HCharacter::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionChar ) );
	return ( HCharacter( _character, action_ ) );
	M_EPILOG
}

HCharacter HCharacter::operator[]( action_char_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionChar ) );
	return ( HCharacter( _character, action_ ) );
	M_EPILOG
}

HCharacter HCharacter::operator() ( char character_ ) const {
	M_PROLOG
	return ( HCharacter( character_, _action ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HCharacter::do_parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	M_ENSURE( first_ != last_ );
	yaal::hcore::HString::const_iterator scan( skip_space( first_, last_ ) );
	char c( *scan );
	if ( _character ) {
		if ( ! _character || ( c == _character ) ) {
			if ( !! _actionChar )
				_excutors.push_back( call( _actionChar, c ) );
			else if ( !! _action )
				_excutors.push_back( call( _action ) );
			++ scan;
			first_ = scan;
		}
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HCharacter::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HCharacter( *this ) ) );
	M_EPILOG
}

HCharacter character;

HFollows operator >> ( char character_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( HCharacter( character_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, char character_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, HCharacter( character_ ) ) );
	M_EPILOG
}

HString::HString( hcore::HString const& string_ )
	: HRuleBase(), _string( string_ ), _actionString()
	{}

HString::HString( hcore::HString const& string_, action_t action_ )
	: HRuleBase( action_ ), _string( string_ ), _actionString()
	{}

HString::HString( hcore::HString const& string_, action_string_t action_ )
	: HRuleBase(), _string( string_ ), _actionString( action_ )
	{}

HString::HString( HString const& string_ )
	: HRuleBase( string_._action ), _string( string_._string ), _actionString( string_._actionString )
	{}

HString HString::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionString ) );
	return ( HString( _string, action_ ) );
	M_EPILOG
}

HString HString::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionString ) );
	return ( HString( _string, action_ ) );
	M_EPILOG
}

HString HString::operator() ( hcore::HString const& string_ ) const {
	M_PROLOG
	return ( HString( string_, _action ) );
	M_EPILOG
}

hcore::HString::const_iterator HString::do_parse( hcore::HString::const_iterator first_, hcore::HString::const_iterator last_ ) {
	M_PROLOG
	M_ENSURE( first_ != last_ );
	if ( ! _string.is_empty() ) {
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HString::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HString( *this ) ) );
	M_EPILOG
}

HString string;

HFollows operator >> ( char const* string_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( HString( string_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( hcore::HString const& string_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( HString( string_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, char const* string_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, HString( string_ ) ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, hcore::HString const& string_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, HString( string_ ) ) );
	M_EPILOG
}

}

}

}

