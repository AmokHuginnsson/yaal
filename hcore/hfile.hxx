/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hfile.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hfile.hxx
 * \brief HFile class declaration.
 */

#ifndef YAAL_HCORE_HFILE_HXX_INCLUDED
#define YAAL_HCORE_HFILE_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief File manipulation class.
 */
class HFile : public HStreamInterface
	{
protected:
	typedef HFile self_t;
	typedef HStreamInterface hier_t;
public:
	/*! \brief Bits user for specyfying the way files are opened.
	 */
	struct OPEN
		{
		typedef enum
			{
			READING = 1,
			WRITING = 2,
			APPEND = 4,
			TRUNCATE = 8
			} enum_t;
		};
	typedef HStrongEnum<OPEN> open_t;
	/*! \brief Read operation modifiers.
	 */
	struct READ
		{
		typedef enum
			{
			DEFAULTS = 0, /* KEEP_NEWLINES | BUFFERED_READS */
			KEEP_NEWLINES = 1,
			STRIP_NEWLINES = 2,
			BUFFERED_READS = 4,
			UNBUFFERED_READS = 8
			} enum_t;
		};
	typedef HStrongEnum<READ> read_t;
	/*! \brief Kinds of seek operation.
	 */
	struct SEEK
		{
		typedef enum
			{
			SET,
			CUR,
			END
			} enum_t;
		};
	typedef HStrongEnum<SEEK> seek_t;
private:
	open_t f_eOpen;
	void* f_pvHandle;
	HString f_oPath;
	HString f_oError;
	bool f_bExternal;
public:
	/*! \brief Create closed (or based on low level interface) file stream.
	 *
	 * \param mode - open mode.
	 * \param raw - low level interface stream handler.
	 */
	HFile( open_t const& mode = OPEN::READING, void* const raw = NULL );
	/*! \brief Create new file stream and open file item immediatelly.
	 *
	 * \param path - path to file item to be opened.
	 * \param mode - open mode.
	 */
	HFile( yaal::hcore::HString const& path, open_t const& mode = OPEN::READING );
	virtual ~HFile( void );
	int open( HString const& );
	int close( void );
	int long read_line( HString&, read_t const& = READ::DEFAULTS, int const = 0 );
	int long tell( void ) const;
	void seek( int long const&, seek_t const& = SEEK::SET );
	HString const& get_path( void ) const;
	HString const& get_error( void ) const;
	virtual void flush( void ) const;
	bool operator ! ( void ) const;
private:
	virtual int long do_write( void const* const, int long const& );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long const& );
	virtual bool do_is_valid( void ) const;
	int long get_line_length( void );
	HFile( HFile const& );
	HFile& operator = ( HFile const& );
	};

typedef HExceptionT<HFile, HStreamInterfaceException> HFileException;

extern HFile cin;
extern HFile cout;
extern HFile cerr;

}

}

#endif /* not YAAL_HCORE_HFILE_HXX_INCLUDED */
