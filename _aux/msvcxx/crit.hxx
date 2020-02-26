#ifndef YAAL_MSVCXX_CRIT_HXX_INCLUDED
#define YAAL_MSVCXX_CRIT_HXX_INCLUDED 1

#include <WinSock2.h>
#include <windows.h>
#include "cleanup.hxx"

namespace msvcxx {

class Handle {
	HANDLE _handle;
public:
	Handle( void )
		: _handle( INVALID_HANDLE_VALUE ) {
	}
	Handle( HANDLE handle_ )
		: _handle( handle_ ) {
	}
	Handle( Handle&& other_ ) {
		_handle = other_._handle;
		other_._handle = INVALID_HANDLE_VALUE;
	}
	Handle& operator = ( Handle&& other_ ) {
		if ( _handle != INVALID_HANDLE_VALUE ) {
			::CloseHandle( _handle );
		}
		_handle = other_._handle;
		other_._handle = INVALID_HANDLE_VALUE;
		return ( *this );
	}
	~Handle( void ) {
		if ( _handle != INVALID_HANDLE_VALUE ) {
			::CloseHandle( _handle );
		}
	}
	HANDLE get( void ) const {
		return ( _handle );
	}
private:
	Handle( Handle const& ) = delete;
	Handle& operator = ( Handle const& ) = delete;
};

class CMutex {
	Handle _handle;
public:
	CMutex( void )
		: _handle( ::CreateMutex( nullptr, false, nullptr ) ) {
	}
	void Lock( void ) {
		::WaitForSingleObject( _handle.get(), INFINITE );
	}
	void Unlock( void ) {
		::ReleaseMutex( _handle.get() );
	}
private:
	CMutex( CMutex const& ) = delete;
	CMutex& operator = ( CMutex const& ) = delete;
};

class CLock {
	CMutex& _mutex;
public:
	CLock( CMutex& mutex_ )
		: _mutex( mutex_ ) {
		_mutex.Lock();
	}
	~CLock( void ) {
		_mutex.Unlock();
	}
private:
	CLock( CLock const& ) = delete;
	CLock operator = ( CLock const& ) = delete;
};

}

#endif /* #ifndef YAAL_MSVCXX_CRIT_HXX_INCLUDED */

