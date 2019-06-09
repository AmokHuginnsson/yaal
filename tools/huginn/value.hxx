/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/value.hxx
 * \brief Declaration of huginn::HValue class.
 */

#ifndef YAAL_TOOLS_HUGINN_VALUE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_VALUE_HXX_INCLUDED 1

#include "tools/huginn/class.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HValue {
public:
	typedef HValue this_type;
	typedef HHuginn::value_t value_t;
private:
	HClass const* _class;
public:
	HValue( HClass const* );
	virtual ~HValue( void ) {
		return;
	}
	HHuginn::type_id_t const& type_id( void ) const {
		return ( _class->type_id() );
	}
	HClass const* get_class( void ) const {
		return ( _class );
	}
	HHuginn::value_t clone( huginn::HThread*, HHuginn::value_t*, int ) const;
	int field_index( HHuginn::identifier_id_t const& ) const;
	bool is_kind_of( HHuginn::identifier_id_t const& ) const;
	HHuginn::value_t field( HHuginn::value_t const& subject_, int index_ ) const {
		return ( do_field( subject_, index_ ) );
	}
	HHuginn::value_t const& field( int index_ ) const {
		return ( do_field( index_ ) );
	}
	void destroy( HHuginn::value_t* object_ ) {
		do_destroy( object_ );
	}
	bool operator_equals( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
		M_ASSERT( self_.raw() == this );
		bool noneOperand( ( _class->type_id() == HHuginn::TYPE::NONE ) || ( other_->type_id() == HHuginn::TYPE::NONE ) );
		M_ASSERT( noneOperand || ( _class->type_id() == other_->type_id() ) );
		bool res( false );
		if ( ! noneOperand ) {
			res = do_operator_equals( thread_, self_, other_, position_ );
		} else {
			res = _class->type_id() == other_->type_id();
		}
		return ( res );
	}
	bool operator_less( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		return ( do_operator_less( thread_, self_, other_, position_ ) );
	}
	bool operator_greater( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		return ( do_operator_greater( thread_, self_, other_, position_ ) );
	}
	bool operator_less_or_equal( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		return ( do_operator_less_or_equal( thread_, self_, other_, position_ ) );
	}
	bool operator_greater_or_equal( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		return ( do_operator_greater_or_equal( thread_, self_, other_, position_ ) );
	}
	bool operator_contains( HThread* thread_, HHuginn::value_t const& self_, HHuginn::value_t const& other_, int position_ ) const {
		M_ASSERT( self_.raw() == this );
		return ( do_operator_contains( thread_, self_, other_, position_ ) );
	}
	void operator_add( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		do_operator_add( thread_, self_, other_, position_ );
	}
	void operator_subtract( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		do_operator_subtract( thread_, self_, other_, position_ );
	}
	void operator_multiply( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		do_operator_multipy( thread_, self_, other_, position_ );
	}
	void operator_divide( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		do_operator_divide( thread_, self_, other_, position_ );
	}
	void operator_modulo( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		do_operator_modulo( thread_, self_, other_, position_ );
	}
	void operator_power( HThread* thread_, HHuginn::value_t& self_, HHuginn::value_t const& other_, int position_ ) {
		M_ASSERT( self_.raw() == this );
		M_ASSERT( _class->type_id() == other_->type_id() );
		do_operator_power( thread_, self_, other_, position_ );
	}
private:
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	virtual bool do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	virtual bool do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	virtual bool do_operator_contains( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const;
	virtual void do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	virtual void do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	virtual void do_operator_multipy( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	virtual void do_operator_divide( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	virtual void do_operator_modulo( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	virtual void do_operator_power( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int );
	virtual void do_destroy( HHuginn::value_t* ) {}
	virtual HHuginn::value_t do_field( HHuginn::value_t const&, int ) const;
	virtual HHuginn::value_t const& do_field( int ) const;
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const;
	HValue( HValue const& ) = delete;
	HValue& operator = ( HValue const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_VALUE_HXX_INCLUDED */

