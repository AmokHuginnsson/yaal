/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfsitem.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HFSITEM_HXX_INCLUDED
#define YAAL_TOOLS_HFSITEM_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/htime.hxx"

namespace yaal {

namespace tools {

/*! \brief File system object representation.
 */
class HFSItem {
	typedef HFSItem this_type;
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
	int long get_size( void ) const;
	int long size( void ) const;
	yaal::hcore::HString get_path( void ) const;
	yaal::hcore::HString get_name( void ) const;
	yaal::hcore::HTime modified( void ) const;
	yaal::hcore::HTime created( void ) const;
	yaal::hcore::HTime accessed( void ) const;
	yaal::hcore::HString get_user( void ) const;
	yaal::hcore::HString get_group( void ) const;
	int get_permissions( void ) const;
	int get_uid( void ) const;
	int get_gid( void ) const;
	HIterator begin( void );
	HIterator end( void );
	bool operator ! ( void ) const;
	void swap( HFSItem& );
	yaal::hcore::HString error_message( int ) const;
private:
	void do_stat( void* ) const;
	void set_path( yaal::hcore::HString const&, int );
	friend class HIterator;
};

typedef yaal::hcore::HExceptionT<HFSItem> HFSItemException;

/*! \brief File system iterator.
 */
class HFSItem::HIterator {
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

#endif /* #ifndef YAAL_TOOLS_HFSITEM_HXX_INCLUDED */

