#ifndef YAAL_MSVCXX_EMU_PWD_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_PWD_HXX_INCLUDED 1

#include <utility>
#include <string>

namespace msvcxx
{

typedef std::pair<int, int> owner_t;
owner_t get_path_owner( std::string const& );

}

#endif /* #ifndef YAAL_MSVCXX_EMU_PWD_HXX_INCLUDED */