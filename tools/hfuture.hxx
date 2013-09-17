/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hfuture.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hfuture.hxx
 * \brief Declaration and implementation of HFuture<> class.
 */

#ifndef YAAL_TOOLS_HFUTURE_HXX_INCLUDED
#define YAAL_TOOLS_HFUTURE_HXX_INCLUDED 1

#include "hcore/hthread.hxx"

namespace yaal {

namespace tools {

/*! \brief Request asynchronous execution of single operation.
 */
template<typename return_t>
class HFuture {
public:
	typedef HFuture<return_t> this_type;
private:
	template<typename basic_t>
	class ref_fwd {
		basic_t* _ref;
	public:
		ref_fwd( void ) : _ref( NULL ) {}
		operator basic_t& ( void ) const
			{ return ( *_ref ); }
		basic_t& operator = ( basic_t& v ) {
			_ref = &v;
			return ( *_ref );
		}
	};
	typedef yaal::hcore::HBoundCall<return_t ( void )> call_t;
	typedef typename trait::ternary<trait::is_reference<return_t>::value, ref_fwd<typename trait::strip_reference<return_t>::type>, return_t>::type return_type;
	call_t _call;
	yaal::hcore::HThread _thread;
	return_type _return;
	bool _finished;
public:
	HFuture( call_t const& call_ )
		: _call( call_ ), _thread(), _return(), _finished( false ) {
		M_PROLOG
		_thread.spawn( yaal::hcore::call( &this_type::execute, this ) );
		return;
		M_EPILOG
	}
	return_t get( void ) {
		M_PROLOG
		if ( ! _finished ) {
			_thread.join();
			_finished = true;
		}
		return ( _return );
		M_EPILOG
	}
private:
	void execute( void ) {
		M_PROLOG
		_return = _call();
		return;
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_TOOLS_HFUTURE_HXX_INCLUDED */

