#ifndef YAAL_MSVCXX_CXXABI_H_INCLUDED
#define YAAL_MSVCXX_CXXABI_H_INCLUDED

namespace abi
{

char* __cxa_demangle( char const* const, int, int, int* );

}
int setenv( char const*, char const*, int const& );

#endif /* not YAAL_MSVCXX_CXXABI_H_INCLUDED */

