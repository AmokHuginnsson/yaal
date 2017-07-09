/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  iterator.hxx - this file is integral part of `yaal' project.

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

