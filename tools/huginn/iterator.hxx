/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED 1

#include "tools/huginn/iterable.hxx"

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

class HNotifableIterator : public HIteratorInterface, public HHuginn::HNotifableReference {
protected:
	int _skip;
public:
	HNotifableIterator( HInvalidatingIterable* owner_ )
		: HIteratorInterface()
		, HNotifableReference( owner_ )
		, _skip( 0 ) {
		return;
	}
	void skip( HThread* thread_, int position_ ) {
		do_skip( thread_, position_ );
	}
protected:
	virtual void do_skip( HThread*, int ) = 0;
};

class HIterator : public HIterable, public HIteratorInterface {
	HHuginn::value_t _source;
	HIterable::iterator_t _impl;
public:
	HIterator( huginn::HClass const*, HHuginn::value_t const&, HIterable::iterator_t const& );
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
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

class HIterableAdaptorIterator : public HIteratorInterface {
	HHuginn::value_t _source;
	HHuginn::value_t _iterator;
	HHuginn::function_t _isValidMethod;
	HHuginn::function_t _valueMethod;
	HHuginn::function_t _nextMethod;
public:
	HIterableAdaptorIterator( huginn::HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int );
protected:
	virtual bool do_is_valid( HThread*, int ) override;
	virtual void do_next( HThread*, int ) override;
	virtual HHuginn::value_t do_value( HThread*, int ) override;
};

class HIterableAdaptor : public HIterable {
	HHuginn::value_t _source;
public:
	HIterableAdaptor( huginn::HClass const*, HHuginn::value_t const& );
	static HHuginn::class_t get_class( HRuntime* );
protected:
	virtual iterator_t do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED */

