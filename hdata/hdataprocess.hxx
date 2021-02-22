/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hdata/hdataprocess.hxx
 * \brief Declaration of HDataProcess class.
 */

#ifndef YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED
#define YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED 1

#include "hconsole/htuiprocess.hxx"
#include "hconsole/hwindow.hxx"
#include "hconsole/omenuitem.hxx"
#include "tools/hxml.hxx"
#include "dbwrapper/hdatabase.hxx"

namespace yaal {

namespace hdata {

/*! \brief A backbone of database enabled TUI based application.
 *
 * HDataProcess enables rapid building of database connected TUI
 * applications with simple XML resource files.
 */
class HDataProcess : public hconsole::HTUIProcess {
public:
	typedef HDataProcess this_type;
	typedef HTUIProcess base_type;
protected:
	dbwrapper::database_ptr_t _dataBase;
public:
	HDataProcess( void );
	virtual ~HDataProcess( void );
/* cppcheck-suppress functionConst */
	dbwrapper::database_ptr_t data_base( void );
	void init_xrc_db( yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
protected:
	virtual void do_quit( void ) override;
	virtual void do_close_window( void ) override;
	virtual void do_init_xrc( yaal::hcore::HString const&, yaal::hcore::HString const& ) override;
	virtual void do_init_xrc_db( yaal::hcore::HString const&, yaal::hcore::HString const&, yaal::hcore::HString const& );
	void force_close_window( void );
	void force_quit( void );
private:
	HDataProcess( HDataProcess const& );
	HDataProcess& operator = ( HDataProcess const& );
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATAPROCESS_HXX_INCLUDED */

