/* Read yaal/LICENSE.md file for copyright and licensing information. */
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

