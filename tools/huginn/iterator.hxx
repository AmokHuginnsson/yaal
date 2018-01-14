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

}

class HHuginn::HIterable::HIterator {
public:
	typedef yaal::hcore::HResource<huginn::HIteratorInterface> iterator_implementation_t;
private:
	iterator_implementation_t _impl;
public:
	HIterator( iterator_implementation_t&& impl_ )
		: _impl( yaal::move( impl_ ) ) {
		return;
	}
	value_t value( huginn::HThread* thread_, int position_ ) {
		return ( _impl->value( thread_, position_ ) );
	}
	bool is_valid( huginn::HThread* thread_, int position_ ) {
		return ( !! _impl && _impl->is_valid( thread_, position_ ) );
	}
	void next( huginn::HThread* thread_, int position_ ) {
		_impl->next( thread_, position_ );
	}
};

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ITERATOR_HXX_INCLUDED */

