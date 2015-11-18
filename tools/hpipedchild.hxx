/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hpipedchild.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_PIPEDCHILD_HXX_INCLUDED
#define YAAL_TOOLS_PIPEDCHILD_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Run process and access its std(in/out/err).
 */
class HPipedChild : public yaal::hcore::HStreamInterface {
public:
	typedef HPipedChild this_type;
	typedef HStreamInterface base_type;
	/*! \brief Child process output stream types.
	 */
	enum class STREAM {
		OUT, /*!< Output stream is of interest. */
		ERR /*!< Error stream is of interest. */
	};
	/*! \brief Status of this child process.
	 */
	struct STATUS {
		/*! \brief Status flags.
		 */
		enum class TYPE {
			NORMAL, /*!< process is running. */
			ABORT, /*!< process has been stopped. */
			NOT_SPAWNED /*!< process has not been spawned yet. */
		};
		TYPE type; /*!< child process current status. */
		int value; /*!< exit value of finished child process. */
		STATUS( void ) : type( TYPE::NOT_SPAWNED ), value( 0 ) {}
	};
	typedef yaal::hcore::HArray<yaal::hcore::HString> argv_t;
private:
	int _pid;
	int _pipeIn;
	int _pipeOut;
	int _pipeErr;
	STREAM _cSOI; /* Current Stream Of Interest */
	yaal::hcore::HChunk _secondLineCache;
	int _secondLineOffset;
public:
	M_YAAL_TOOLS_PUBLIC_API static int _killGracePeriod;
	HPipedChild( void );
	~HPipedChild( void );
	void spawn( yaal::hcore::HString const&, argv_t const& = argv_t() );
	bool read_poll( int long* );
	STATUS finish( void );
	bool is_running( void ) const;
	void set_csoi( STREAM );
private:
	virtual int long do_write( void const* const, int long ) override;
	virtual void do_flush( void ) override;
	virtual int long do_read( void* const, int long ) override;
	virtual bool do_is_valid( void ) const override;
	HPipedChild( HPipedChild const& ) = delete;
	HPipedChild& operator = ( HPipedChild const& ) = delete;
};

typedef yaal::hcore::HExceptionT<HPipedChild, yaal::hcore::HStreamInterfaceException> HPipedChildException;

}

}

#endif /* #ifndef YAAL_TOOLS_PIPEDCHILD_HXX_INCLUDED */

