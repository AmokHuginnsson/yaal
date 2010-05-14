/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hlogpad.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED
#define YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED

#include "hcore/hlist.hxx"
#include "hconsole/hcontrol.hxx"

namespace yaal
{
	
namespace hconsole
{

/*! \brief Implementation of TUI LogPad control class.
 *
 * LogPad is read only TUI control that is able to display formatted text.
 */
class HLogPad : public HControl
	{
	/*! \brief Basic unit of data for HLogPad display.
	 */
	class HLogLine
		{
		typedef enum
			{
			NONE,
			ATTRIBUTE,
			TEXT,
			TEXT_EOL
			} type_t;
		type_t _type;
		int _attribute;
		hcore::HString _text;
	public:
		HLogLine ( void );
		virtual ~HLogLine ( void );
		friend class HLogPad;
		};
	typedef hcore::HList < HLogLine > contents_t;
	int _lines;
	int _offsetRow;
	int _offsetColumn;
	int _attribute;
	contents_t _contents;
public:
	HLogPad( HWindow*, int, int, int, int, char const * const );
	virtual ~HLogPad( void );
	void add( int, yaal::hcore::HString const& );
	void add( yaal::hcore::HString const& );
	void add( int );
protected:
	virtual int do_process_input ( int );
	virtual void do_refresh ( void );
	};

}

}

#endif /* not YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED */

