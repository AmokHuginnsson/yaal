/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/reference.hxx
 * \brief Declaration of huginn::HReference class.
 */

#ifndef YAAL_TOOLS_HUGINN_REFERENCE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_REFERENCE_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HReference : public HValue {
public:
	typedef HReference this_type;
	typedef HValue base_type;
private:
	HHuginn::value_t* _ref;
public:
	HReference( HClass const* class_, HHuginn::value_t* value_ )
		: HValue( class_ )
		, _ref( value_ ) {
		return;
	}
	HHuginn::value_t get( huginn::HThread* thread_, int position_ ) const {
		return ( do_get( thread_, position_ ) );
	}
	void set( huginn::HThread* thread_, HHuginn::value_t&& value_, int position_ ) {
		do_set( thread_, yaal::move( value_ ), position_ );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
	virtual HHuginn::value_t do_get( huginn::HThread*, int ) const;
	virtual void do_set( huginn::HThread*, HHuginn::value_t&&, int );
private:
	HReference( HReference const& ) = delete;
	HReference& operator = ( HReference const& ) = delete;
};

class HSubscriptReference : public HReference {
public:
	typedef HSubscriptReference this_type;
	typedef HReference base_type;
private:
	HHuginn::value_t _base;
	HHuginn::value_t _key;
public:
	HSubscriptReference( HClass const*, HHuginn::value_t const&, HHuginn::value_t const& );
private:
	virtual HHuginn::value_t do_get( huginn::HThread*, int ) const override;
	virtual void do_set( huginn::HThread*, HHuginn::value_t&&, int ) override;
};

class HMemberReference : public HReference {
public:
	typedef HMemberReference this_type;
	typedef HReference base_type;
private:
	HHuginn::value_t _object;
	HHuginn::identifier_id_t _memberId;
public:
	HMemberReference( HClass const*, HHuginn::value_t const&, HHuginn::identifier_id_t );
private:
	virtual HHuginn::value_t do_get( huginn::HThread*, int ) const override;
	virtual void do_set( huginn::HThread*, HHuginn::value_t&&, int ) override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_REFERENCE_HXX_INCLUDED */

