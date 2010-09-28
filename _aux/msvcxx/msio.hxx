#ifndef YAAL_MSVCXX_MSIO_H_INCLUDED
#define YAAL_MSVCXX_MSIO_H_INCLUDED 1

#include <memory>
#include <string>
#include <hash_map>

#include "crit.hxx"
#include "hcore/macro.hxx"

struct IO
	{
	struct TYPE
		{
		typedef enum
			{
			TERMINAL,
			PIPE,
			NAMED_PIPE,
			SOCKET
			} type_t;
		};
	TYPE::type_t _type;
	HANDLE _handle;
	OVERLAPPED _overlapped;
	char _buffer;
	bool _scheduled; /* io has been scheduled */
	bool _nonBlocking;
	std::string _path;
	IO( TYPE::type_t t_, HANDLE h_, HANDLE e_ = NULL, std::string const& p_ = std::string() )
		: _type( t_ ), _handle( h_ ), _overlapped(), _buffer( 0 ), _scheduled( false ), _nonBlocking( false ), _path( p_ )
		{
		_overlapped.hEvent = e_ ? e_ : ::CreateEvent( NULL, false, false, NULL );
		}
	~IO( void )
		{
		if ( _overlapped.hEvent )
			::CloseHandle( _overlapped.hEvent );
		}
private:
	IO( IO const& );
	IO& operator = ( IO const& );
	};

typedef std::tr1::shared_ptr<IO> io_ptr_t;

class M_YAAL_HCORE_PUBLIC_API SystemIO
	{
public:
	static int const MANAGED_IO = 0x10000;
private:
	typedef stdext::hash_map<int, io_ptr_t> io_table_t;
	io_table_t _ioTable;
	CMutex _mutex;
	int _idPool;
public:
	typedef io_table_t::value_type io_t;
	io_t& create_io( IO::TYPE::type_t, HANDLE, HANDLE = NULL, std::string const& = std::string() );
	io_t& get_io( int );
	void erase_io( int );
	static SystemIO& get_instance( void );
private:
	SystemIO( void );
	};

#endif /* #ifndef YAAL_MSVCXX_MSIO_H_INCLUDED */
