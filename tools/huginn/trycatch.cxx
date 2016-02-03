/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  trycatch.cxx - this file is integral part of `yaal' project.

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
#include "trycatch.hxx"
#include "thread.hxx"
#include "scope.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace tools {

namespace huginn {

HTryCatch::HTryCatch( HHuginn::scope_t const& try_, catches_t const& catches_ )
	: HStatement()
	, _try( try_ )
	, _catches( catches_ ) {
	_try->make_inline();
	return;
}

void HTryCatch::do_execute( huginn::HThread* thread_ ) const {
	M_PROLOG
	thread_->create_try_catch_frame();
	_try->execute( thread_ );
	if ( thread_->has_exception() ) {
		HFrame* f( thread_->current_frame() );
		HHuginn::value_t v( f->result() );
		HHuginn::HObject* e( dynamic_cast<HHuginn::HObject*>( v.raw() ) );
		bool handled( false );
		for ( OCatch const& c : _catches ) {
			if ( ( e && e->is_kind_of( c._type ) ) || ( ! e && ( v->get_class()->identifier_id() == c._type ) ) ) {
				f->set_variable( c._identifier, v, c._position );
				c._scope->execute( thread_ );
				handled = true;
				break;
			}
		}
		thread_->pop_frame();
		if ( ! handled ) {
			thread_->break_execution( HFrame::STATE::EXCEPTION, yaal::move( v ) );
		}
	} else {
		thread_->pop_frame();
	}
	return;
	M_EPILOG
}

}

}

}

