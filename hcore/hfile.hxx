/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hfile.hxx
 * \brief HFile class declaration.
 */

#ifndef YAAL_HCORE_HFILE_HXX_INCLUDED
#define YAAL_HCORE_HFILE_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hsynchronizedstream.hxx"
#include "hcore/hstrongenum.hxx"

namespace yaal {

namespace hcore {

/*! \brief File manipulation class.
 */
class HFile : public HStreamInterface {
public:
	typedef HFile this_type;
	typedef HStreamInterface base_type;
	/*! \brief Bits user for specyfying the way files are opened.
	 */
	struct OPEN {
		typedef enum {
			READING = 1,
			WRITING = 2,
			APPEND = 4,
			TRUNCATE = 8
		} enum_t;
	};
	typedef HStrongEnum<OPEN> open_t;
	/*! \brief Read operation modifiers.
	 */
	struct READ {
		typedef enum {
			DEFAULTS = 0, /* BUFFERED_READS */
			BUFFERED_READS = 1,
			UNBUFFERED_READS = 2
		} enum_t;
	};
	typedef HStrongEnum<READ> read_t;
	/*! \brief Kinds of seek operation.
	 */
	struct SEEK {
		typedef enum {
			SET,
			CUR,
			END
		} enum_t;
	};
	typedef HStrongEnum<SEEK> seek_t;
private:
	void* _handle;
	HString _path;
	HString _error;
	bool _owner;
public:
	/*! \brief Create closed file stream object.
	 */
	HFile( void );
	/*! \brief Create file stream based on existing low level stream (may be closed).
	 *
	 * \param raw - low level interface stream handler.
	 * \param owner - shall this file object take ownership of this low level stream.
	 */
	HFile( void* raw, bool owner );
	/*! \brief Create new file stream and open file item immediatelly.
	 *
	 * \param path - path to file item to be opened.
	 * \param mode - open mode.
	 */
	HFile( yaal::hcore::HString const& path, open_t const& mode );
	virtual ~HFile( void );
	int open( HString const&, open_t const& mode );
	/*! \brief Assign existing low level stream to this file stream (may be closed).
	 *
	 * \param raw - low level interface stream handler.
	 * \param owner - shall this file object take ownership of this low level stream.
	 */
	int open( void* raw, bool owner );
	int close( void );
	void* release( void );
	int long read_line( HString&, read_t const& = READ::DEFAULTS, int const = 0 );
	int long tell( void ) const;
	void seek( int long, seek_t const& = SEEK::SET );
	HString const& get_path( void ) const;
	HString const& get_error( void ) const;
	bool is_opened( void ) const;
	bool operator ! ( void ) const;
protected:
	virtual int do_open( HString const&, open_t const& mode );
	virtual int do_close( void );
	int get_file_descriptor( void ) const;
private:
	virtual int long do_write( void const* const, int long );
	virtual void do_flush( void );
	virtual int long do_read( void* const, int long );
	virtual bool do_is_valid( void ) const;
	int long get_line_length( void );
	HFile( HFile const& );
	HFile& operator = ( HFile const& );
};

typedef HExceptionT<HFile, HStreamInterfaceException> HFileException;

extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream cin;
extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream cout;
extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream cerr;
extern M_YAAL_HCORE_PUBLIC_API HSynchronizedStream clog;

}

}

#endif /* #ifndef YAAL_HCORE_HFILE_HXX_INCLUDED */

