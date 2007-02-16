#ifndef __YAAL_EXTENSIONS_STD_COLOROSTREAM_H
#define __YAAL_EXTENSIONS_STD_COLOROSTREAM_H

#include <iostream>

namespace std
{

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& red( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[31m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& green( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[32m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& brown( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[33m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& blue( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[34m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& magenta( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[35m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& cyan( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[36m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& lightgray( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[0m\033[37m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& gray( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[40m\033[1m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& brightred( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[31m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& brightgreen( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[32m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& yellow( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[33m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& brightblue( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[34m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& brightmagenta( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[35m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& brightcyan( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[36m" );
	}

template<typename _CharT, typename _Traits>
basic_ostream<_CharT, _Traits>& white( basic_ostream<_CharT, _Traits>& __os )
	{
	return flush( __os << "\033[1m\033[37m" );
	}

}

#endif /* not __YAAL_EXTENSIONS_STD_COLOROSTREAM_H */

