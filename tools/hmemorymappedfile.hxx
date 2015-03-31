/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hmemorymappedfile.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hmemorymappedfile.hxx
 * \brief Declaration of HMemoryMappedFile class.
 */

#ifndef YAAL_TOOLS_HMEMORYMAPPEDFILE_HXX_INCLUDED
#define YAAL_TOOLS_HMEMORYMAPPEDFILE_HXX_INCLUDED 1

#include "hcore/trait.hxx"
#include "hcore/hstring.hxx"

namespace yaal {

namespace tools {

/*! \brief Implements Memory Mapped Files (MMF) resource wrapper.
 */
class HMemoryMappedFile : private yaal::trait::HNonCopyable {
private:
	int _fd;
	void* _map;
	int long _size;
public:
	/*! \brief Construct Memory Mapped File object.
	 * \param path - path to file to be mapped.
	 * \param size - requested mapping size.
	 *
	 * Default size of 0 means to use actual file size.
	 */
	HMemoryMappedFile( yaal::hcore::HString const& path, int long size = 0 );
	/*! \brief Destroy Memory Mapped File object and free all allocated resources.
	 */
	~HMemoryMappedFile( void );
	/*! \brief Get mapped region.
	 * \return Mapped region.
	 */
	void* map( void );
	/*! \brief Get read-only mapped region.
	 * \return Read-only mapped region.
	 */
	void const* map( void ) const;
	/*! \brief Get mapping size.
	 */
	int long size( void ) const;
	/*! \brief Get mapping size.
	 */
	int long get_size( void ) const;
private:
	HMemoryMappedFile( HMemoryMappedFile const& ) = delete;
	HMemoryMappedFile& operator = ( HMemoryMappedFile const& ) = delete;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HMEMORYMAPPEDFILE_HXX_INCLUDED */

