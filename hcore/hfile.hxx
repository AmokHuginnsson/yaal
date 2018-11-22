/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hfile.hxx
 * \brief HFile class declaration.
 */

#ifndef YAAL_HCORE_HFILE_HXX_INCLUDED
#define YAAL_HCORE_HFILE_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hsynchronizedstream.hxx"
#include "hcore/hbitflag.hxx"

namespace yaal {

namespace hcore {

/*! \brief File manipulation class.
 */
class HFile : public HStreamInterface {
public:
	typedef HFile this_type;
	typedef HStreamInterface base_type;
	struct OPEN;
	/*! \brief Bits used for specifying the way files are opened.
	 */
	typedef HBitFlag<OPEN> open_t;
	struct OPEN {
		static M_YAAL_HCORE_PUBLIC_API open_t const READING;
		static M_YAAL_HCORE_PUBLIC_API open_t const WRITING;
		static M_YAAL_HCORE_PUBLIC_API open_t const APPEND;
		static M_YAAL_HCORE_PUBLIC_API open_t const TRUNCATE;
	};
	/*! \brief Ownership.
	 */
	enum class OWNERSHIP {
		NONE,
		ACQUIRED,
		EXTERNAL
	};
	/*! \brief Read operation modifiers.
	 */
	enum class READ {
		DEFAULTS, /* BUFFERED_READS */
		BUFFERED_READS,
		UNBUFFERED_READS
	};
private:
	void* _handle;
	HString _path;
	HString _error;
	OWNERSHIP _ownership;
public:
	/*! \brief Create closed file stream object.
	 */
	HFile( void );
	/*! \brief Create file stream based on existing low level stream (may be closed).
	 *
	 * \param raw - low level interface stream handler.
	 * \param ownership - shall this file object take ownership of this low level stream.
	 */
	HFile( void* raw, OWNERSHIP ownership );
	/*! \brief Create new file stream and open file item immediately.
	 *
	 * \param path - path to file item to be opened.
	 * \param mode - open mode.
	 */
	HFile( yaal::hcore::HString const& path, open_t mode );
	virtual ~HFile( void );
	int open( HString const&, open_t mode );
	/*! \brief Assign existing low level stream to this file stream (may be closed).
	 *
	 * \param raw - low level interface stream handler.
	 * \param ownership - shall this file object take ownership of this low level stream.
	 */
	int open( void* raw, OWNERSHIP ownership );
	int close( void );
	void* release( void );
	HFile& read_line( HString&, READ = READ::DEFAULTS, int = 0 );
	int long tell( void ) const;
	HString const& get_path( void ) const;
	HString const& get_error( void ) const;
	bool is_opened( void ) const;
	explicit operator bool ( void ) const;
protected:
	virtual int do_open( HString const&, open_t mode );
	virtual int do_close( void );
	int get_file_descriptor( void ) const;
private:
	virtual int long do_write( void const*, int long ) override;
	virtual void do_flush( void ) override;
	virtual int long do_read( void*, int long ) override;
	virtual void do_seek( int long, SEEK ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
	HFile( HFile const& ) = delete;
	HFile& operator = ( HFile const& ) = delete;
};

typedef HExceptionT<HFile, HStreamInterfaceException> HFileException;

extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream cin;
extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream cout;
extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream cerr;
extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream clog;

}

}

#endif /* #ifndef YAAL_HCORE_HFILE_HXX_INCLUDED */

