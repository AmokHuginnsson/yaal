/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HDATA_HDICTIONARY_HXX_INCLUDED
#define YAAL_HDATA_HDICTIONARY_HXX_INCLUDED 1

#include "dbwrapper/hcruddescriptor.hxx"

namespace yaal {

namespace hdata {

class HDictionary {
public:
	typedef HDictionary this_type;
	typedef yaal::hcore::HPointer<HDictionary> ptr_t;
	typedef yaal::hcore::HHashMap<int, yaal::hcore::HString> data_t;
private:
	data_t _data;
	dbwrapper::HCRUDDescriptor _crud;
public:
	HDictionary( yaal::dbwrapper::database_ptr_t const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const&,
			yaal::hcore::HString const& );
	void load( void );
	yaal::hcore::HString const& operator[] ( int ) const;
	data_t const& data( void ) const;
private:
	HDictionary( HDictionary const& ) = delete;
	HDictionary& operator = ( HDictionary const& ) = delete;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDICTIONARY_HXX_INCLUDED */

