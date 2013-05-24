/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhuginn.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hhuginn.hxx"

namespace yaal {

namespace tools {

namespace executing_parser {

HRule name( regex( "\\<[a-zA-Z_][a-zA-Z0-9_]*\\>" ) );
HRule expr;
HRule absoluteValue( '|' >> expr >> '|' );
HRule parenthesis( '(' >> expr >> ')' );
HRule atom( absoluteValue | parenthesis );
HRule power( atom >> ( * ( '^' >> atom ) ) );
HRule multiplication( power >> ( * ( '*' >> power ) ) );
HRule sum( multiplication >> ( * ( '+' >> multiplication ) ) );
HRule value( sum );
HRule assignment( *( name >> '=' ) >> value );
HRule expression( assignment );
HRule expressionList( + expression );
HRule ifStatement( executing_parser::constant( "if" ) >> '(' >> expression >> ')' >> '{' >> expressionList >> '}' >> "else" >> '{' >> expressionList >> '}' );
HRule whileStatement( constant( "while" ) >> '(' >> expression >> ')' >> '{' >> expressionList >> '}' );
HRule statementList( *( ifStatement | whileStatement | expressionList ) );
HRule scope( constant( '{' ) >> statementList >> '}' );
HRule functionDefinition( name >> '(' >> ')' >> scope );

}

void HHuginn::execute( void ) {
	M_PROLOG
	call( "main" );
	return;
	M_EPILOG
}

void HHuginn::call( yaal::hcore::HString const& ) {
	M_PROLOG
	return;
	M_EPILOG
}

HHuginn::HScope::HScope( HScope* parent_ )
	: _variables(), _parent( parent_ ) {
}

HHuginn::HIf::HIf( HExecutingParser::executor_t condition_,
		HExecutingParser::executor_t ifClause_,
		HExecutingParser::executor_t elseClause_ )
	: HScope( NULL ), _condition( condition_ ), _ifClause( ifClause_ ), _elseClause( elseClause_ ) {
}

}

}

