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
	struct ORange {
		int long from;
		int long to;
		int long step;
		bool valid;
	};
	static int long const npos = -1;
	HIterable( HClass const* );
	iterator_t iterator( huginn::HThread*, int );
	int long size( huginn::HThread*, int ) const;
protected:
	virtual iterator_t do_iterator( huginn::HThread*, int ) = 0;
	virtual int long do_size( huginn::HThread*, int ) const = 0;
	int long long extract_index( huginn::HThread*, HHuginn::value_t const&, int ) const;
	ORange extract_range( huginn::HThread*, HHuginn::value_t const&, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
};

class HInvalidatingIterable : public HIterable, public HHuginn::HReferenceTracker {
public:
	HInvalidatingIterable( HClass const* );
	void skip( HThread*, void const*, int );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ITERABLE_HXX_INCLUDED */

