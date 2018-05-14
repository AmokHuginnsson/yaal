/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"


namespace yaal {

namespace tools {

namespace huginn {

class HThread;

class HIteratorInterface {
public:
	virtual ~HIteratorInterface( void ) {
		return;
	}
	HHuginn::value_t value( HThread* thread_, int position_ ) {
		return ( do_value( thread_, position_ ) );
	}
	bool is_valid( HThread* thread_, int position_ ) {
		return ( do_is_valid( thread_, position_ ) );
	}
	void next( HThread* thread_, int position_ ) {
		do_next( thread_, position_ );
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) = 0;
	virtual bool do_is_valid( HThread*, int ) = 0;
	virtual void do_next( HThread*, int ) = 0;
};

class HNotifableIterator : public HIteratorInterface {
private:
	HHuginn::HInvalidatingIterable* _owner;
protected:
	int _skip;
public:
	HNotifableIterator( HHuginn::HInvalidatingIterable* owner_ )
		: _owner( owner_ )
		, _skip( 0 ) {
		_owner->notify( this );
	}
	virtual ~HNotifableIterator( void ) {
		_owner->forget( this );
	}
	void invalidate( void ) {
		do_invalidate();
	}
	void skip( void ) {
		do_skip();
	}
	void const* node_id( void ) const {
		return ( do_node_id() );
	}
protected:
	virtual void do_invalidate( void ) = 0;
	virtual void do_skip( void ) = 0;
	virtual void const* do_node_id( void ) const = 0;
private:
	HNotifableIterator( HNotifableIterator const& ) = delete;
	HNotifableIterator& operator = ( HNotifableIterator const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED */

