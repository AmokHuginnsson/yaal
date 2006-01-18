/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	analyser.h - this file is integral part of `stdhapi' project.

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

#ifndef __TOOLS_HANALYSER_H
#define __TOOLS_HANALYSER_H

#include "hcore/hstring.h"
#include "hcore/htree.h"
#include "hcore/hpool.h"

namespace stdhapi
{

namespace tools
{

typedef hcore::HList < double * > double_ptr_list_t;
class HAnalyser : public hcore::HTree < double_ptr_list_t >
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
	typedef hcore::HTree < double_ptr_list_t >::HNode * ANALYZER_NODE_PTR_t;
	class HAnalyserNode;
	friend class HAnalyserNode;
	class HAnalyserNode : public hcore::HTree < double_ptr_list_t > ::HNode
		{
		typedef double ( HAnalyser::* METHOD_t ) ( HAnalyserNode * );
	protected:
		/*{*/
		METHOD_t METHOD;
		/*}*/
	public:
		/*{*/
		/*}*/
	protected:
		/*{*/
		HAnalyserNode ( HAnalyserNode * );
		HAnalyserNode * grow_up_branch ( int = D_FILL_NEW_AUTO );
		/*}*/
		friend class HAnalyser;
		};
protected:
	/*{*/
	int f_iIndex;
	int f_iLength;
	syntax_error_t f_eError;
	double	f_pdVariables [ 26 ];
	hcore::HPool < double > f_oConstantsPool;
	hcore::HPool < int > f_oTerminalIndexes;
	hcore::HString	f_oFormula;
	/*}*/
public:
	/*{*/
	HAnalyser ( void );
	virtual ~HAnalyser ( void );
	double * analyse ( char const * );
	double & operator [ ] ( int );
	double count ( void );
	char const * get_error ( void ) const;
	int get_error_token ( void ) const;
	/*}*/
protected:
	/*{*/
	bool translate ( char const * );
	bool addition_production ( HAnalyserNode * );
	bool multiplication_production ( HAnalyserNode * );
	bool power_production ( HAnalyserNode * );
	bool signum_production ( HAnalyserNode * );
	bool terminal_production ( HAnalyserNode * );
	double count_branch ( HAnalyserNode * );
	double addition ( HAnalyserNode * );
	double multiplication ( HAnalyserNode * );
	double power ( HAnalyserNode * );
	double signum ( HAnalyserNode * );
	double bracket ( HAnalyserNode * );
	double functions ( HAnalyserNode * );
	/*}*/
	};

}

}

#endif /* not __TOOLS_HANALYSER_H */
