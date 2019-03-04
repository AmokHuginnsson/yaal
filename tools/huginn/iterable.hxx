/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/iterable.hxx
 * \brief Declaration of huginn::HIterable class.
 */

#ifndef YAAL_TOOLS_HUGINN_ITERABLE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ITERABLE_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HIteratorInterface;

class HIterable : public HValue {
public:
	typedef HIterable this_type;
	typedef HValue base_type;
	typedef yaal::hcore::HPointer<huginn::HIteratorInterface> iterator_t;
	static int long const npos = -1;
	HIterable( HClass const* );
	iterator_t iterator( huginn::HThread*, int );
	int long size( huginn::HThread*, int ) const;
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) = 0;
	virtual int long do_size( huginn::HThread*, int ) const = 0;
};

class HInvalidatingIterable : public HIterable, public HHuginn::HReferenceTracker {
public:
	HInvalidatingIterable( HClass const* );
	void skip( void const* );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ITERABLE_HXX_INCLUDED */

