/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	enum class ROLE {
		INVALID,
		MAIN,		/*!< main control of a window, contents of this control holds
									 data of main record-set of a window */

		FILTER,	/*!< this kind of control is meant to setup filter for main
									control for `view mode' */
		DATA		/*!< this kind of control is used to store record-set data in `edit mode' */
	};
	/*! \brief control types
	 */
	enum class TYPE {
		INVALID,
		EDIT,
		LIST,
		TREE,
		COMBO,
		DATE,
		CHECK
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

