/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

  HEditWidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HEDITWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HEDITWIDGET_HXX_INCLUDED 1

#include "hcore/hlist.hxx"
#include "hcore/hregex.hxx"
#include "hconsole/hwidget.hxx"
#include "hconsole/hwidgetfactory.hxx"

namespace yaal {

namespace hconsole {

extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskLetters_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskDigits_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskAlpha_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskExtended_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskLoose_;
extern M_YAAL_HCONSOLE_PUBLIC_API char const* const _maskDefault_;

class HInfoString : public HInfo {
private:
	yaal::hcore::HString& _data;
public:
	typedef HInfoString this_type;
	typedef HInfo base_type;
	HInfoString( yaal::hcore::HString& );
	HInfoString( HInfoString const& );
	virtual ~HInfoString( void );
protected:
	virtual int long long do_get_integer( void ) const override;
	virtual double long do_get_real( void ) const override;
	virtual yaal::hcore::HString const& do_get_string( void ) const override;
	virtual yaal::hcore::HTime const& do_get_time( void ) const override M_DEBUG_CODE( __attribute__(( noreturn )) );
	virtual void do_set_integer( int long long ) override;
	virtual void do_set_real( double long ) override;
	virtual void do_set_string( yaal::hcore::HString const& ) override;
	virtual void do_set_time( yaal::hcore::HTime const& ) override;
private:
	void purge( void );
};

/*! \brief Implementation of TUI Edit widget class.
 *
 * Edit widget allows convenient text editing with handful
 * of advanced automatic text operations.
 */
class HEditWidget : public virtual HWidget {
public:
	typedef HEditWidget this_type;
	typedef HWidget base_type;
protected:
	bool _replace;         /*!< current edit mode */
	bool _multiLine;       /*!< is widget multiline */
	bool _readOnly;        /*!< shall editing be disabled */
	bool _rightAligned;    /*!< is widget content right aligned,
	                            widget content can be right aligned
	                            only in single line mode */
	bool _password;        /*!< should be entered text visible */
	int  _maxStringSize;   /*!< maximum length of text in widget */
	int  _cursorPosition;  /*!< cursor position relative to widget beginning */
	int  _widgetOffset;   /*!< when content is bigger than widget size
	                            this variable keeps offset of first
	                            character shown */
	int  _maxHistoryLevel; /*!< how many instertions should history keep */
	yaal::hcore::HRegex _mask; /*!< regular expression describing what
	                                   characters and in what way can be entered */
	hcore::HString _string; /*!< widget content */
	HInfoString _infoString;
	typedef hcore::HList<yaal::hcore::HString> history_t;
	history_t	_history;    /*!< history of insertions */
	typedef hcore::HCyclicIterator<history_t> cyclic_iterator;
	cyclic_iterator _historyIt;
public:
	/*! \brief Constructor of edit widget.
	 *
	 * \param parent - parent window of this widget.
	 * \param top - widget top coordinate (parent relative).
	 * \param left - left widget coordinate (parent relative).
	 * \param height - height of a widget.
	 * \param width - width of a widget.
	 * \param label - label of a widget.
	 */
	HEditWidget( HWindow* parent, int top, int left,	int height, int width,
								yaal::hcore::HString const& label = "",
								HWidgetAttributesInterface const& = HWidgetAttributesInterface() );
	virtual ~HEditWidget( void );
	void set_flags( bool = false, bool = false );
	void set_text( yaal::hcore::HString const& );
	yaal::hcore::HString const& get_text( void ) const;
	void set_mask( yaal::hcore::HString const& );
	/*! \brief Set varius bits of internal structure.
	 *
	 * \param maxlen - maximum string length.
	 * \param val - initial content.
	 * \param mask - regex description of valid content of this edit widget.
	 * \param replace - replace mode flag.
	 * \param multiline - multi-line mode flag.
	 * \param readonly - read only mode flag.
	 * \param rightAlign - right alignment mode flag.
	 * \param password - hide entry content flag.
	 * \param maxhist - maximum number of entries in history of this widget.
	 */
	void set_bits( int const* maxlen,
			yaal::hcore::HString const* val, yaal::hcore::HString const* mask,
			bool const* replace, bool const* multiline,
			bool const* readonly, bool const* rightAlign,
			bool const* password, int const* maxhist );
protected:
	virtual void do_paint( void ) override;
	virtual int do_process_input( int ) override;
	virtual bool do_click( mouse::OMouse& ) override;
	virtual HInfo const& do_get_data( void ) const override;
	virtual void do_set_data( HInfo const& ) override;
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

typedef yaal::hcore::HExceptionT<HEditWidget, HWidgetException> HEditWidgetException;

class HEditWidgetAttributes : virtual public HWidgetAttributes {
	bool _replace;
	bool _replaceSet;
	bool _multiLine;
	bool _multiLineSet;
	bool _readOnly;
	bool _readOnlySet;
	bool _rightAligned;
	bool _rightAlignedSet;
	bool _password;
	bool _passwordSet;
	int  _maxStringSize;
	bool _maxStringSizeSet;
	int  _maxHistoryLevel;
	bool _maxHistoryLevelSet;
	yaal::hcore::HString _mask;
	bool _maskSet;
	yaal::hcore::HString _text;
	bool _textSet;
protected:
	virtual void do_apply( HWidget& ) const override;
public:
	HEditWidgetAttributes( void );
	HEditWidgetAttributes& replace( bool );
	HEditWidgetAttributes& multiline( bool );
	HEditWidgetAttributes& readonly( bool );
	HEditWidgetAttributes& rightaligned( bool );
	HEditWidgetAttributes& password( bool );
	HEditWidgetAttributes& max_string_size( int );
	HEditWidgetAttributes& max_history_level( int );
	HEditWidgetAttributes& mask( yaal::hcore::HString const& );
	HEditWidgetAttributes& text( yaal::hcore::HString const& );
};

class HEditWidgetCreator : virtual public HWidgetCreatorInterface {
protected:
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) override;
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HEDITWIDGET_HXX_INCLUDED */

