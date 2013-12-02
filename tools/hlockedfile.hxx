/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hlockedfile.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hlockedfile.hxx
 * \brief Declaration of HLockedFile class.
 */

#ifndef YAAL_TOOLS_HLOCKEDFILE_HXX_INCLUDED
#define YAAL_TOOLS_HLOCKEDFILE_HXX_INCLUDED 1

#include "hcore/hfile.hxx"

namespace yaal {

namespace tools {

class HLockedFile : public yaal::hcore::HFile {
public:
	typedef HLockedFile this_type;
	typedef yaal::hcore::HFile base_type;
	/*! \brief Create closed file stream object.
	 */
	HLockedFile( void );
	/*! \brief Create new file stream and open file item immediatelly.
	 *
	 * \param path - path to file item to be opened.
	 * \param mode - open mode.
	 */
	HLockedFile( yaal::hcore::HString const& path, open_t const& mode );
	virtual ~HLockedFile( void );
protected:
	virtual int do_open( yaal::hcore::HString const&, open_t const& mode );
	virtual int do_close( void );
private:
	void lock( void );
	void unlock( void );
};

typedef yaal::hcore::HExceptionT<HLockedFile, yaal::hcore::HFileException> HLockedFileException;

}

}

#endif /* #ifndef YAAL_TOOLS_HLOCKEDFILE_HXX_INCLUDED */

