/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  subprocess.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "subprocess.hxx"
#include "stream.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "stream.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HSubprocessClass : public HHuginn::HClass {
	HHuginn::class_t _streamClass;
public:
	HSubprocessClass(
		HHuginn* huginn_,
		HHuginn::type_id_t typeId_
	) : HHuginn::HClass(
			huginn_,
			typeId_,
			huginn_->identifier_id( "Subprocess" ),
			nullptr,
			HHuginn::field_names_t{
				"is_alive",
				"kill",
				"get_pid",
				"in",
				"out",
				"err"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HSubprocess::is_alive, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HSubprocess::kill, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HSubprocess::get_pid, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HSubprocess::stream, "Subprocess.in", &HPipedChild::stream_in, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HSubprocess::stream, "Subprocess.out", &HPipedChild::stream_out, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HSubprocess::stream, "Subprocess.err", &HPipedChild::stream_err, _1, _2, _3, _4 ) )
			}
		)
		, _streamClass( HStream::get_class( huginn_ ) ) {
		return;
	}
	HHuginn::HClass const* stream_class( void ) const {
		M_PROLOG
		return ( _streamClass.raw() );
		M_EPILOG
	}
};

HSubprocess::HSubprocess( HHuginn::HClass const* class_, HHuginn::values_t const& argv_ )
	: HHuginn::HObject( class_ )
	, _pipedChild() {
	HPipedChild::argv_t argv;
	hcore::HString program( get_string( argv_[0] ) );
	for ( int i( 1 ), C( static_cast<int>( argv_.get_size() ) ); i < C; ++ i ) {
		argv.push_back( get_string( argv_[i] ) );
	}
	_pipedChild.spawn( program, argv );
	return;
}

HHuginn::value_t HSubprocess::is_alive(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	M_PROLOG
	char const name[] = "Subprocess.is_alive";
	verify_arg_count( name, values_, 0, 0, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	return ( thread_->huginn().object_factory()->create_boolean( o->_pipedChild.is_running() ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::kill(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	M_PROLOG
	char const name[] = "Subprocess.kill";
	verify_arg_count( name, values_, 0, 0, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	HPipedChild::STATUS s( o->_pipedChild.finish() );
	return ( thread_->huginn().object_factory()->create_integer( s.value ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::get_pid(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	M_PROLOG
	char const name[] = "Subprocess.get_pid";
	verify_arg_count( name, values_, 0, 0, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	return ( thread_->huginn().object_factory()->create_integer( o->_pipedChild.get_pid() ) );
	M_EPILOG
}

HHuginn::value_t HSubprocess::stream(
	char const* name_,
	stream_getter_t streamGetter_,
	huginn::HThread*,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	M_PROLOG
	verify_arg_count( name_, values_, 0, 0, position_ );
	HSubprocess* o( static_cast<HSubprocess*>( object_->raw() ) );
	HSubprocessClass const* c( static_cast<HSubprocessClass const*>( o->HHuginn::HValue::get_class() ) );
	HHuginn::HClass const* sc( c->stream_class() );
	return ( make_pointer<HStream>( sc, (o->_pipedChild.*streamGetter_)() ) );
	M_EPILOG
}

HHuginn::class_t HSubprocess::get_class( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		huginn_->create_class(
			HHuginn::class_constructor_t(
				[&huginn_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HSubprocessClass>(
							huginn_,
							typeId_
						)
					);
				}
			)
		)
	);
	huginn_->register_class( c );
	return ( c );
	M_EPILOG
}

}

}

}

