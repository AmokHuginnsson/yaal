/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/filesystem.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/filesystem.hxx
 * \brief Declarations of file system handling functions.
 */

#ifndef YAAL_TOOLS_FILESYSTEM_HXX_INCLUDED
#define YAAL_TOOLS_FILESYSTEM_HXX_INCLUDED 1

#include "hcore/hpattern.hxx"

namespace yaal {

namespace tools {

namespace filesystem {

struct FILE_TYPE {
	typedef enum {
		REGULAR_FILE = 1,
		DIRECTORY = 2,
		ALL = REGULAR_FILE | DIRECTORY
	} enum_t;
};

typedef yaal::hcore::HArray<yaal::hcore::HString> find_result;

find_result find( yaal::hcore::HString const&, yaal::hcore::HPattern const&,
		int = 0, int = meta::max_signed<int>::value, FILE_TYPE::enum_t = FILE_TYPE::ALL );

}

}

}

#endif /* #ifndef YAAL_TOOLS_FILESYSTEM_HXX_INCLUDED */

