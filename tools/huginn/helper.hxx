/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  helper.hxx - this file is integral part of `yaal' project.

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

void operands_type_mismatch( char const*, HHuginn::type_id_t, HHuginn::type_id_t, int ) __attribute__(( noreturn ));
void verify_arg_count( yaal::hcore::HString const&, HHuginn::values_t const&, int, int, huginn::HThread*, int );
void verify_arg_type( yaal::hcore::HString const&, HHuginn::values_t const&, int, HHuginn::TYPE, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_type( yaal::hcore::HString const&, HHuginn::values_t const&, int, types_t const&, ARITY, huginn::HThread*, int );
void verify_arg_type( yaal::hcore::HString const&, HHuginn::values_t const&, int, HHuginn::HClass const*, ARITY, huginn::HThread*, int );
void verify_signature( yaal::hcore::HString const&, HHuginn::values_t const&, types_t const&, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_numeric( yaal::hcore::HString const&, HHuginn::values_t const&, int, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_collection( yaal::hcore::HString const&, HHuginn::values_t const&, int, ARITY, ONTICALLY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_collection_value_type( yaal::hcore::HString const&, HHuginn::values_t const&, int, ARITY, types_t const&, UNIFORMITY, huginn::HThread*, int );

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

