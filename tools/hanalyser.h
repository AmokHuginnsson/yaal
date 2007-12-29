/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	analyser.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_TOOLS_HANALYSER_H
#define __YAAL_TOOLS_HANALYSER_H

#include "hcore/hstring.h"
#include "hcore/htree.h"
#include "hcore/hpool.h"

namespace yaal
{

namespace tools
{

class HAnalyser
	{
	typedef enum
		{
		E_OK = 0,
		E_UNKNOWN_MNEMONIC = 1,
		E_UNEXPECTED_TERMINATION = 2,
		E_CLOSING_BRACKET_EXPECTED = 3,
		E_CLOSING_ABSOLUTE_EXPECTED = 4,
		E_CLOSING_FUNCTION_BRACKET_EXPECTED = 5,
		E_OPENING_FUNCTION_BRACKET_EXPECTED = 6,
		E_DIGIT_EXPECTED = 7,
		E_UNEXPECTED_TOKEN = 8,
		E_PREMATURE_TERMINATION = 9
		} syntax_error_t;
	typedef hcore::HList<int> int_list_t;
	struct OEquationElement;
	typedef hcore::HTree<OEquationElement> tree_t;
	struct OEquationElement
		{
		typedef double ( HAnalyser::* METHOD_t ) ( tree_t::const_node_t );
		METHOD_t METHOD;
		int_list_t f_oVariables;
		OEquationElement() : METHOD(), f_oVariables() {}
		};
	int f_iIndex;
	int f_iLength;
	syntax_error_t f_eError;
	double	f_pdVariables[ 26 ];
	hcore::HPool<double> f_oConstantsPool;
	hcore::HPool<int> f_oTerminalIndexes;
	hcore::HString	f_oFormula;
	tree_t f_oEquationTree;
public:
	/*{*/
	HAnalyser( void );
	virtual ~HAnalyser( void );
	double* analyse( char const* );
	double& operator [] ( int );
	double count( void );
	char const* get_error( void ) const;
	int get_error_token( void ) const;
private:
	bool translate( char const* );
	bool addition_production( tree_t::node_t );
	bool multiplication_production( tree_t::node_t );
	bool power_production( tree_t::node_t );
	bool signum_production( tree_t::node_t );
	bool terminal_production( tree_t::node_t );
	double count_branch( tree_t::const_node_t );
	double addition( tree_t::const_node_t );
	double multiplication( tree_t::const_node_t );
	double power( tree_t::const_node_t );
	double signum( tree_t::const_node_t );
	double bracket( tree_t::const_node_t );
	double functions( tree_t::const_node_t );
	void shorten_the_branch( tree_t::node_t );
	};

}

}

#endif /* not __YAAL_TOOLS_HANALYSER_H */
