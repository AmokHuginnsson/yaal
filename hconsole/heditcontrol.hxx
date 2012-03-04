/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	HEditControl.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HEDITCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HEDITCONTROL_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/hpattern.hxx"
#include "hconsole/hcontrol.hxx"

namespace yaal {

namespace hconsole {

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskLetters_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskDigits_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskAlpha_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskExtended_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskLoose_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskDefault_;

/*! \brief Implementation of TUI Edit control class.
 *
 * Edit control allows convenient text editing with handful
 * of advanced automatic text operations.
 */
class HEditControl : public virtual HControl {
public:
	typedef HEditControl this_type;
	typedef HControl base_type;
protected:
	bool _replace;         /*!< current edit mode */
	bool _multiLine;       /*!< is control multiline */
	bool _readOnly;        /*!< shall editing be disabled */
	bool _rightAligned;    /*!< is control content right aligned,
														 control content can be right aligned
														 only in single line mode */
	bool _password;        /*!< should be entered text visible */
	int  _maxStringSize;   /*!< maximum length of text in control */
	int  _cursorPosition;  /*!< cursor position relative to control begining */
	int  _controlOffset;   /*!< when content is bigger than control size
														 this variable keeps offset of first
														 character shown */
	int  _maxHistoryLevel; /*!< how many instertions should history keep */
	hcore::HPattern _pattern; /*!< regular expression describing what
																characters and in what way can be entered */
	hcore::HString _string;   /*!< control content */
	HInfoString _infoString;
	typedef hcore::HList<yaal::hcore::HString> history_t;
	history_t	_history;    /*!< history of insertions */
	history_t::cyclic_iterator _historyIt;
public:
	/*! \brief Constructor of edit control.
	 *
	 * \param parent - parent window of this control.
	 * \param top - control top coordinate (parent relative).
	 * \param left - left control coordinate (parent relative).
	 * \param height - height of a control.
	 * \param width - width of a control.
	 * \param label - label of a control.
	 * \param maxlen - maximum string length.
	 * \param val - initial content.
	 * \param mask - regex description of valid content of this edit control.
	 * \param replace - replace mode flag.
	 * \param multiline - multi-line mode flag.
	 * \param readonly - read only mode flag.
	 * \param rightAlign - right alignment mode flag.
	 * \param password - hide entry content flag.
	 * \param maxhist - maximum number of entries in history of this control.
	 */
	HEditControl( HWindow* parent, int top, int left,	int height, int width,
								char const* label = "", int maxlen = 127,
								char const* val = "", char const* mask = _maskDefault_,
								bool replace = false, bool multiline = false,
								bool readonly = false, bool rightAlign = false,				
								bool password = false, int maxhist = 8 );
	virtual ~HEditControl( void );
	void set_flags( bool = false, bool = false );
	virtual HInfo const& get( void ) const;
	virtual int set_focus( char = 0 );
	virtual void set( HInfo const& );
	void set_text( yaal::hcore::HString const& );
	yaal::hcore::HString const& get_text( void ) const;
protected:
	virtual void do_refresh( void );
	virtual int do_process_input( int );
	virtual int do_click( mouse::OMouse& );
private:
	int find_eow( int );
	int go_to_eow( int );
	int kill_line( void );
	int move_right( int );
	int move_left( void );
	int go_to_end( int );
	int delete_char( int );
	int kill_char( void );
	int find_bow( int );
	int go_to_bow( int );
	int delete_word( int );
	int kill_word( int );
	int insert_char( int, int );
	int update_from_history( void );
};

typedef yaal::hcore::HExceptionT<HEditControl, HControlException> HEditControlException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HEDITCONTROL_HXX_INCLUDED */

