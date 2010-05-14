/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfsitem.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HFSITEM_HXX_INCLUDED
#define YAAL_TOOLS_HFSITEM_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpointer.hxx"

namespace yaal
{

namespace tools
{

/*! \brief File system object representation.
 */
class HFSItem
	{
	typedef HFSItem self_t;
	int _nameLen;
	yaal::hcore::HString _path;
public:
	typedef yaal::hcore::HPointer<HFSItem> ptr_t;
	class HIterator;
	explicit HFSItem( yaal::hcore::HString const& );
	~HFSItem( void );
	bool is_directory( void ) const;
	bool is_file( void ) const;
	bool is_executable( void ) const;
	yaal::hcore::HString get_path() const;
	yaal::hcore::HString get_name() const;
	HIterator begin( void );
	HIterator end( void );
	bool operator ! ( void ) const;
	void swap( HFSItem& );
private:
	void do_stat( void* ) const;
	void set_path( yaal::hcore::HString const&, int );
	friend class HIterator;
	};

typedef yaal::hcore::HExceptionT<HFSItem> HFSItemException;

/*! \brief File system iterator.
 */
class HFSItem::HIterator
	{
	yaal::hcore::HString _path;
	void* _dir;
	yaal::hcore::HChunk::ptr_t _dirEnt;
	HFSItem _item;
public:
	HIterator( HIterator const& it_ );
	~HIterator( void );
	HIterator& operator= ( HIterator const& it_ );
	HIterator& operator ++ ( void );
	HIterator const operator ++ ( int );
	HFSItem const& operator* ( void );
	HFSItem const* operator->( void );
	bool operator == ( HIterator const& it ) const;
	bool operator != ( HIterator const& it ) const;
	void swap( HIterator& );
private:
	void update( void );
	void cleanup( void );
	friend class HFSItem;
	explicit HIterator( yaal::hcore::HString const& it );
	};

}

inline void swap( yaal::tools::HFSItem& a, yaal::tools::HFSItem& b )
	{ a.swap( b ); }
inline void swap( yaal::tools::HFSItem::HIterator& a, yaal::tools::HFSItem::HIterator& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_TOOLS_HFSITEM_HXX_INCLUDED */

