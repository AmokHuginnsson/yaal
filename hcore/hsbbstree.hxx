/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsbbstree.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hsbbstree.hxx
 * \brief Declaration of HSBBSTree related classes.
 */

#ifndef YAAL_HCORE_HSBBSTREE_HXX_INCLUDED
#define YAAL_HCORE_HSBBSTREE_HXX_INCLUDED

#line 31

#define D_VCSID_HSBBSTREE_H "$Id$"

#include "hcore/hexception.hxx"
#include "hcore/hpair.hxx"

namespace yaal
{

namespace hcore
{

extern char const * const n_ppcErrMsgHSBBSTree [ ];

/*! \brief HSBBSTree stands for: Self-balancing binary search tree.
 *
 * HSBBSTree is implementation of self balancing binary search tree data structure.
 * Implementation of core engine for HMap<> and HSet<> classes.
 * HSBBSTree is not ment to be used directly.
 */
class HSBBSTree
	{
	typedef HSBBSTree self_t;
public:
	/*! \brief HSBBSTree operation error types.
	 */
	struct ERROR
		{
		typedef enum
			{
			E_OK = 0,
			E_NON_EXISTING_KEY,
			E_NIL_ITERATOR
			} error_t;
		};
	class HIterator;
private:
	class HAbstractNode;
	struct ONodePtr
		{
		HAbstractNode* f_poNode;
		bool f_bExists;
		ONodePtr( void ) : f_poNode( NULL ), f_bExists( false ) { }
		ONodePtr( ONodePtr const& np ) : f_poNode( np.f_poNode ), f_bExists( np.f_bExists ) {}
		ONodePtr& operator = ( ONodePtr const& np )
			{
			if ( &np != this )
				{
				f_poNode = np.f_poNode;
				f_bExists = np.f_bExists;
				}
			return ( *this );
			}
		};
	template<typename tType>
	class HNode;
	class HAbstractNode
		{
		typedef enum
			{
			D_RED,
			D_BLACK
			} color_t;
		color_t f_eColor;
		HAbstractNode* f_poParent;
		HAbstractNode* f_poLeft;
		HAbstractNode* f_poRight;
		HAbstractNode( void );
		virtual ~HAbstractNode( void );
		HAbstractNode( HAbstractNode const& );
		HAbstractNode& operator = ( HAbstractNode const & );
		void set_child( HAbstractNode*, HAbstractNode* );
		friend class HSBBSTree;
		friend class HSBBSTree::HIterator;
		template<typename any_t>
		friend class HNode;
		};
	HAbstractNode* f_poRoot;
	int long f_lSize;
public:
	HSBBSTree( void );
	virtual ~HSBBSTree( void );
	template<typename tType, typename ttType>
	HPair<HIterator, bool> insert( tType const& );
	template<typename tType, typename ttType, typename tttType>
	void remove( ttType const& );
	void remove( HIterator const& );
	template<typename tType, typename ttType, typename tttType>
	HIterator find( ttType const& ) const;
	int long size( void ) const;
	bool empty( void ) const;
	bool is_empty( void ) const;
	void clear( void );
	void swap( HSBBSTree& );
	template<typename tType, typename ttType>
	void copy_from( HSBBSTree const& );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HIterator rbegin( void ) const;
	HIterator rend( void ) const;
private:
	template<typename tType, typename ttType, typename tttType>
	ONodePtr find_node( ttType const& ) const;
	void remove_node( HAbstractNode* );
	void swap( HAbstractNode*, HAbstractNode* );
	HAbstractNode* sibling( HAbstractNode* ) const;
	void insert_rebalance( HAbstractNode* );
	void remove_rebalance( HAbstractNode* );
	void rotate_left( HAbstractNode* );
	void rotate_right( HAbstractNode* );
	HSBBSTree( HSBBSTree const& );
	HSBBSTree& operator = ( HSBBSTree const& );
	};

/*! \brief Iterator for HSBBSTree data structure.
 */
class HSBBSTree::HIterator
	{
	HAbstractNode* f_poCurrent;
public:
	HIterator( void );
	HIterator( HIterator const& );
	HIterator& operator ++ ( void );
	HIterator const operator ++ ( int );
	HIterator& operator -- ( void );
	HIterator const operator -- ( int );
	HIterator& operator = ( HIterator const& );
	bool operator == ( HIterator const& ) const;
	bool operator != ( HIterator const& ) const;
	template<typename tType>
	tType& operator* ( void );
	template<typename tType>
	tType const& operator* ( void ) const;
private:
	friend class HSBBSTree;
	explicit HIterator( HAbstractNode* const );
	};

/*! \brief True HSBBSTree contents.
 */
template<typename tType>
class	HSBBSTree::HNode : public HAbstractNode
	{
	tType f_tKey;
	HNode( tType const& );
	HNode( HNode const& );
	HNode& operator = ( HNode const& );
	friend class HSBBSTree;
	friend tType& HSBBSTree::HIterator::operator*<tType>( void );
	friend tType const& HSBBSTree::HIterator::operator*<tType>( void ) const;
	};

template<typename tType>
tType& HSBBSTree::HIterator::operator* ( void )
	{
	M_PROLOG
	M_ASSERT( f_poCurrent );
	return ( static_cast<HNode<tType>*>( f_poCurrent )->f_tKey );
	M_EPILOG
	}

template<typename tType>
tType const& HSBBSTree::HIterator::operator* ( void ) const
	{
	M_PROLOG
	M_ASSERT( f_poCurrent );
	return ( static_cast<HNode<tType>*>( f_poCurrent )->f_tKey );
	M_EPILOG
	}

template<typename tType>
HSBBSTree::HNode<tType>::HNode( tType const& a_tKey )
	: HAbstractNode(), f_tKey( a_tKey )
	{
	return;
	}

template<typename tType, typename ttType>
HPair<HSBBSTree::HIterator, bool> HSBBSTree::insert( tType const& a_tKey )
	{
	M_PROLOG
	ONodePtr l_oNode;
	HNode<tType>* l_poNode = NULL;
	if ( f_poRoot )
		l_oNode = find_node<tType, tType, ttType>( a_tKey );
	if ( l_oNode.f_bExists )
		l_poNode = static_cast<HNode<tType>*>( l_oNode.f_poNode );
	else
		{
		l_poNode = new HNode<tType>( a_tKey );
		f_lSize ++;
		if ( f_poRoot )
			{
			l_poNode->f_poParent = l_oNode.f_poNode;
			if ( ttType::less( a_tKey, static_cast<HNode<tType>*>( l_oNode.f_poNode )->f_tKey ) )
				{
				M_ASSERT( ! l_oNode.f_poNode->f_poLeft );
				l_oNode.f_poNode->f_poLeft = l_poNode;
				}
			else
				{
				M_ASSERT( ! l_oNode.f_poNode->f_poRight );
				l_oNode.f_poNode->f_poRight = l_poNode;
				}
			insert_rebalance( l_poNode );
			}
		else
			{
			f_poRoot = l_poNode;
			f_poRoot->f_eColor = HAbstractNode::D_BLACK;
			}
		}
	M_ASSERT( ( ! f_poRoot ) || ( f_poRoot->f_poParent == NULL ) );
	M_ASSERT( ( ! f_poRoot ) || ( f_poRoot->f_eColor == HAbstractNode::D_BLACK ) );
	return ( make_pair( HIterator( l_poNode ), ! l_oNode.f_bExists ) );
	M_EPILOG
	}

template<typename tType, typename ttType, typename tttType>
void HSBBSTree::remove( ttType const& a_tKey )
	{
	M_PROLOG
	if ( f_poRoot )
		{
		ONodePtr l_oNode = find_node<tType, ttType, tttType>( a_tKey );
		if ( l_oNode.f_bExists )
			{
			remove_node( l_oNode.f_poNode );
			return;
			}
		}
	M_THROW( n_ppcErrMsgHSBBSTree [ ERROR::E_NON_EXISTING_KEY ],
			static_cast<int>( ERROR::E_NON_EXISTING_KEY ) );
	M_EPILOG
	}

template<typename tType, typename ttType, typename tttType>
HSBBSTree::HIterator HSBBSTree::find( ttType const& a_tKey ) const
	{
	M_PROLOG
	ONodePtr l_oNodePtr = find_node<tType, ttType, tttType>( a_tKey );
	return ( HIterator( l_oNodePtr.f_bExists ? l_oNodePtr.f_poNode : NULL ) );
	M_EPILOG
	}

template<typename tType, typename ttType, typename tttType>
typename HSBBSTree::ONodePtr HSBBSTree::find_node( ttType const& a_tKey ) const
	{
	M_PROLOG
	ONodePtr l_oNodePtr;
	if ( f_poRoot )
		{
		l_oNodePtr.f_poNode = f_poRoot;
		while ( ! l_oNodePtr.f_bExists )
			{
			if ( tttType::less( a_tKey, static_cast<HNode<tType>*>( l_oNodePtr.f_poNode )->f_tKey ) )
				{
				if ( l_oNodePtr.f_poNode->f_poLeft )
					l_oNodePtr.f_poNode = l_oNodePtr.f_poNode->f_poLeft;
				else
					break;
				}
			else if ( tttType::less( static_cast<HNode<tType>*>( l_oNodePtr.f_poNode )->f_tKey, a_tKey ) )
				{
				if ( l_oNodePtr.f_poNode->f_poRight )
					l_oNodePtr.f_poNode = l_oNodePtr.f_poNode->f_poRight;
				else
					break;
				}
			else
				l_oNodePtr.f_bExists = true;
			}
		}
	return ( l_oNodePtr );
	M_EPILOG
	}

template<typename tType, typename ttType>
void HSBBSTree::copy_from( HSBBSTree const& source )
	{
	M_PROLOG
	if ( &source != this )
		{
		clear();
		for ( HIterator it = source.begin(); it != source.end(); ++ it )
			insert<tType, ttType>( it.operator*<tType>() );
		}
	return;
	M_EPILOG
	}

}

inline void swap( yaal::hcore::HSBBSTree& a, yaal::hcore::HSBBSTree& b )
	{ a.swap( b );	}

}

#endif /* not YAAL_HCORE_HSBBSTREE_HXX_INCLUDED */

