/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipedchild.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_TOOLS_PIPEDCHILD_H
#define __YAAL_TOOLS_PIPEDCHILD_H

#include "hcore/hstreaminterface.h"

namespace yaal
{

namespace tools
{

class HPipedChild : public yaal::hcore::HStreamInterface
	{
protected:
	typedef HPipedChild self_t;
	typedef HStreamInterface hier_t;
public:
	struct STREAM
		{
		typedef enum
			{
			D_OUT,
			D_ERR
			} stream_t;
		};
	struct STATUS
		{
		struct TYPE
			{
			typedef enum
				{
				D_NORMAL,
				D_ABORT,
				D_NOT_SPAWNED
				} type_t;
			};
		TYPE::type_t type;
		int value;
		STATUS( void ) : type( TYPE::D_NOT_SPAWNED ), value( 0 ) {}
		};
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
	void spawn( yaal::hcore::HString const&, char* const[] = NULL );
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

}

}

#endif /* not __YAAL_TOOLS_PIPEDCHILD_H */

