#ifndef YAAL_MSVCXX_CRIT_HXX_INCLUDED
#define YAAL_MSVCXX_CRIT_HXX_INCLUDED 1

#include <WinSock2.h>
#include <windows.h>

class CMutex {
	HANDLE _handle;
public:
	CMutex( void ) : _handle( ::CreateMutex( nullptr, false, nullptr ) ) {}
	~CMutex( void ) { ::CloseHandle( _handle ); }
	void Lock( void ) { ::WaitForSingleObject( _handle, INFINITE ); }
	void Unlock( void ) { ::ReleaseMutex( _handle ); }
private:
	CMutex( CMutex const& ) = delete;
	CMutex& operator = ( CMutex const& ) = delete;
};

class CLock {
	CMutex& _mutex;
public:
	CLock( CMutex& mutex_ ) : _mutex( mutex_ ) {
		_mutex.Lock();
	}
	~CLock( void ) {
		_mutex.Unlock();
	}
private:
	CLock( CLock const& ) = delete;
	CLock operator = ( CLock const& ) = delete;
};

#endif /* #ifndef YAAL_MSVCXX_CRIT_HXX_INCLUDED */

