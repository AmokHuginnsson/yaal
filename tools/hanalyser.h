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

#include "../hcore/hstring.h"
#include "../hcore/htree.h"
#include "../hcore/hpool.h"

class HAnalyser : public HTree < HList < double * > >
	{
	class HAnalyserNode;
	friend class HAnalyserNode;
	class HAnalyserNode : public HTree < HList < double * > > ::HNode
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
	int f_iError;
	double	f_pdVariables [ 26 ];
	HPool < double > f_oConstantsPool;
	HString	f_oFormula;
	/*}*/
public:
	/*{*/
	HAnalyser ( void );
	virtual ~HAnalyser ( void );
	double * analyse ( const char * );
	double & operator [ ] ( int );
	double count ( void );
	/*}*/
protected:
	/*{*/
	int translate ( const char * );
	void addition_production ( HAnalyserNode * );
	void multiplication_production ( HAnalyserNode * );
	void power_production ( HAnalyserNode * );
	void signum_production ( HAnalyserNode * );
	void terminal_production ( HAnalyserNode * );
	double count_branch ( HAnalyserNode * );
	double addition ( HAnalyserNode * );
	double multiplication ( HAnalyserNode * );
	double power ( HAnalyserNode * );
	double signum ( HAnalyserNode * );
	double bracket ( HAnalyserNode * );
	double functions ( HAnalyserNode * );
	/*}*/
	};

#endif /* not __TOOLS_HANALYSER_H */
