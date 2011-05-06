/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hmemorymappedfile.hxx - this file is integral part of yaal project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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

namespace yaal
{

namespace tools
{

/*! \brief Implements Memory Mapped Files (MMF) resource wrapper.
 */
class HMemoryMappedFile : private yaal::trait::HNonCopyable
	{
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
	HMemoryMappedFile( HMemoryMappedFile const& );
	HMemoryMappedFile& operator = ( HMemoryMappedFile const& );
	};

}

}

#endif /* #ifndef YAAL_TOOLS_HMEMORYMAPPEDFILE_HXX_INCLUDED */

