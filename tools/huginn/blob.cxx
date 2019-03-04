/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace blob {

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<huginn::HClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::BLOB ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::BLOB ) ),
			"The `blob` represents raw memory buffer.",
			&builtin::blob
		)
	);
	return ( c );
	M_EPILOG
}

}

}

huginn::HBlob::HBlob( huginn::HClass const* class_, yaal::hcore::HChunk&& data_ )
	: HValue( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

int long huginn::HBlob::get_size( void ) const {
	return ( _data.get_size() );
}

huginn::HBlob::value_type const& huginn::HBlob::value( void ) const {
	return ( _data );
}

HHuginn::value_t huginn::HBlob::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HChunk c;
	try {
		c.realloc( _data.get_size(), HChunk::STRATEGY::EXACT );
		memcpy( c.raw(), _data.raw(), static_cast<size_t>( _data.get_size() ) );
	} catch ( hcore::HException const& e ) {
		thread_->raise( thread_->object_factory().runtime_exception_class(), e.what(), position_ );
	}
	return ( thread_->runtime().object_factory()->create_blob( yaal::move( c ) ) );
}

}

}

