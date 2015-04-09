/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatawidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HDATA_HDATAWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATAWIDGET_HXX_INCLUDED 1

#include "dbwrapper/hcruddescriptor.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hwidget.hxx"

namespace yaal {

namespace hdata {

class HDataWindow;

/*! \brief Base class for database connected TUI controls.
 */
class HDataWidget : public virtual hconsole::HWidget {
public:
	/*! \brief control role flags
	 */
	struct ROLE {
		/*! \brief the flags
		 */
		typedef enum {
			INVALID,
			MAIN,		/*!< main control of a window, contents of this control holds
										 data of main record-set of a window */

			FILTER,	/*!< this kind of control is meant to setup filter for main
										control for `view mode' */
			DATA		/*!< this kind of control is used to store record-set data in `edit mode' */
		} role_t;
	};
	/*! \brief control types
	 */
	struct TYPE {
		typedef enum {
			INVALID,
			EDIT,
			LIST,
			TREE,
			COMBO,
			DATE,
			CHECK
		} type_t;
	};
protected:
	dbwrapper::HCRUDDescriptor::ptr_t _crud;
	yaal::hcore::HString _viewQuery;
public:
	HDataWidget( void );
	virtual ~HDataWidget( void );
	void set_crud_descriptor( dbwrapper::HCRUDDescriptor::ptr_t );
	int long get_current_id( void );
	void load( int long = 0 );
	void save( int long = 0 );
	void add_new( void );
	void cancel_new( void );
	void set_view_query( yaal::hcore::HString const& );
protected:
	virtual void do_load( int long );
	virtual void do_save( int long );
	virtual void do_add_new( void );
	virtual void do_cancel_new( void );
	virtual int long do_get_current_id( void );
private:
	HDataWidget ( HDataWidget const & );
	HDataWidget & operator = ( HDataWidget const & );
};

void sync( dbwrapper::HRecordSet::value_t&, hconsole::HInfo const&, TYPE );
void apply_role( HDataWindow*, HDataWidget*, yaal::tools::HXml::HConstNodeProxy );

}

}

#endif /* #ifndef YAAL_HDATA_HDATAWIDGET_HXX_INCLUDED */

