/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
	/*! \brief Create new file stream and open file item immediately.
	 *
	 * \param path - path to file item to be opened.
	 * \param mode - open mode.
	 */
	HLockedFile( yaal::hcore::HString const& path, open_t mode );
	virtual ~HLockedFile( void );
protected:
	virtual int do_open( yaal::hcore::HString const&, open_t mode ) override;
	virtual int do_close( void ) override;
private:
	void lock( void );
	void unlock( void );
};

typedef yaal::hcore::HExceptionT<HLockedFile, yaal::hcore::HFileException> HLockedFileException;

}

}

#endif /* #ifndef YAAL_TOOLS_HLOCKEDFILE_HXX_INCLUDED */

