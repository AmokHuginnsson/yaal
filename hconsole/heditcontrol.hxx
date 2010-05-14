/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	HEditControl.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HEDITCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HEDITCONTROL_HXX_INCLUDED

#include "hcore/hlist.hxx"
#include "hcore/hpattern.hxx"
#include "hconsole/hcontrol.hxx"

namespace yaal
{

namespace hconsole
{

extern char const* const _maskLetters_;
extern char const* const _maskDigits_;
extern char const* const _maskAlpha_;
extern char const* const _maskExtended_;
extern char const* const _maskLoose_;
extern char const* const _maskDefault_;

/*! \brief Implementation of TUI Edit control class.
 *
 * Edit control allows convenient text editing with handful
 * of advanced automatic text operations.
 */
class HEditControl : public virtual HControl
	{
protected:
	typedef HEditControl self_t;
	typedef HControl hier_t;
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
	virtual hcore::HInfo get( void );
	virtual int set_focus( char = 0 );
	virtual void set( hcore::HInfo const& );
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

}

}

#endif /* not YAAL_HCONSOLE_HEDITCONTROL_HXX_INCLUDED */
