/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipedchild.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_PIPEDCHILD_HXX_INCLUDED
#define YAAL_TOOLS_PIPEDCHILD_HXX_INCLUDED

#include "hcore/harray.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Run process and access its std(in/out/err).
 */
class HPipedChild : public yaal::hcore::HStreamInterface
	{
protected:
	typedef HPipedChild self_t;
	typedef HStreamInterface hier_t;
public:
	/*! \brief Child process output stream types.
	 */
	struct STREAM
		{
		/*! \brief Output type flags.
		 */
		typedef enum
			{
			OUT, /*!< Output stream is of interest. */
			ERR /*!< Error stream is of interest. */
			} stream_t;
		};
	/*! \brief Status of this child process.
	 */
	struct STATUS
		{
		/*! \brief Status flags.
		 */
		struct TYPE
			{
			/*! \brief Status flags.
			 */
			typedef enum
				{
				NORMAL, /*!< process is running. */
				ABORT, /*!< process has been stopped. */
				NOT_SPAWNED /*!< process has not been spawned yet. */
				} type_t;
			};
		TYPE::type_t type; /*!< child process current status. */
		int value; /*!< exit value of finished child process. */
		STATUS( void ) : type( TYPE::NOT_SPAWNED ), value( 0 ) {}
		};
	typedef yaal::hcore::HArray<yaal::hcore::HString> argv_t;
private:
	int f_iPid;
	int f_iPipeIn;
	int f_iPipeOut;
	int f_iPipeErr;
	STREAM::stream_t f_eCSOI; /* Current Stream Of Interest */
	yaal::hcore::HStreamInterface::cache_t f_oSecondLineCache;
	int f_iSecondLineOffset;
public:
	HPipedChild( void );
	~HPipedChild( void );
	void spawn( yaal::hcore::HString const&, argv_t const& = argv_t() );
	bool read_poll( void* );
	STATUS finish( void );
	bool is_running( void );
	void set_csoi( STREAM::stream_t const& );
private:
	virtual int long do_write( void const* const, int long const& );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long const& );
	HPipedChild( HPipedChild const& );
	HPipedChild& operator = ( HPipedChild const& );
	};

typedef yaal::hcore::HExceptionT<HPipedChild,yaal::hcore::HStreamInterfaceException> HPipedChildException;

}

}

#endif /* not YAAL_TOOLS_PIPEDCHILD_HXX_INCLUDED */

