/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hfuture.hxx
 * \brief Declaration and implementation of HFuture<> class.
 */

#ifndef YAAL_TOOLS_HFUTURE_HXX_INCLUDED
#define YAAL_TOOLS_HFUTURE_HXX_INCLUDED 1

#include "tools/hthreadpool.hxx"

namespace yaal {

namespace tools {

/*! \brief Request asynchronous execution of single operation.
 */
template<typename return_t>
class HFuture final {
public:
	typedef HFuture<return_t> this_type;
private:
	template<typename basic_t>
	class ref_fwd {
		basic_t* _ref;
	public:
		ref_fwd( void ) : _ref( nullptr ) {}
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
	yaal::hcore::HMutex _mutex;
	yaal::hcore::HCondition _condVar;
	return_type _return;
	bool _finished;
public:
	HFuture( call_t const& call_, HWorkFlow::SCHEDULE_POLICY schedulePolicy_ = HWorkFlow::SCHEDULE_POLICY::LAZY )
		: _call( call_ )
		, _mutex()
		, _condVar( _mutex )
		, _return()
		, _finished( false ) {
		M_PROLOG
		HThreadPool::get_instance().schedule_task( schedulePolicy_, yaal::hcore::call( &this_type::execute, this ) );
		return;
		M_EPILOG
	}
	return_t get( void ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		while ( ! _finished ) {
			_condVar.wait_for( /* ever */ );
		}
		return ( _return );
		M_EPILOG
	}
	bool is_ready( void ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex, yaal::hcore::HLock::TYPE::TENTATIVE );
		return ( l.owns_lock() ? _finished : false );
		M_EPILOG
	}
private:
	void execute( void ) {
		M_PROLOG
		yaal::hcore::HLock l( _mutex );
		_return = _call();
		_finished = true;
		_condVar.broadcast();
		return;
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_TOOLS_HFUTURE_HXX_INCLUDED */

