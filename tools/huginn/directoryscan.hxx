/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  directoryscan.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_DIRERCTORYSCAN_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_DIRERCTORYSCAN_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/hfsitem.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HDirectoryScan : public HHuginn::HIterable {
	typedef yaal::hcore::HResource<yaal::tools::HFSItem::HIterator> fs_iter_t;
	yaal::hcore::HString _path;
	fs_iter_t _it;
	fs_iter_t _end;
public:
	HDirectoryScan( HHuginn::HClass const* class_, yaal::hcore::HString const& path_ )
		: HIterable( class_ )
		, _path( path_ )
		, _it()
		, _end() {
		M_PROLOG
		yaal::tools::HFSItem dir( path_ );
		_it = hcore::make_resource<yaal::tools::HFSItem::HIterator>( dir.begin() );
		_end = hcore::make_resource<yaal::tools::HFSItem::HIterator>( dir.end() );
		return;
		M_EPILOG
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		return (
			runtime_->create_class(
				"DirectoryScan",
				nullptr,
				HHuginn::field_definitions_t{},
				"The `DirectoryScan` class represents ongoing directory scan operation."
			)
		);
		M_EPILOG
	}
protected:
	virtual int long do_size( void ) const override {
		throw HHuginn::HHuginnRuntimeException( "Getting size of `DirectoryScan' is an invalid operation.", 0 );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HRuntime* ) const override {
		return ( yaal::hcore::make_pointer<HDirectoryScan>( HIterable::get_class(), _path ) );
	}
};

class HDirectoryScanIterator : public HIteratorInterface {
	yaal::tools::HFSItem::HIterator& _it;
	yaal::tools::HFSItem::HIterator& _end;
	HObjectFactory* _objectFactory;
public:
	HDirectoryScanIterator( yaal::tools::HFSItem::HIterator& it_, yaal::tools::HFSItem::HIterator& end_, HDirectoryScan* directoryScan_ )
		: _it( it_ )
		, _end( end_ )
		, _objectFactory( directoryScan_->HIterable::get_class()->runtime()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _objectFactory->create_string( _it->get_name() ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _end );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HDirectoryScanIterator( HDirectoryScanIterator const& ) = delete;
	HDirectoryScanIterator& operator = ( HDirectoryScanIterator const& ) = delete;
};

HDirectoryScan::HIterator HDirectoryScan::do_iterator( HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HDirectoryScanIterator( *_it, *_end, this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_DIRERCTORYSCAN_HXX_INCLUDED */

