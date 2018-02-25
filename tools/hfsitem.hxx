/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HFSITEM_HXX_INCLUDED
#define YAAL_TOOLS_HFSITEM_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/system.hxx"
#include "hcore/htime.hxx"

namespace yaal {

namespace tools {

/*! \brief File system object representation.
 */
class HFSItem final {
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
	u64_t id( void ) const;
	int get_permissions( void ) const;
	yaal::hcore::system::user_id_t get_uid( void ) const;
	yaal::hcore::system::user_id_t get_gid( void ) const;
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
	int _pos;
	yaal::u64_t _inode;
	yaal::hcore::HString _name;
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

inline void swap( yaal::tools::HFSItem& a, yaal::tools::HFSItem& b )
	{ a.swap( b ); }

inline void swap( yaal::tools::HFSItem::HIterator& a, yaal::tools::HFSItem::HIterator& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_TOOLS_HFSITEM_HXX_INCLUDED */

