/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hwidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HWIDGET_HXX_INCLUDED 1

#include "hconsole/console.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hstring.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hinfo.hxx"
#include "hconsole/hevent.hxx"

namespace yaal {

namespace hconsole {

class HWindow;

class HWidget;
class HWidgetAttributesInterface {
public:
	virtual ~HWidgetAttributesInterface( void ) {}
	void apply( HWidget& ) const;
	template<typename T>
	T& up( void ) {
		return ( dynamic_cast<T&>( *this ) );
	}
private:
	virtual void do_apply( HWidget& ) const {}
};

/*! \brief Base class for all TUI widget classes.
 *
 * This class is a common interface for all TUI widget classes.
 */
class HWidget : public yaal::hcore::HPointerFromThisInterface<HWidget> {
public:
	typedef HWidget this_type;
	struct OAttribute {
		int _label;
		int _data;
		bool operator == ( OAttribute const& attr_ ) const;
	};
/*! \brief Various configuration bits for HWidget hierarchy.
 */
	struct BITS {
		/*! \brief Possible alignment types of text in HWidget.
		 */
		enum class ALIGN {
			LEFT,   /*!< text ought to be left aligned */
			CENTER, /*!< text ought to be centered */
			RIGHT   /*!< text ought to be right aligned */
		};
	};
	/*! \brief How widget label should be painted.
	 */
	struct LABEL {
		/*! \brief Where should label be painted relative to widget contents.
		 */
		enum class POSITION {
			SIDE_BY_SIDE, /*!< Label is on the left of widget contents. */
			STACKED       /*!< Label is drawn above of the widget contents (TY aerofoam). */
		};
		enum class DECORATION {
			AUTO,
			EXPLICIT
		};
	};
	static OAttribute const DEFAULT_ATTRS; /*!< Default HWidget attribites (colors of label and data fore/back-ground) */
	typedef yaal::hcore::HPointer<HWidget> ptr_t;
protected:
	bool _enabled;    /*!< Tells if widget is enabled, focus can go only to enabled widget. */
	bool _focused;    /*!< Tells if widget has focus. */
	bool _drawLabel;  /*!< Will be label driven? */
	LABEL::POSITION _labelPosition; /*!< Is label in the same line as top of widget? */
	LABEL::DECORATION _labelDecoration; /*!< Should this label be auto-decorated? */
	/** \name Widget attributes.
	 * High byte of attribute, in all
	 * three cases keeps label (widget title)
	 * color, low byte keeps work place color */
	//@{
	OAttribute _attributeDisabled; /*!< The attribute of disabled control. */
	OAttribute _attributeEnabled;  /*!< The attribute of enabled control. */
	OAttribute _attributeFocused;  /*!< The attribute of focused control. */
	//@}
	int _row;       /*!< Widget top coordinate relative
	                     to parent window (top or bottom border). */
	int _column;    /*!< Widget left coordinate relative
	                     to parent window (left or right border). */
	int _height;    /*!< Height or coorinate of bottom of widget. */
	int _width;     /*!< Width or coorinate of right of widget. */
	int _rowRaw;    /*!< Widget top coordinate relative
	                     to parent window (after repositioning). */
	int _columnRaw; /*!< Widget left coordinate relative
	                     to parent window (after repositioning). */
	int _heightRaw; /*!< Drawing state keeping helper. */
	int _widthRaw;  /*!< Drawing state keeping helper. */
	hcore::HString _label;        /*!< Widget title. */
	hcore::HString _varTmpBuffer; /*!< Buffer for temporary operations, used
	                                   to keep memory fragmentation low. */
	HWindow* _window;   /*!< Window that will hold this widget. */
/* this is used locally, there is no way to modify this variables explicitly */
	int _labelLength;   /*!< Length of the label. */
	int _shortcutIndex; /*!< Index of shortcut char in label. */
	bool _valid;        /*!< Is model data in sync with widget internals. */
	bool _needRepaint;  /*!< Does this widget need to be repainted. */
	event_dispatcher_t _eventDispatcher;
public:
/*! \brief Widget constructor.
 *
 * When library user wants to create new widget he must provision
 * basic parameters shared by all types of TUI widgets.
 *
 * \param parent - The parent window for this widget.
 * \param row - Widget top coordinate relative to parent window (top or bottom border)
 * \param col - Widget left coordinate relative to parent window (left or right border).
 * \param height - Height or coorinate of bottom of widget.
 * \param width - Width or coorinate of right of widget.
 * \param label - Widget title.
 * \param attributes - Additional attributes for this widget.
 */
	HWidget( HWindow* parent, int row, int col, int height, int width,
			yaal::hcore::HString const& label,
			HWidgetAttributesInterface const& attributes = HWidgetAttributesInterface() );

/*! \brief Widget destructor.
 *
 * Trivial implementation.
 */
	virtual ~HWidget( void );

/*! \brief Change enabled/disabled state of the widget.
 *
 * \param enabled - Tells if widget shall be enabled.
 */
	void enable( bool enabled );

/*! \brief Process keyboard input.
 *
 * All keyboard input for this widget shall be handled inside this method call.
 *
 * \param keycode - Internal code for keypress event.
 * \return Unhandled portion of key press.
 */
	int process_input( int keycode );

/*! \brief Focus managing method (gain).
 *
 * Set focus if widget is enabled or if shortcut char is specified.
 *
 * \param shortcut - Alphanumeric portion of keypress event that possibly holds shortcut to this widget.
 * \return True iff focus was successfully switched to this widget.
 */
	virtual bool set_focus( char shortcut = 0 ); /* focused and shortcut char match */

/*! \brief Focus managing method (lose).
 *
 * Allows for focus removal from widget owner context.
 */
	void kill_focus( void );

/*! \brief Interface for setting widget data.
 *
 * Some widgets store data directly, this interface allows for setting
 * data from widget owner context.
 *
 * \param data - Data that shall be stored inside of this widget.
 */
	void set_data( HInfo const& data );

/*! \brief Interface for getting widget data.
 *
 * Some widgets store data directly, this interface allows for getting
 * data from widget from widget owner context.
 *
 * \return Data that has been stored inside of this widget.
 */
	HInfo const& get_data( void ) const;

/*! \brief Interface for checking is particular widget instance support searching.
 *
 * Some widget types support search operation, this method allows for checking
 * if this particular widget can be searched.
 *
 * \return True if widget can be searched, false otherwise.
 */
	virtual bool is_searchable( void );

/*! \brief Refresh TUI widget screen representation.
 */
	void paint( void );

/*! \brief Update widget internals to comply with new Model data.
 *
 * The external Model for this widget changed its data, now widget
 * must update its internal data structures to resychronize with
 * its Model.
 */
	void update( void );

/*! \brief Process mouse input.
 *
 * All mouse input for this widget shall be handled inside this method call.
 *
 * \param eventdesc - Full description of mouse event that occurred in this widget.
 * \return Return true iff click action was handled in whole by this widget.
 */
	bool click( mouse::OMouse& eventdesc );

/*! \brief Test if some point belongs to this widget.
 *
 * \param row - Row of the point to be tested.
 * \param col - Column of the point to be tested.
 * \return True if point with row, col coordinates is inside of this widget.
 */
	bool hit_test( int row, int col ) const;

/*! \brief Set optional widget attributes.
 *
 * \param attributes - Optional attributes to set for this widget.
 */
	void set_attributes( HWidgetAttributesInterface const& attributes );

/*! \brief Set foreground/background colors for an enabled widget.
 *
 * Attribute holds information about foreground/background colors of three
 * specific parts of a widget.
 *
 * Those parts are:
 *  - label
 *  - data
 *
 * \param attribute - Colors for widget in enabled state.
 */
	void set_attribute_enabled( OAttribute attribute );

/*! \brief Set foreground/background colors for an disabled widget.
 *
 * Attribute holds information about foreground/background colors of three
 * specific parts of a widget.
 *
 * Those parts are:
 *  - label
 *  - data
 *
 * \param attribute - Colors for widget in disabled state.
 */
	void set_attribute_disabled( OAttribute attribute );

/*! \brief Set foreground/background colors for an enabled widget in focused state.
 *
 * Attribute holds information about foreground/background colors of three
 * specific parts of a widget.
 *
 * Those parts are:
 *  - label
 *  - data
 *
 * \param attribute - Colors for widget in focused state.
 */
	void set_attribute_focused( OAttribute attribute );

/*! \brief Set various foreground/background colors for a widget.
 *
 * Each attribute holds information about foreground/background colors of three
 * specific parts of a widget.
 *
 * Those parts are:
 *  - label
 *  - data
 *
 * \param disabled - Colors for widget in disabled state.
 * \param enabled - Colors for widget in enabled state.
 * \param focused - Colors for widget while its focused.
 */
	void set_attributes( OAttribute disabled = DEFAULT_ATTRS,
			OAttribute enabled = DEFAULT_ATTRS,
			OAttribute focused = DEFAULT_ATTRS );

/*! \brief Decide if label shall be drawn for this particular instance of a widget.
 *
 * \param draw - If set to true the label is drawn.
 */
	void set_draw_label( bool draw );

/*! \brief Set label position.
 *
 * \param labelPosition - New position for label in this widget.
 */
	void set_label_position( LABEL::POSITION labelPosition );

/*! \brief Set label decoraton mode.
 *
 * \param decoration - New decoration mode for label in this widget.
 */
	void set_label_decoration( LABEL::DECORATION decoration );

/*! \brief Set new coordinates and size for a widget.
 *
 * \param row - New widget vertical position.
 * \param col - New widget horizontal position.
 * \param height - New widget height.
 * \param width -  New widget width.
 */
	void move( int row, int col, int height, int width );

/*! \brief Retrieve attribute (colors) of a widgets label.
 *
 * \return Attribute of widgets label.
 */
	int attr_label( void ) const;

/*! \brief Retrieve attribute (widgets) of a widgets shortcut highlight.
 *
 * \return Attribute of widgets shortcut highlight.
 */
	int attr_shortcut( void ) const;

/*! \brief Retrieve attribute (widgets) of a widgets data.
 *
 * \return Attribute of widgets data.
 */
	int attr_data( void ) const;

/*! \brief Schedule full paint on next refresh cycle.
 */
	void schedule_repaint( void );

/*! \brief Mark this widget as no longer synchronized with its internel model.
 */
	void invalidate( void );

/*! \brief Tells if this widget needs to be repainted.
 *
 * \return True iff paint is required.
 */
	bool need_repaint( void ) const;

/*! \brief Tell if this widget has input focus.
 *
 * \return True iff this widget currently holds input focus.
 */
	bool has_focus( void ) const;

/*! \brief Get widget label text.
 *
 * \return Current label text.
 */
	yaal::hcore::HString const& get_label( void ) const;

/*! \brief Get this widget parent window (owner).
 *
 * \return This widget parent window (owner).
 */
	HWindow* get_window( void ) const;

/*! \brief Register event listener.
 *
 * \param liserner - event listener to register.
 */
	void register_event_listener( event_listener_t const& );

protected:
	void set_attr_label( void ) const;
	void set_attr_shortcut( void ) const;
	void set_attr_data( void ) const;
	virtual int do_process_input( int );
	virtual void do_paint( void ) = 0;
	virtual void do_update( void );
	virtual void do_kill_focus( void );
	virtual bool do_hit_test( int, int ) const;
	virtual bool do_click( mouse::OMouse& );
	virtual void do_draw_label( void );
	virtual void do_set_data( HInfo const& data );
	virtual HInfo const& do_get_data( void ) const;
	void draw_label( void );
private:
	HWidget ( HWidget const& );
	HWidget& operator = ( HWidget const& );
};

typedef yaal::hcore::HExceptionT<HWidget> HWidgetException;

class HWidgetAttributes : public HWidgetAttributesInterface {
	bool _drawLabel;
	bool _drawLabelSet;
	HWidget::LABEL::POSITION _labelPosition;
	bool _labelPositionSet;
	HWidget::LABEL::DECORATION _labelDecoration;
	bool _labelDecorationSet;
	HWidget::OAttribute _attributeDisabled;
	bool _attributeDisabledSet;
	HWidget::OAttribute _attributeEnabled;
	bool _attributeEnabledSet;
	HWidget::OAttribute _attributeFocused;
	bool _attributeFocusedSet;
protected:
	virtual void do_apply( HWidget& ) const;
public:
	HWidgetAttributes( void );
	HWidgetAttributes& draw_label( bool );
	HWidgetAttributes& label_position( HWidget::LABEL::POSITION );
	HWidgetAttributes& label_decoration( HWidget::LABEL::DECORATION );
	HWidgetAttributes& attribure_enabled( HWidget::OAttribute const& );
	HWidgetAttributes& attribure_disabled( HWidget::OAttribute const& );
	HWidgetAttributes& attribure_focused( HWidget::OAttribute const& );
};

class HWidgetEvent : public HEvent {
public:
	typedef HWidgetEvent this_type;
	typedef HEvent base_type;
private:
	HWidget* _widget;
public:
	HWidgetEvent( HWidget* widget_ )
		: _widget( widget_ ) {
	}
	HWidgetEvent( HWidgetEvent const& ) = default;
	HWidgetEvent& operator = ( HWidgetEvent const& ) = default;
	HWidget* get_source( void ) const {
		return ( _widget );
	}
private:
	virtual TYPE do_get_type( void ) const {
		return ( TYPE::WINDOW );
	}
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWIDGET_HXX_INCLUDED */

