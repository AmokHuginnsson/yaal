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
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hhuginn.hxx"

namespace yaal {

namespace tools {

namespace executing_parser {

#if 0
/*
 * Sample code in huginn.
 */

sum( a, b ) {
	return ( a + b );
}

main( args ) {
	a = integer( args[0] );
	b = integer( args[1] );
	return ( sum( 3 + a, sum( 4, ( b + 1 ) * 2 ) ) );
}

#endif

HRule huginn_grammar( void );
HRule huginn_grammar( void ) {
	M_PROLOG
	HRule name( "name", regex( "\\<[a-zA-Z_][a-zA-Z0-9_]*\\>" ) );
	HRule expression( "expression" );
	HRule absoluteValue( "absoluteValue", '|' >> expression >> '|' );
	HRule parenthesis( "parenthesis", '(' >> expression >> ')' );
	HRule argList( "argList", expression >> ( * ( ',' >> expression ) ) );
	HRule functionCall( "functionCall", name >> '(' >> -argList >> ')' );
	HRule atom( "atom", absoluteValue | parenthesis | functionCall | real | name );
	HRule power( "power", atom >> ( * ( '^' >> atom ) ) );
	HRule multiplication( "multiplication", power >> ( * ( '*' >> power ) ) );
	HRule sum( "sum", multiplication >> ( * ( '+' >> multiplication ) ) );
	HRule value( "value", sum );
	HRule assignment( "assignment", *( name >> '=' ) >> value );
	expression %= assignment;
	HRule expressionList( "expressionList", + ( expression >> ';' ) );
	HRule statementList( "statementList" );
	HRule ifStatement( "ifStatement", executing_parser::constant( "if" ) >> '(' >> expression >> ')' >> '{' >> statementList >> '}' >> -( constant( "else" ) >> '{' >> statementList >> '}' ) );
	HRule whileStatement( "whileStatement", constant( "while" ) >> '(' >> expression >> ')' >> '{' >> statementList >> '}' );
	HRule caseStatement( "caseStatement", constant( "case" ) >> '(' >> integer >> ')' >> ':' >> '{' >> statementList >> '}' );
	HRule switchStatement( "switchStatement", constant( "switch" ) >> '(' >> expression >> ')' >> '{' >> +caseStatement >> '}' );
	HRule returnStatement( "returnStatement", constant( "return" ) >> '(' >> expression >> ')' );
	HRule statement( "statement", ifStatement | whileStatement | returnStatement | expressionList );
	statementList %= *statement;
	HRule scope( "scope", '{' >> statementList >> '}' );
	HRule nameList( "nameList", name >> ( * ( ',' >> name ) ) );
	HRule functionDefinition( "functionDefinition", name >> '(' >> -nameList >> ')' >> scope );
	HRule huginnGrammar( "huginnGrammar", * functionDefinition );
	return ( huginnGrammar );
	M_EPILOG
}

}

HHuginn::HHuginn( void )
	: _functions(), _engine( executing_parser::huginn_grammar() ) {
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

