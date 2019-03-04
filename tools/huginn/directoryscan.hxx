/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_DIRECTORYSCAN_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_DIRECTORYSCAN_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/filesystem.hxx"
#include "tools/hfsitem.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HDirectoryScan : public huginn::HIterable {
	typedef yaal::hcore::HResource<yaal::tools::HFSItem::HIterator> fs_iter_t;
	yaal::tools::filesystem::path_t _path;
	fs_iter_t _it;
	fs_iter_t _end;
public:
	HDirectoryScan( huginn::HClass const* class_, yaal::tools::filesystem::path_t const& path_ )
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
	static HHuginn::class_t get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			runtime_->create_class(
				"DirectoryScan",
				"The `DirectoryScan` class represents ongoing directory scan operation.",
				HHuginn::ACCESS::PRIVATE
			)
		);
		c->set_origin( origin_ );
		runtime_->huginn()->register_class( c );
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		throw HHuginn::HHuginnRuntimeException( "Getting size of `DirectoryScan' is an invalid operation.", thread_->current_frame()->file_id(), position_ );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HDirectoryScan>( HIterable::get_class(), _path ) );
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
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _it != _end );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HDirectoryScanIterator( HDirectoryScanIterator const& ) = delete;
	HDirectoryScanIterator& operator = ( HDirectoryScanIterator const& ) = delete;
};

HDirectoryScan::iterator_t HDirectoryScan::do_iterator( HThread*, int ) {
	return ( hcore::make_pointer<HDirectoryScanIterator>( *_it, *_end, this ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_DIRECTORYSCAN_HXX_INCLUDED */

