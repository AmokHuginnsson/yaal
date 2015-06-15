/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  for.cxx - this file is integral part of `yaal' project.

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
#include "for.hxx"
#include "thread.hxx"
#include "expression.hxx"
#include "scope.hxx"
#include "iterator.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HFor::HFor( yaal::hcore::HString const& variableName_,
	HHuginn::expression_t const& source_,
	HHuginn::scope_t const& loop_,
	int position_ )
	: HStatement()
	, _variableName( variableName_ )
	, _source( source_ )
	, _loop( loop_ )
	, _position( position_ ) {
	_loop->make_inline();
	return;
}

void HFor::do_execute( HThread* thread_ ) const {
	M_PROLOG
	thread_->create_loop_frame();
	HFrame* f( thread_->current_frame() );
	_source->execute( thread_ );
	if ( thread_->can_continue() ) {
		HHuginn::value_t source( f->result() );
		HHuginn::HIterable* coll( dynamic_cast<HHuginn::HIterable*>( source.raw() ) );
		if ( ! coll ) {
			throw HHuginn::HHuginnRuntimeException( "`For' source is not an iterable.", _source->position() );
		}
		HHuginn::HIterable::HIterator it( coll->iterator() );
		while ( thread_->can_continue() && it.is_valid() ) {
			HHuginn::value_t v( it.value() );
			f->set_variable( _variableName, v, _position );
			_loop->execute( thread_ );
			it.next();
		}
	}
	thread_->pop_frame();
	return;
	M_EPILOG
}

}

}

}

