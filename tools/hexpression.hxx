/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hexpression.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_TOOLS_HEXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HEXPRESSION_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/htree.hxx"
#include "hcore/harray.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Mathematical expression analyser and evaluator.
 */
class HExpression
	{
	typedef HExpression this_type;
	typedef enum
		{
		OK = 0,
		UNKNOWN_MNEMONIC = 1,
		UNEXPECTED_TERMINATION = 2,
		CLOSING_BRACKET_EXPECTED = 3,
		CLOSING_ABSOLUTE_EXPECTED = 4,
		CLOSING_FUNCTION_BRACKET_EXPECTED = 5,
		OPENING_FUNCTION_BRACKET_EXPECTED = 6,
		DIGIT_EXPECTED = 7,
		UNEXPECTED_TOKEN = 8,
		PREMATURE_TERMINATION = 9
		} syntax_error_t;
	typedef hcore::HList<int> int_list_t;
	struct OEquationElement;
	typedef hcore::HTree<OEquationElement> tree_t;
	struct OEquationElement
		{
		typedef double long ( HExpression::* METHOD_t ) ( tree_t::const_node_t );
		METHOD_t METHOD;
		int_list_t _variables;
		OEquationElement() : METHOD(), _variables() {}
		};
	int _index;
	int _length;
	syntax_error_t _error;
	double long _variables[ 26 ];
	hcore::HArray<double long> _constantsPool;
	hcore::HArray<int> _terminalIndexes;
	hcore::HString	_formula;
	tree_t _equationTree;
public:
	/*{*/
	HExpression( void );
	HExpression( yaal::hcore::HString const& );
	HExpression( HExpression const& );
	HExpression& operator = ( HExpression const& );
	virtual ~HExpression( void );
	void swap( HExpression& );
	double long* compile( yaal::hcore::HString const& );
	double long* variables( void );
	double long& operator [] ( int );
	double long evaluate( void );
	char const* get_error( void ) const;
	int get_error_token( void ) const;
private:
	bool translate( yaal::hcore::HString const& );
	bool addition_production( tree_t::node_t );
	bool multiplication_production( tree_t::node_t );
	bool power_production( tree_t::node_t );
	bool signum_production( tree_t::node_t );
	bool terminal_production( tree_t::node_t );
	double long count_branch( tree_t::const_node_t );
	double long addition( tree_t::const_node_t );
	double long multiplication( tree_t::const_node_t );
	double long power( tree_t::const_node_t );
	double long signum( tree_t::const_node_t );
	double long bracket( tree_t::const_node_t );
	double long functions( tree_t::const_node_t );
	void shorten_the_branch( tree_t::node_t );
	};

typedef yaal::hcore::HExceptionT<HExpression> HExpressionException;

}

}

#endif /* #ifndef YAAL_TOOLS_HEXPRESSION_HXX_INCLUDED */

