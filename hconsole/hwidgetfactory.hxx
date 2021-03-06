/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hconsole/hwidgetfactory.hxx
 * \brief Declaration of HWidgetFactory class.
 */

#ifndef YAAL_HCONSOLE_HWIDGETFACTORY_HXX_INCLUDED
#define YAAL_HCONSOLE_HWIDGETFACTORY_HXX_INCLUDED 1

#include "hcore/hsingleton.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hwidget.hxx"

namespace yaal {

namespace hconsole {

class HWidgetCreatorInterface {
public:
	struct OResource {
		int _row;           /*!< top coordinate of control */
		int _column;        /*!< left coordinate of control */
		int _height;        /*!< height of control */
		int _width;         /*!< width of control */
		yaal::hcore::HString _label; /*!< control label */
		OResource( void );
	};
protected:
	virtual void do_initialize_globals( void ) {};
	virtual void do_cleanup_globals( void ) {};
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& ) = 0;
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& ) = 0;
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& ) = 0;
public:
	virtual ~HWidgetCreatorInterface( void ) {}
	void initialize_globals( void );
	void cleanup_globals( void );
	HWidget::ptr_t new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& );
	void apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
	void prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& );
	OResource get_resource( yaal::tools::HXml::HConstNodeProxy const& );
	typedef yaal::hcore::HPointer<HWidgetCreatorInterface> ptr_t;
};

class M_YAAL_HCONSOLE_PUBLIC_API HWidgetFactory : public yaal::hcore::HSingleton<HWidgetFactory> {
public:
	typedef HWidgetFactory this_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HWidgetCreatorInterface::ptr_t> creators_t;
private:
	creators_t _creators;
public:
	void register_widget_creator( yaal::hcore::HString const&, HWidgetCreatorInterface::ptr_t );
	HWidget::ptr_t create_widget( HWindow*, yaal::tools::HXml::HConstNodeProxy const& );
	bool is_type_valid( yaal::hcore::HString const& );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
	void initialize_globals( void );
	void cleanup_globals( void );
private:
	HWidgetFactory( void ) : _creators() {}
	~HWidgetFactory( void ) {}
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HWidgetFactory>;
	friend class yaal::hcore::HDestructor<HWidgetFactory>;
};

typedef yaal::hcore::HExceptionT<HWidgetFactory> HWidgetFactoryException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HWIDGETFACTORY_HXX_INCLUDED */

