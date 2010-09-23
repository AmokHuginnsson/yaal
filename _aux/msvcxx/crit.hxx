#ifndef YAAL_MSVCXX_CRIT_HXX_INCLUDED
#define YAAL_MSVCXX_CRIT_HXX_INCLUDED 1

class CMutex
	{
	HANDLE _handle;
public:
	CMutex( void ) : _handle( ::CreateMutex( NULL, false, NULL ) ) {}
	~CMutex( void ) { ::CloseHandle( _handle ); }
	void Lock( void ) { ::WaitForSingleObject( _handle, INFINITE ); }
	void Unlock( void ) { ::ReleaseMutex( _handle ); }
	};

class CLock
	{
	CMutex& _mutex;
public:
	CLock( CMutex& mutex_ ) : _mutex( mutex_ )
		{
		_mutex.Lock();
		}
	~CLock( void )
		{
		_mutex.Unlock();
		}
	};

#endif /* #ifndef YAAL_MSVCXX_CRIT_HXX_INCLUDED */

