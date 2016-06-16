/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hlogpad.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED
#define YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hconsole/hwidget.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI LogPad control class.
 *
 * LogPad is read only TUI control that is able to display formatted text.
 */
class HLogPad : public HWidget {
public:
	typedef HLogPad this_type;
	typedef HWidget base_type;
private:
	/*! \brief Basic unit of data for HLogPad display.
	 */
	class HLogLine {
		typedef enum {
			NONE,
			ATTRIBUTE,
			TEXT,
			TEXT_EOL
		} type_t;
		type_t _type;
		int _attribute;
		hcore::HString _text;
	public:
		HLogLine( void );
		virtual ~HLogLine( void );
		friend class HLogPad;
	};
	typedef hcore::HList<HLogLine> contents_t;
	int _lines;
	int _offsetRow;
	int _offsetColumn;
	int _attribute;
	contents_t _contents;
public:
	HLogPad( HWindow*, int, int, int, int, yaal::hcore::HString const&,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface() );
	virtual ~HLogPad( void );
	void add( int, yaal::hcore::HString const& );
	void add( yaal::hcore::HString const& );
	void add( int );
protected:
	void add( HLogLine const& );
	virtual int do_process_input( int ) override;
	virtual void do_paint( void ) override;
};

typedef yaal::hcore::HExceptionT<HLogPad, HWidgetException> HLogPadException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HLOGPAD_HXX_INCLUDED */

