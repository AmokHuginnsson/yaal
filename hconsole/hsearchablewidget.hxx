/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hsearchablewidget.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hsearchablewidget.hxx
 * \brief Declaration of HSearchableWidget class.
 */

#ifndef YAAL_HCONSOLE_HSEARCHABLEWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HSEARCHABLEWIDGET_HXX_INCLUDED 1

#include "hconsole/hpattern.hxx"
#include "hconsole/hwidget.hxx"
#include "hconsole/hwidgetfactory.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Provision H*Control classes with search capabilities.
 */
class HSearchableWidget : public virtual HWidget {
public:
	typedef HSearchableWidget this_type;
	typedef HWidget base_type;
protected:
	bool _searchable;      /*!< are items searchable */
	bool _searchActivated; /*!< should we highlight last search */
	bool _filtered;        /*!< is content of list filtered through pattern */
	bool _backwards;       /*!< last search direction */
	HPattern _pattern;     /*!< used for searching */
public:
	HSearchableWidget( HWidgetAttributesInterface const& = HWidgetAttributesInterface() );
	virtual ~HSearchableWidget( void );
	void search( hcore::HString const&, bool );
	void set_searchable( bool );
protected:
	void highlight( int, int, int, bool );
	virtual void go_to_match( void ) = 0;
	virtual void go_to_match_previous( void ) = 0;
};

typedef yaal::hcore::HExceptionT<HSearchableWidget, HWidgetException> HSearchableWidgetException;

class HSearchableWidgetAttributes : virtual public HWidgetAttributes {
	bool _searchable;   /*!< can widget content be sorted */
	bool _searchableSet;
protected:
	virtual void do_apply( HWidget& ) const override;
public:
	HSearchableWidgetAttributes( void );
	HSearchableWidgetAttributes& searchable( bool );
};

class HSearchableWidgetCreator : virtual public HWidgetCreatorInterface {
protected:
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) override;
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HSEARCHABLEWIDGET_HXX_INCLUDED */

