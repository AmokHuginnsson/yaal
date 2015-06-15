/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  thread.cxx - this file is integral part of `yaal' project.

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
#include "thread.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HThread::HThread( HHuginn* huginn_, yaal::hcore::HThread::id_t id_ )
	: _frames()
	, _id( id_ )
	, _huginn( huginn_ )
	, _exceptionMessage()
	, _exceptionPosition( 0 ) {
	return;
}

yaal::hcore::HThread::id_t HThread::id( void ) const {
	return ( _id );
}

HFrame* HThread::current_frame( void ) {
	M_PROLOG
	return ( ! _frames.is_empty() ? _frames.top().raw() : nullptr );
	M_EPILOG
}

HFrame const* HThread::current_frame( void ) const {
	M_PROLOG
	return ( ! _frames.is_empty() ? _frames.top().raw() : nullptr );
	M_EPILOG
}

void HThread::create_function_frame( HHuginn::HObject* object_, int upCast_ ) {
	M_PROLOG
	HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( this, parent, object_, upCast_, true, false ) );
	return;
	M_EPILOG
}

void HThread::create_loop_frame( void ) {
	M_PROLOG
	HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( this, parent, nullptr, 0, false, true ) );
	return;
	M_EPILOG
}

void HThread::create_scope_frame( void ) {
	M_PROLOG
	HFrame* parent( current_frame() );
	_frames.push( make_pointer<HFrame>( this, parent, nullptr, 0, false, false ) );
	return;
	M_EPILOG
}

void HThread::pop_frame( void ) {
	M_PROLOG
	_frames.pop();
	return;
	M_EPILOG
}

bool HThread::can_continue( void ) const {
	M_PROLOG
	M_ASSERT( current_frame() );
	return ( current_frame()->can_continue() );
	M_EPILOG
}

void HThread::break_execution( HFrame::STATE state_, HHuginn::value_t const& value_, int level_ ) {
	M_PROLOG
	M_ASSERT(
		( state_ == HFrame::STATE::RETURN )
		|| ( state_ == HFrame::STATE::BREAK )
		|| ( state_ == HFrame::STATE::CONTINUE )
		|| ( state_ == HFrame::STATE::RUNTIME_EXCEPTION )
	);
	int level( 0 );
	HFrame* f( current_frame() );
	HFrame* target( f );
	int no( f->number() );
	while ( f ) {
		target = f;
		if ( f->is_loop() ) {
			++ level;
		}
		f->break_execution( state_ );
		f = f->parent();
		if ( ! f ) {
			break;
		} else if ( ( state_ == HFrame::STATE::RETURN ) && ( f->number() != no ) ) {
			break;
		} else if ( ( state_ == HFrame::STATE::BREAK ) && ( level > level_ ) ) {
			break;
		} else if ( ( state_ == HFrame::STATE::CONTINUE ) && ( level > 0 ) ) {
			break;
		}
	}
	if ( target ) {
		target->set_result( value_ );
	}
	return;
	M_EPILOG
}

void HThread::set_exception( yaal::hcore::HString const& message_, int position_ ) {
	M_PROLOG
	_exceptionMessage = message_;
	_exceptionPosition = position_;
	return;
	M_EPILOG
}

bool HThread::has_exception( void ) const {
	return ( ! _exceptionMessage.is_empty() || ( _exceptionPosition != 0 ) );
}

void HThread::flush_exception( void ) {
	M_PROLOG
	if ( has_exception() ) {
		int position( _exceptionPosition );
		_exceptionPosition = 0;
		HString message( yaal::move( _exceptionMessage ) );
		throw HHuginn::HHuginnRuntimeException( message, position );
	}
	M_EPILOG
}

HHuginn& HThread::huginn( void ) {
	return ( *_huginn );
}


}

}

}

