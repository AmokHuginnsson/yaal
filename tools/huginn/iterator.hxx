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

class HIterator : public HHuginn::HIterable, public HIteratorInterface {
	HHuginn::value_t _source;
	HHuginn::HIterable::iterator_t _impl;
public:
	HIterator( HHuginn::HClass const*, HHuginn::value_t const&, HHuginn::HIterable::iterator_t const& );
	static HHuginn::class_t get_class( HRuntime* );
protected:
	/* Value interface. */
	static HHuginn::value_t is_valid( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t value( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t next( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
protected:
	/* Iterator interface. */
	virtual bool do_is_valid( HThread* thread_, int position_ ) override {
		return ( _impl->is_valid( thread_, position_ ) );
	}
	virtual void do_next( HThread* thread_, int position_ ) override {
		_impl->next( thread_, position_ );
	}
	virtual HHuginn::value_t do_value( HThread* thread_, int position_ ) override {
		return ( _impl->value( thread_, position_ ) );
	}
	/* Iterable interface. */
	virtual iterator_t do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override;
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED */

