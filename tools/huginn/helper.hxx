/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/helper.hxx
 * \brief Declarations of Huginn interpreter implementation helper functions.
 */

#ifndef YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED 1

#include "tools/huginn/class.hxx"
#include "tools/huginn/runtime.hxx" /* For templated `C++ function` auto-registration code. */
#include "tools/huginn/enumeration.hxx"
#include "tools/stringalgo.hxx"

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

enum class BASE {
	DEC,
	HEX,
	OCT,
	BIN
};

class HParameter {
	char const* _name;
	HClass const* _class;
	HHuginn::TYPE _type;
	HHuginn::value_t _value;
public:
	HParameter( void )
		: _name( nullptr )
		, _class( nullptr )
		, _type( HHuginn::TYPE::UNKNOWN )
		, _value() {
	}
	HParameter( char const* name_, HClass const* class_ )
		: _name( name_ )
		, _class( class_ )
		, _type( HHuginn::TYPE::UNKNOWN )
		, _value() {
	}
	HParameter( char const* name_, HHuginn::TYPE type_ )
		: _name( name_ )
		, _class( nullptr )
		, _type( type_ )
		, _value() {
	}
	char const* name( void ) const {
		return ( _name );
	}
	HClass const* cls( void ) const {
		return ( _class );
	}
	HHuginn::TYPE type( void ) const {
		return ( _type );
	}
	HHuginn::value_t const& value( void ) const {
		return ( _value );
	}
	void set_value( HHuginn::value_t&& value_ ) {
		_value = yaal::move( value_ );
	}
private:
	HParameter( HParameter const& ) = delete;
	HParameter& operator = ( HParameter const& ) = delete;
};

class HThread;

typedef yaal::hcore::HArray<HHuginn::TYPE> types_t;
typedef yaal::hcore::HArray<huginn::HClass const*> classes_t;

bool is_keyword( yaal::hcore::HString const& );
bool is_builtin( yaal::hcore::HString const& );
bool is_restricted( yaal::hcore::HString const& );

yaal::hcore::HString full_class_name( huginn::HRuntime const&, huginn::HClass const*, bool = true );
yaal::hcore::HString full_class_name( huginn::HRuntime const&, HHuginn::value_t const&, bool = true );
yaal::hcore::HString a_type_name( HHuginn::TYPE );
yaal::hcore::HString a_type_name( huginn::HClass const* );
inline yaal::hcore::HString a_type_name( HHuginn::type_id_t type_ ) {
	return ( a_type_name( static_cast<HHuginn::TYPE>( type_.get() ) ) );
}

void operands_type_mismatch( char const*, huginn::HClass const*, huginn::HClass const*, int, int ) __attribute__(( noreturn ));
HHuginn::class_t add_class_as_member( huginn::HClass*, HHuginn::class_t const&, yaal::hcore::HString const&, huginn::HClass::MEMBER_TYPE = huginn::HClass::MEMBER_TYPE::INSTANCE );
HHuginn::class_t add_class_as_member( huginn::HClass*, HHuginn::class_t const&, huginn::HClass::MEMBER_TYPE = huginn::HClass::MEMBER_TYPE::INSTANCE );
enumeration::HEnumerationClass::ptr_t add_enumeration_as_member( huginn::HClass*, enumeration::HEnumerationClass::ptr_t const&, yaal::hcore::HString const&, huginn::HClass::MEMBER_TYPE = huginn::HClass::MEMBER_TYPE::INSTANCE );
HHuginn::class_t class_exception( huginn::HClass*, huginn::HClass const* = nullptr );
HHuginn::class_t create_class(
	HRuntime*,
	yaal::hcore::HString const&,
	HHuginn::field_definitions_t const&,
	yaal::hcore::HString const&,
	HHuginn::VISIBILITY,
	huginn::HClass const* = nullptr
);
void verify_arg_count( char const*, HHuginn::values_t&, int, int, huginn::HThread*, int );
void verify_arg_type( char const*, HHuginn::values_t&, int, HHuginn::TYPE, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_type( char const*, HHuginn::values_t&, int, types_t const&, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_type( char const*, HHuginn::values_t&, int, classes_t const&, ARITY, huginn::HThread*, int );
void verify_arg_type( char const*, HHuginn::values_t&, int, huginn::HClass const*, ARITY, huginn::HThread*, int );
void verify_signature( char const*, HHuginn::values_t&, int, types_t const&, huginn::HThread*, int );
void verify_signature_by_class( char const*, HHuginn::values_t&, int, classes_t const&, huginn::HThread*, int );
inline void verify_signature( char const* name_, HHuginn::values_t& values_, types_t const& types_, huginn::HThread* thread_, int position_ ) {
	verify_signature( name_, values_, 0, types_, thread_, position_ );
}
inline void verify_signature_by_class( char const* name_, HHuginn::values_t& values_, classes_t const& classes_, huginn::HThread* thread_, int position_ ) {
	verify_signature_by_class( name_, values_, 0, classes_, thread_, position_ );
}
HHuginn::type_id_t verify_arg_numeric( char const*, HHuginn::values_t&, int, ARITY, huginn::HThread*, int );
HHuginn::value_t verify_arg_callable( char const*, HHuginn::values_t&, int, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_materialized_collection( char const*, HHuginn::values_t&, int, ARITY, huginn::HThread*, int );
HHuginn::value_t verify_arg_virtual_collection( char const*, HHuginn::values_t&, int, ARITY, huginn::HThread*, int );
HHuginn::type_id_t verify_arg_collection_value_type( char const*, HHuginn::values_t&, int, ARITY, types_t const&, UNIFORMITY, huginn::HThread*, int );
void verify_named_parameters( char const*, HHuginn::values_t&, HParameter*, huginn::HThread*, int );

bool is_numeric( huginn::HClass const* );
bool is_collection( huginn::HClass const* );
bool is_collection_like( huginn::HClass const* );
bool is_numeric_congruent( huginn::HClass const* );
bool is_set_congruent( huginn::HClass const* );
bool is_summable( huginn::HClass const*, bool );
bool is_comparable( huginn::HClass const* );
bool is_comparable_congruent( huginn::HClass const* );
bool is_boolean_congruent( huginn::HClass const* );
bool is_unknown( huginn::HClass const* );
bool is_reference_congruent( huginn::HClass const* );
bool is_integer_congruent( huginn::HClass const* );
bool is_meta_class( huginn::HClass const* );
inline bool is_enumeral( HHuginn::value_t const& val_ ) {
	return ( dynamic_cast<huginn::HEnumeral const*>( val_.raw() ) != nullptr );
}
inline bool is_enum_class( huginn::HClass const* class_ ) {
	return ( dynamic_cast<enumeration::HEnumerationClass const*>( class_ ) != nullptr );
}
inline bool is_enum_class( huginn::HValue const* val_ ) {
	return ( is_enum_class( val_->get_class() ) );
}
inline bool is_enum_class( HHuginn::value_t const* val_ ) {
	return ( is_enum_class( val_->raw() ) );
}
inline bool is_enum_class( HHuginn::value_t const& val_ ) {
	return ( is_enum_class( val_.raw() ) );
}
bool are_congruous( huginn::HClass const*, huginn::HClass const* );

huginn::HString::value_type const& get_string( HHuginn::value_t const& );
huginn::HInteger::value_type get_integer( HHuginn::value_t const& );
huginn::HReal::value_type get_real( HHuginn::value_t const& );
huginn::HNumber::value_type const& get_number( HHuginn::value_t const& );
huginn::HBoolean::value_type get_boolean( HHuginn::value_t const& );
huginn::HCharacter::value_type get_character( HHuginn::value_t const& );
huginn::HList::values_t const& get_list( HHuginn::value_t const& );
huginn::HEnumeral::value_type get_enumeral( HHuginn::value_t const& );
yaal::tools::string::tokens_t get_strings( HHuginn::value_t const& );

huginn::HString::value_type const& get_string( huginn::HValue const* );
huginn::HInteger::value_type get_integer( huginn::HValue const* );
huginn::HReal::value_type get_real( huginn::HValue const* );
huginn::HNumber::value_type const& get_number( huginn::HValue const* );
huginn::HBoolean::value_type get_boolean( huginn::HValue const* );
huginn::HCharacter::value_type get_character( huginn::HValue const* );
huginn::HEnumeral::value_type get_enumeral( huginn::HValue const* );

template<typename data_t>
data_t get_by_type( HHuginn::value_t const& );

template<typename data_t>
data_t get_by_type( HHuginn::value_t& );

yaal::hcore::HString int_to_str( huginn::HInteger::value_type, BASE, bool );

template<typename T>
class HAnchorGuard {
	T const& _obj;
public:
	HAnchorGuard( T const& obj_, HThread* thread_, int position_ )
		: _obj( obj_ ) {
		_obj.anchor( thread_, position_ );
	}
	~HAnchorGuard( void ) {
		_obj.detach();
	}
};

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

inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	HHuginn::identifier_id_t id_,
	HHuginn::value_t (*func_)( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ),
	char const* doc_
) {
	return {
		runtime_->identifier_name( id_ ),
		runtime_->object_factory()->create_function_reference(
			id_, yaal::hcore::call( func_, yaal::hcore::_1, yaal::hcore::_2, yaal::hcore::_3, yaal::hcore::_4 ), doc_
		),
		doc_
	};
}

inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	char const* name_,
	HHuginn::value_t (*func_)(
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return ( create_field( runtime_, runtime_->identifier_id( name_ ), func_, doc_ ) );
}

template<typename T0>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_, HHuginn::identifier_id_t id_,
	T0&& a0_,
	HHuginn::value_t (*func_)( typename trait::decay<T0>::type, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ),
	char const* doc_
) {
	return {
		runtime_->identifier_name( id_ ),
		runtime_->object_factory()->create_function_reference(
			id_, yaal::hcore::call(
				func_, yaal::forward<T0>( a0_ ),
				yaal::hcore::_1, yaal::hcore::_2, yaal::hcore::_3, yaal::hcore::_4
			), doc_
		),
		doc_
	};
}

template<typename T0>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	char const* name_,
	T0&& a0_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return ( create_field( runtime_, runtime_->identifier_id( name_ ), yaal::forward<T0>( a0_ ), func_, doc_ ) );
}

template<typename T0, typename T1>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	HHuginn::identifier_id_t id_,
	T0&& a0_, T1&& a1_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type, typename trait::decay<T1>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return {
		runtime_->identifier_name( id_ ),
		runtime_->object_factory()->create_function_reference(
			id_, yaal::hcore::call(
				func_, yaal::forward<T0>( a0_ ), yaal::forward<T1>( a1_ ),
				yaal::hcore::_1, yaal::hcore::_2, yaal::hcore::_3, yaal::hcore::_4
			), doc_
		),
		doc_
	};
}

template<typename T0, typename T1>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	char const* name_,
	T0&& a0_, T1&& a1_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type, typename trait::decay<T1>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return ( create_field( runtime_, runtime_->identifier_id( name_ ), yaal::forward<T0>( a0_ ), yaal::forward<T1>( a1_ ), func_, doc_ ) );
}

template<typename T0, typename T1, typename T2>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	HHuginn::identifier_id_t id_,
	T0&& a0_, T1&& a1_, T2&& a2_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type, typename trait::decay<T1>::type, typename trait::decay<T2>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return {
		runtime_->identifier_name( id_ ),
		runtime_->object_factory()->create_function_reference(
			id_,
			yaal::hcore::call(
				func_,
				yaal::forward<T0>( a0_ ), yaal::forward<T1>( a1_ ), yaal::forward<T2>( a2_ ),
				yaal::hcore::_1, yaal::hcore::_2, yaal::hcore::_3, yaal::hcore::_4
			),
			doc_
		),
		doc_
	};
}

template<typename T0, typename T1, typename T2>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	char const* name_,
	T0&& a0_, T1&& a1_, T2&& a2_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type, typename trait::decay<T1>::type, typename trait::decay<T2>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return ( create_field( runtime_, runtime_->identifier_id( name_ ), yaal::forward<T0>( a0_ ), yaal::forward<T1>( a1_ ), yaal::forward<T2>( a2_ ), func_, doc_ ) );
}

template<typename T0, typename T1, typename T2, typename T3>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	HHuginn::identifier_id_t id_,
	T0&& a0_, T1&& a1_, T2&& a2_, T3&& a3_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type, typename trait::decay<T1>::type, typename trait::decay<T2>::type, typename trait::decay<T3>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return {
		runtime_->identifier_name( id_ ),
		runtime_->object_factory()->create_function_reference(
			id_,
			yaal::hcore::call(
				func_,
				yaal::forward<T0>( a0_ ), yaal::forward<T1>( a1_ ), yaal::forward<T2>( a2_ ), yaal::forward<T3>( a3_ ),
				yaal::hcore::_1, yaal::hcore::_2, yaal::hcore::_3, yaal::hcore::_4
			),
			doc_
		),
		doc_
	};
}

template<typename T0, typename T1, typename T2, typename T3>
inline HHuginn::HFieldDefinition create_field(
	HRuntime* runtime_,
	char const* name_,
	T0&& a0_, T1&& a1_, T2&& a2_, T3&& a3_,
	HHuginn::value_t (*func_)(
		typename trait::decay<T0>::type, typename trait::decay<T1>::type, typename trait::decay<T2>::type, typename trait::decay<T3>::type,
		huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int
	),
	char const* doc_
) {
	return ( create_field( runtime_, runtime_->identifier_id( name_ ), yaal::forward<T0>( a0_ ), yaal::forward<T1>( a1_ ), yaal::forward<T2>( a2_ ), yaal::forward<T3>( a3_ ), func_, doc_ ) );
}

typedef yaal::hcore::HArray<yaal::hcore::HUTF8String> utf8_converters_t;

template<int const ARG_COUNT, bool const RETURN_VOID, typename F>
struct function_wrapper;

template<typename T>
struct huginn_type_from_integer {
	static HHuginn::TYPE const value = HHuginn::TYPE::INTEGER;
	static T get( utf8_converters_t&, huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
		T val( 0 );
		try {
			val = safe_int::cast<T>( get_integer( value_ ) );
		} catch ( yaal::hcore::HOutOfRangeException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->file_id(), position_ );
		}
		return ( val );
	}
	static HHuginn::value_t make( HRuntime& runtime_, T value_ ) {
		return ( runtime_.object_factory()->create_integer( value_ ) );
	}
};

template<typename T>
struct huginn_type_from_character {
	static HHuginn::TYPE const value = HHuginn::TYPE::CHARACTER;
	static T get( utf8_converters_t&, huginn::HThread* thread_, HHuginn::value_t const& value_, int position_ ) {
		T val( 0 );
		try {
			val = safe_int::cast<T>( get_character( value_ ).get() );
		} catch ( yaal::hcore::HOutOfRangeException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->file_id(), position_ );
		}
		return ( val );
	}
	static HHuginn::value_t make( HRuntime& runtime_, T value_ ) {
		return ( runtime_.object_factory()->create_character( code_point_t( value_ ) ) );
	}
};

template<typename T>
struct huginn_type_from_real {
	static HHuginn::TYPE const value = HHuginn::TYPE::REAL;
	static T get( utf8_converters_t&, huginn::HThread*, HHuginn::value_t const& value_, int ) {
		return ( static_cast<T>( get_real( value_ ) ) );
	}
	static HHuginn::value_t make( HRuntime& runtime_, T value_ ) {
		return ( runtime_.object_factory()->create_real( value_ ) );
	}
};

template<typename T>
struct huginn_type_from_pod;

template<>
struct huginn_type_from_pod<bool> {
	static HHuginn::TYPE const value = HHuginn::TYPE::BOOLEAN;
	static bool get( utf8_converters_t&, huginn::HThread*, HHuginn::value_t const& value_, int ) {
		return ( get_boolean( value_ ) );
	}
	static HHuginn::value_t make( HRuntime& runtime_, bool value_ ) {
		return ( runtime_.boolean_value( value_ ) );
	}
};

/*
 * Work-around for bug in GCC 4.8.4,
 * Superfluous warning about non-virtual destructor in base class.
 * In our case non of the classes in question have any fields
 * so the warning makes no sense.
 */
#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */

template<>
struct huginn_type_from_pod<int long long> : public huginn_type_from_integer<int long long> {
};
template<>
struct huginn_type_from_pod<int long long unsigned> : public huginn_type_from_integer<int long long unsigned> {
};

template<>
struct huginn_type_from_pod<int long> : public huginn_type_from_integer<int long> {
};

template<>
struct huginn_type_from_pod<int long unsigned> : public huginn_type_from_integer<int long unsigned> {
};

template<>
struct huginn_type_from_pod<int> : public huginn_type_from_integer<int> {
};

template<>
struct huginn_type_from_pod<int unsigned> : public huginn_type_from_integer<int unsigned> {
};

template<>
struct huginn_type_from_pod<int short> : public huginn_type_from_integer<int short> {
};

template<>
struct huginn_type_from_pod<int short unsigned> : public huginn_type_from_integer<int short unsigned> {
};

template<>
struct huginn_type_from_pod<char> : public huginn_type_from_character<char> {
};

template<>
struct huginn_type_from_pod<char unsigned> : public huginn_type_from_character<char unsigned> {
};

template<>
struct huginn_type_from_pod<char signed> : public huginn_type_from_character<char signed> {
};

template<>
struct huginn_type_from_pod<double long> : public huginn_type_from_real<double long> {
};

template<>
struct huginn_type_from_pod<double> : public huginn_type_from_real<double> {
};

template<>
struct huginn_type_from_pod<float> : public huginn_type_from_real<float> {
};

#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005
# pragma GCC diagnostic pop
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4008005 */

template<>
struct huginn_type_from_pod<code_point_t> {
	static HHuginn::TYPE const value = HHuginn::TYPE::CHARACTER;
	static code_point_t get( utf8_converters_t&, huginn::HThread*, HHuginn::value_t const& value_, int ) {
		return ( get_character( value_ ) );
	}
	static HHuginn::value_t make( HRuntime& runtime_, code_point_t value_ ) {
		return ( runtime_.object_factory()->create_character( value_ ) );
	}
};

template<>
struct huginn_type_from_pod<yaal::hcore::HNumber> {
	static HHuginn::TYPE const value = HHuginn::TYPE::NUMBER;
	static yaal::hcore::HNumber const& get( utf8_converters_t&, huginn::HThread*, HHuginn::value_t const& value_, int ) {
		return ( get_number( value_ ) );
	}
	static HHuginn::value_t make( HRuntime& runtime_, yaal::hcore::HNumber&& value_ ) {
		return ( runtime_.object_factory()->create_number( yaal::move( value_ ) ) );
	}
};

template<>
struct huginn_type_from_pod<yaal::hcore::HString> {
	static HHuginn::TYPE const value = HHuginn::TYPE::STRING;
	static yaal::hcore::HString const& get( utf8_converters_t&, huginn::HThread*, HHuginn::value_t const& value_, int ) {
		return ( get_string( value_ ) );
	}
	static HHuginn::value_t make( HRuntime& runtime_, yaal::hcore::HString&& value_ ) {
		return ( runtime_.object_factory()->create_string( yaal::move( value_ ) ) );
	}
};

template<>
struct huginn_type_from_pod<char const*> {
	static HHuginn::TYPE const value = HHuginn::TYPE::STRING;
	static char const* get( utf8_converters_t& utf8_, huginn::HThread*, HHuginn::value_t const& value_, int ) {
		yaal::hcore::HString const& s( get_string( value_ ) );
		utf8_.emplace_back( s );
		return ( utf8_.back().c_str() );
	}
	static HHuginn::value_t make( HRuntime& runtime_, char const* value_ ) {
		return ( value_ ? runtime_.object_factory()->create_string( value_ ) : runtime_.none_value() );
	}
};

template<typename T, int const N>
struct decayed_argument_type {
	typedef typename trait::decay<typename trait::argument_type<T, N>::type>::type type;
};

template<typename T>
struct decayed_return_type {
	typedef typename trait::decay<typename trait::return_type<T>::type>::type type;
};

void dispatch_callback_exception( yaal::hcore::HUTF8String const&, huginn::HThread*, int );

template<typename F>
struct function_wrapper<0, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 0, 0, thread_, position_ );
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_()
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<0, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 0, 0, thread_, position_ );
	try {
		fun_();
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<1, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 1, 1, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::UNARY, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<1, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 1, 1, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::UNARY, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<2, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 2, 2, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<2, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 2, 2, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<3, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 3, 3, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<3, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 3, 3, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<4, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 4, 4, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<4, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 4, 4, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<5, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 5, 5, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<5, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 5, 5, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<6, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 6, 6, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<6, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 6, 6, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<7, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 7, 7, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<7, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 7, 7, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<8, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 8, 8, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 7, huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::get( utf8, thread_, values_[7], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<8, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 8, 8, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 7, huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::get( utf8, thread_, values_[7], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<9, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 9, 9, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 7, huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 8, huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::get( utf8, thread_, values_[7], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::get( utf8, thread_, values_[8], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<9, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 9, 9, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 7, huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 8, huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::get( utf8, thread_, values_[7], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::get( utf8, thread_, values_[8], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<10, false, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 10, 10, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 7, huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 8, huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 9, huginn_type_from_pod<typename decayed_argument_type<F, 9>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		return (
			huginn_type_from_pod<typename decayed_return_type<F>::type>::make(
				thread_->runtime(),
				fun_(
					huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::get( utf8, thread_, values_[7], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::get( utf8, thread_, values_[8], position_ ),
					huginn_type_from_pod<typename decayed_argument_type<F, 9>::type>::get( utf8, thread_, values_[9], position_ )
				)
			)
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
struct function_wrapper<10, true, F> {
static HHuginn::value_t impl( F fun_, yaal::hcore::HUTF8String const& name_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( name_.c_str(), values_, 10, 10, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 0, huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 1, huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 2, huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 3, huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 4, huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 5, huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 6, huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 7, huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 8, huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_type( name_.c_str(), values_, 9, huginn_type_from_pod<typename decayed_argument_type<F, 9>::type>::value, ARITY::MULTIPLE, thread_, position_ );
	utf8_converters_t utf8;
	try {
		fun_(
			huginn_type_from_pod<typename decayed_argument_type<F, 0>::type>::get( utf8, thread_, values_[0], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 1>::type>::get( utf8, thread_, values_[1], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 2>::type>::get( utf8, thread_, values_[2], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 3>::type>::get( utf8, thread_, values_[3], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 4>::type>::get( utf8, thread_, values_[4], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 5>::type>::get( utf8, thread_, values_[5], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 6>::type>::get( utf8, thread_, values_[6], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 7>::type>::get( utf8, thread_, values_[7], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 8>::type>::get( utf8, thread_, values_[8], position_ ),
			huginn_type_from_pod<typename decayed_argument_type<F, 9>::type>::get( utf8, thread_, values_[9], position_ )
		);
	} catch ( ... ) {
		dispatch_callback_exception( name_, thread_, position_ );
	}
	return ( thread_->runtime().none_value() );
	M_EPILOG
}
};

template<typename F>
HHuginn::function_t function( yaal::hcore::HString const& name_, F fun_ ) {
	typedef typename trait::return_type<F>::type return_type;
	bool const returns_void( trait::same_type<return_type, void>::value || trait::same_type<return_type, trait::no_type>::value );
	return (
		yaal::hcore::call(
			&function_wrapper<trait::argument_count<F>::value, returns_void, F>::impl,
			fun_,
			name_,
			yaal::hcore::_1,
			yaal::hcore::_2,
			yaal::hcore::_3,
			yaal::hcore::_4
		)
	);
}

template<typename F>
void register_function( HHuginn& h_, yaal::hcore::HString const& name_, F fun_, yaal::hcore::HString const& doc_ ) {
	M_PROLOG
	h_.register_function( name_, huginn::function( name_, fun_ ), doc_ );
	return;
	M_EPILOG
}

inline HHuginn::value_t call_function( yaal::tools::HHuginn& huginn_, yaal::tools::HHuginn::values_t& argsHuginn_, yaal::hcore::HString const& name_ ) {
	return ( huginn_.call( name_, argsHuginn_ ) );
}

template<typename arg_t, typename... args_t>
inline yaal::tools::HHuginn::value_t call_function(
	yaal::tools::HHuginn& huginn_,
	yaal::tools::HHuginn::values_t& argsHuginn_,
	yaal::hcore::HString const& name_,
	arg_t const& arg_,
	args_t... argsCXX_
) {
	argsHuginn_.push_back( huginn_.value( arg_ ) );
	return ( call_function( huginn_, argsHuginn_, name_, yaal::forward<args_t>( argsCXX_ )... ) );
}

template<typename... args_t>
inline yaal::tools::HHuginn::value_t call_function( yaal::tools::HHuginn& huginn_, yaal::hcore::HString const& name_, args_t... argsCXX_ ) {
	M_PROLOG
	yaal::tools::HHuginn::values_t args;
	return ( call_function( huginn_, args, name_, yaal::forward<args_t>( argsCXX_ )... ) );
	M_EPILOG
}

namespace package {

HHuginn::value_t value( HHuginn::value_t, yaal::hcore::HUTF8String, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
HHuginn::value_t instance( huginn::HClass const*, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_HELPER_HXX_INCLUDED */

