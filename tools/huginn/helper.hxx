/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/helper.hxx
 * \brief Declarations of Huginn interpreter implementation helper functions.
 */

#ifndef YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

enum class ARITY {
	UNARY,
	MULTIPLE
};

enum class UNIFORMITY {
	REQUIRED,
	OPTIONAL
};

enum class ONTICALLY {
	MATERIALIZED,
	VIRTUAL
};

enum class BASE {
	DEC,
	HEX,
	OCT,
	BIN
};

class HThread;

typedef yaal::hcore::HArray<HHuginn::TYPE> types_t;
typedef yaal::hcore::HArray<HHuginn::HClass const*> classes_t;

bool is_keyword( yaal::hcore::HString const& );
bool is_builtin( yaal::hcore::HString const& );
bool is_restricted( yaal::hcore::HString const& );

yaal::hcore::HString a_type_name( HHuginn::TYPE );
yaal::hcore::HString a_type_name( HHuginn::HClass const* );
inline yaal::hcore::HString a_type_name( HHuginn::type_id_t type_ ) {
	return ( a_type_name( static_cast<HHuginn::TYPE>( type_.get() ) ) );
}

void operands_type_mismatch( char const*, HHuginn::HClass const*, HHuginn::HClass const*, int, int ) __attribute__(( noreturn ));
void verify_arg_count( char const*, HHuginn::values_t&, int, int, huginn::HThread*, int );
void verify_arg_type( char const*, HHuginn::values_t&, int, HHuginn::TYPE, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_type( char const*, HHuginn::values_t&, int, types_t const&, ARITY, huginn::HThread*, int );
void verify_arg_type( char const*, HHuginn::values_t&, int, HHuginn::HClass const*, ARITY, huginn::HThread*, int );
void verify_signature( char const*, HHuginn::values_t&, types_t const&, huginn::HThread*, int );
void verify_signature_by_class( char const*, HHuginn::values_t&, classes_t const&, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_numeric( char const*, HHuginn::values_t&, int, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_collection( char const*, HHuginn::values_t&, int, ARITY, ONTICALLY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_collection_value_type( char const*, HHuginn::values_t&, int, ARITY, types_t const&, UNIFORMITY, huginn::HThread*, int );

HHuginn::HString::value_type const& get_string( HHuginn::value_t const& );
HHuginn::HInteger::value_type get_integer( HHuginn::value_t const& );
HHuginn::HReal::value_type get_real( HHuginn::value_t const& );
HHuginn::HNumber::value_type const& get_number( HHuginn::value_t const& );
HHuginn::HBoolean::value_type get_boolean( HHuginn::value_t const& );
HHuginn::HCharacter::value_type get_character( HHuginn::value_t const& );
HHuginn::HList::values_t const& get_list( HHuginn::value_t const& );

HHuginn::HString::value_type const& get_string( HHuginn::HValue const* );
HHuginn::HInteger::value_type get_integer( HHuginn::HValue const* );
HHuginn::HReal::value_type get_real( HHuginn::HValue const* );
HHuginn::HNumber::value_type const& get_number( HHuginn::HValue const* );
HHuginn::HBoolean::value_type get_boolean( HHuginn::HValue const* );
HHuginn::HCharacter::value_type get_character( HHuginn::HValue const* );

template<typename data_t>
data_t get_by_type( HHuginn::value_t const& );

template<typename data_t>
data_t get_by_type( HHuginn::value_t& );

yaal::hcore::HString int_to_str( HHuginn::HInteger::value_type, BASE, bool );

template<typename data_t, typename iterator_t>
class HValueUnboxingIterator : public yaal::hcore::iterator_interface<data_t, yaal::hcore::iterator_category::forward> {
public:
	typedef HValueUnboxingIterator this_type;
	typedef yaal::hcore::iterator_interface<data_t, yaal::hcore::iterator_category::forward> base_type;
	typedef typename trait::strip_reference<typename iterator_t::value_type>::type src_const_qual_t;
	typedef typename trait::ternary<is_pod<data_t>::value, data_t, typename trait::copy_const<src_const_qual_t, data_t>::type>::type const_qual_t;
	typedef typename trait::ternary<is_pod<data_t>::value, const_qual_t, const_qual_t&>::type reference_t;
private:
	iterator_t _iter;
public:
	HValueUnboxingIterator( iterator_t iter_ )
		: _iter( iter_ ) {
	}
	HValueUnboxingIterator( HValueUnboxingIterator const& ) = default;
	HValueUnboxingIterator& operator = ( HValueUnboxingIterator const& ) = default;
	HValueUnboxingIterator& operator ++ ( void ) {
		++ _iter;
		return ( *this );
	}
	bool operator == ( HValueUnboxingIterator const& other_ ) const {
		return ( _iter == other_._iter );
	}
	bool operator != ( HValueUnboxingIterator const& other_ ) const {
		return ( _iter != other_._iter );
	}
	reference_t operator * ( void ) {
		return ( get_by_type<reference_t>( *_iter ) );
	}
};

template<typename data_t, typename iterator_t>
HValueUnboxingIterator<data_t, iterator_t> value_unboxing_iterator( iterator_t it_ ) {
	return ( HValueUnboxingIterator<data_t, iterator_t>( it_ ) );
}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED */

