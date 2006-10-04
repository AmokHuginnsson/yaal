/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hlogpad.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HLOGPAD_H
#define __YAAL_HCONSOLE_HLOGPAD_H

#include "hcore/hlist.h"
#include "hconsole/hcontrol.h"

namespace yaal
{
	
namespace hconsole
{
	
class HLogPad : public HControl
	{
	class HLogLine
		{
		typedef enum
			{
			D_NONE,
			D_ATTRIBUTE,
			D_TEXT,
			D_TEXT_EOL
			} type_t;
		type_t f_eType;
		int f_iAttribute;
		hcore::HString f_oText;
	public:
		HLogLine ( void );
		virtual ~HLogLine ( void );
		friend class HLogPad;
		};
	typedef hcore::HList < HLogLine > contents_t;
	int f_iLines;
	int f_iOffsetRow;
	int f_iOffsetColumn;
	int f_iAttribute;
	contents_t f_oContents;
public:
	HLogPad ( HWindow *, int, int, int, int, char const * const );
	virtual ~HLogPad ( void );
	void add ( int, char const * const );
	void add ( char const * const );
	void add ( int );
protected:
	virtual int process_input ( int );
	virtual void refresh ( void );
	};

}

}

#endif /* not __YAAL_HCONSOLE_HLOGPAD_H */

