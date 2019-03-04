/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/class.hxx
 * \brief Declaration of huginn::HClass class.
 */

#ifndef YAAL_TOOLS_HUGINN_CLASS_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_CLASS_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HObjectFactory;

class HClass {
public:
	enum class TYPE {
		BUILTIN,
		USER
	};
	enum class MEMBER_TYPE {
		INSTANCE,
#undef STATIC
		STATIC
	};
	typedef HClass this_type;
	typedef yaal::hcore::HLookupMap<HHuginn::identifier_id_t, int> field_indexes_t;
	typedef HHuginn::value_t ( *create_instance_t )( HClass const*, huginn::HThread*, HHuginn::values_t&, int );
	typedef HHuginn::value_t ( *constructor_t )( huginn::HThread*, HHuginn::value_t*, HHuginn::HHuginn::values_t&, int );
	class HMethod;
	class HUnboundMethod;
	class HBoundMethod;
private:
	huginn::HRuntime* _runtime;
	HHuginn::type_id_t _typeId;
	HHuginn::identifier_id_t _identifierId;
	HClass const* _super;
	create_instance_t _createInstance;
	HHuginn::field_identifiers_t _fieldIdentifiers;
	field_indexes_t _staticFieldIndexes;
	field_indexes_t _fieldIndexes;
	HHuginn::HHuginn::values_t _fieldDefinitions;
	HHuginn::value_t _constructor;
	HHuginn::field_descriptions_t _fieldDescriptions;
	yaal::hcore::HString _doc;
	TYPE _type;
	HClass const* _origin;
public:
	HClass( huginn::HRuntime*, HHuginn::type_id_t, HHuginn::identifier_id_t, yaal::hcore::HString const&, HHuginn::ACCESS, TYPE = TYPE::BUILTIN, HClass const* = nullptr, create_instance_t = nullptr );
	HClass( huginn::HRuntime*, huginn::HObjectFactory*, HHuginn::type_id_t, HHuginn::identifier_id_t, yaal::hcore::HString const&, constructor_t ); /* from HObjectFactory constructor */
	HClass( huginn::HRuntime*, char const*, char const*, HClass const* ); /* ReversedTupleView, ReversedListView, ... */
	HClass( huginn::HRuntime*, HHuginn::TYPE, HHuginn::identifier_id_t, char const* ); /* *none*, *function_reference*, *observer*, ... */
	virtual ~HClass( void ) {
	}
	void redefine( HClass const*, HHuginn::field_definitions_t const& );
	void add_member( HHuginn::HFieldDefinition const&, MEMBER_TYPE = MEMBER_TYPE::INSTANCE );
	HClass const* super( void ) const {
		return ( _super );
	}
	HClass const* origin( void ) const {
		return ( _origin );
	}
	HHuginn::identifier_id_t const& identifier_id( void ) const {
		return ( _identifierId );
	}
	yaal::hcore::HString const& name( void ) const;
	HHuginn::type_id_t const& type_id( void ) const {
		return ( _typeId );
	}
	TYPE type( void ) const {
		return ( _type );
	}
	HHuginn::ACCESS access( void ) const;
	HHuginn::field_identifiers_t const& field_identifiers( void ) const {
		return ( _fieldIdentifiers );
	}
	HHuginn::values_t const& field_definitions( void ) const {
		return ( _fieldDefinitions );
	}
	int field_index( HHuginn::identifier_id_t const&, MEMBER_TYPE = MEMBER_TYPE::INSTANCE ) const;
	HHuginn::value_t const& field( int index_ ) const {
		return ( _fieldDefinitions[index_] );
	}
	HHuginn::function_t constructor_function( void ) const;
	HHuginn::values_t get_defaults( huginn::HThread*, int ) const;
	HHuginn::value_t get_default( huginn::HThread*, int, int ) const;
	HHuginn::function_t const& function( int ) const;
	bool is_kind_of( HHuginn::identifier_id_t const& ) const;
	bool is_complex( void ) const {
		return ( ! _fieldDefinitions.is_empty() );
	}
	bool is_overridden( HClass const* super_, int index_ ) const {
		return ( _fieldDefinitions[index_] != super_->_fieldDefinitions[index_] );
	}
	bool has_builtin_base( void ) const;
	void update_runtime( huginn::HRuntime* );
	void set_origin( HClass const* );
	huginn::HRuntime* runtime( void ) const {
		return ( _runtime );
	}
	HHuginn::value_t const& constructor( void ) const {
		return ( _constructor );
	}
	static HHuginn::value_t create_instance_default( HClass const*, huginn::HThread*, HHuginn::values_t&, int );
	HHuginn::value_t create_instance( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const;
	yaal::hcore::HString const& doc( void ) const;
	yaal::hcore::HString const& doc( HHuginn::identifier_id_t ) const;
	void finalize_registration( huginn::HRuntime* );
private:
	HHuginn::value_t make_constructor( huginn::HObjectFactory*, HHuginn::ACCESS ) const;
	HHuginn::value_t make_constructor( huginn::HObjectFactory*, constructor_t ) const;
	HHuginn::value_t access_violation( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const __attribute__((noreturn));
	HHuginn::value_t base_class_not_initialized( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const;
	virtual void do_finalize_registration( huginn::HRuntime* );
	virtual HHuginn::value_t do_create_instance( huginn::HThread*, HHuginn::values_t&, int ) const;
	HClass( HClass const& ) = delete;
	HClass& operator = ( HClass const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_CLASS_HXX_INCLUDED */

