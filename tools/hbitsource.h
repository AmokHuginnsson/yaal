/*
---           hanalyser.h 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hbitsource.h - this file is integral part of hanalyser.h project.

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

#ifndef __YAAL_TOOLS_HBITSOURCE_H
#define __YAAL_TOOLS_HBITSOURCE_H

#include "hcore/base.h"
#include "hcore/hstring.h"
#include "hcore/hfile.h"
#include "hcore/hchunk.h"
#include "tools/hbitmap.h"

namespace yaal
{

namespace tools
{

class HBitSourceInterface
	{
public:
	virtual ~HBitSourceInterface( void ){}
	HBitmap const& get_nth_block( int long const&, int long const& ) const;
private:
	virtual HBitmap const& do_get_nth_block( int long const&, int long const& ) const = 0;
	};

class HBitSourceFile : public HBitSourceInterface, private yaal::HNonCopyable
	{
	typedef HBitSourceFile self_t;
	yaal::hcore::HString f_oPath;
	mutable yaal::hcore::HFile f_oFile;
	mutable yaal::hcore::HChunk f_oBuffer;
	mutable int long f_lBufferOffset;
	mutable int long f_lBufferSize;
	mutable HBitmap f_oBitmap;
public:
	HBitSourceFile( yaal::hcore::HString const& );
	virtual ~HBitSourceFile( void );
private:
	virtual HBitmap const& do_get_nth_block( int long const&, int long const& ) const;
	};

/*! \brief Memory based data source.
 */
class HBitSourceMemory : public HBitSourceInterface, private yaal::HNonCopyable
	{
	typedef HBitSourceMemory self_t;
	void const* f_pvMemory;
	int long f_lSIze;
	mutable HBitmap f_oBitmap;
public:
	HBitSourceMemory( void const*, int long );
	virtual ~HBitSourceMemory( void );
private:
	virtual HBitmap const& do_get_nth_block( int long const&, int long const& ) const;
	HBitSourceMemory( HBitSourceMemory const& );
	HBitSourceMemory& operator = ( HBitSourceMemory const& );
	};

typedef yaal::hcore::HExceptionT<HBitSourceFile> HBitSourceFileException;
typedef yaal::hcore::HExceptionT<HBitSourceMemory> HBitSourceMemoryException;

}

}

#endif /* not __YAAL_TOOLS_HBITSOURCE_H */

