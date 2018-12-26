/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/huginn/compiler.hxx"
#include "tools/huginn/introexpression.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

void OCompiler::merge( HHuginn::expression_t& to_, HHuginn::expression_t& from_ ) {
	M_PROLOG
	int stepOffset( to_->execution_step_count() );
	to_->merge( *from_ );
	for ( OExecutionStep& es : _executionStepsBacklog ) {
		if ( es._expression == from_ ) {
			es._expression = to_;
			es._index += stepOffset;
		}
	}
	return;
	M_EPILOG
}

namespace {
HString use_name( HHuginn::SYMBOL_KIND symbolKind_ ) {
	HString name;
	switch ( symbolKind_ ) {
		case ( HHuginn::SYMBOL_KIND::UNKNOWN ):  name = "Symbol";   break;
		case ( HHuginn::SYMBOL_KIND::CLASS ):    name = "Class";    break;
		case ( HHuginn::SYMBOL_KIND::FIELD ):    name = "Field";    break;
		case ( HHuginn::SYMBOL_KIND::METHOD ):   name = "Method";   break;
		case ( HHuginn::SYMBOL_KIND::FUNCTION ): name = "Function"; break;
		case ( HHuginn::SYMBOL_KIND::ENUM ):     name = "Enum";     break;
		case ( HHuginn::SYMBOL_KIND::VARIABLE ): name = "Variable"; break;
		case ( HHuginn::SYMBOL_KIND::PACKAGE ):  name = "Package";  break;
	}
	return ( name );
}
}

void OCompiler::detect_misuse( void ) const {
	M_PROLOG
	HHuginn::identifier_id_t implicitUse[] = {
		IDENTIFIER::STANDARD_FUNCTIONS::MAIN,
		_runtime->identifier_id( INTERFACE::GET_SIZE ),
		_runtime->identifier_id( INTERFACE::SUBSCRIPT ),
		_runtime->identifier_id( INTERFACE::MEMBER ),
		_runtime->identifier_id( INTERFACE::ITERATOR ),
		_runtime->identifier_id( INTERFACE::IS_VALID ),
		_runtime->identifier_id( INTERFACE::NEXT ),
		_runtime->identifier_id( INTERFACE::VALUE ),
		_runtime->identifier_id( INTERFACE::CALL ),
		_runtime->identifier_id( INTERFACE::HASH ),
		_runtime->identifier_id( INTERFACE::EQUALS ),
		_runtime->identifier_id( INTERFACE::LESS ),
		_runtime->identifier_id( INTERFACE::ADD ),
		_runtime->identifier_id( INTERFACE::SUBTRACT ),
		_runtime->identifier_id( INTERFACE::MULTIPLY ),
		_runtime->identifier_id( INTERFACE::DIVIDE ),
		_runtime->identifier_id( INTERFACE::MODULO ),
		_runtime->identifier_id( INTERFACE::POWER ),
		_runtime->identifier_id( INTERFACE::MODULUS ),
		_runtime->identifier_id( INTERFACE::NEGATE ),
		_runtime->identifier_id( INTERFACE::CLONE ),
		_runtime->identifier_id( "to_string" ),
		_runtime->identifier_id( "to_integer" ),
		_runtime->identifier_id( "to_real" ),
		_runtime->identifier_id( "to_number" ),
		_runtime->identifier_id( "to_character" ),
		_runtime->identifier_id( "to_boolean" )
	};
	for ( used_identifiers_t::value_type const& iu : _usedIdentifiers ) {
		HHuginn::identifier_id_t id( iu.first );
		OIdentifierUse const& use( iu.second );
		if ( use._readCount == 0 ) {
			if (
				( find( begin( implicitUse ), end( implicitUse ), id ) == end( implicitUse ) )
				&& (
					! _isModule
					|| ( use._type == HHuginn::SYMBOL_KIND::VARIABLE )
					|| ( use._type == HHuginn::SYMBOL_KIND::UNKNOWN )
				)
			) {
				throw HHuginn::HHuginnRuntimeException(
					use_name( use._type )
						.append( " `" )
						.append( _runtime->identifier_name( id ) )
						.append( "' is never used (did you mean `" )
						.append( _runtime->suggestion( id ) )
						.append( "'?)." ),
					_fileId,
					use._writePosition
				);
			}
		}
	}
	return;
	M_EPILOG
}

OCompiler::OFunctionContext& OCompiler::f( void ) {
	return ( *_functionContexts.top() );
}

OCompiler::OFunctionContext const& OCompiler::f( void ) const {
	return ( *_functionContexts.top() );
}

HHuginn::HHuginn::scope_t& OCompiler::current_scope( void ) {
	return ( f()._scopeStack.top()->_scope );
}

HHuginn::HHuginn::expression_t& OCompiler::current_expression( void ) {
	return ( f()._scopeStack.top()->expression() );
}

OCompiler::OScopeContext& OCompiler::current_scope_context( void ) {
	return ( *f()._scopeStack.top() );
}

HHuginn::expression_t OCompiler::new_expression( int fileId_, executing_parser::range_t const& range_ ) {
	return (
		_introspector
			? pointer_static_cast<HExpression>( make_pointer<HIntroExpression>( _introspector, fileId_, range_ ) )
			: make_pointer<HExpression>( fileId_, range_ )
	);
}

HHuginn::HClass const* OCompiler::guess_type( HHuginn::identifier_id_t identifierId_ ) const {
	HHuginn::HClass const* c( f()._scopeStack.top()->guess_type( this, identifierId_ ) );
	if ( c == type_to_class( HHuginn::TYPE::UNKNOWN ) ) {
		HHuginn::value_t const* v( _runtime->get_global( identifierId_ ) );
		if ( ( v && ( (*v)->type_id() == HHuginn::TYPE::FUNCTION_REFERENCE ) )
			|| ( find( _classIdentifiers.begin(), _classIdentifiers.end(), identifierId_ ) != _classIdentifiers.end() ) ) {
			c = type_to_class( HHuginn::TYPE::FUNCTION_REFERENCE );
		}
	}
	return ( c );
}

HHuginn::HClass const* OCompiler::type_to_class( HHuginn::TYPE type_ ) const {
	return ( type_id_to_class( type_id( type_ ) ) );
}

HHuginn::HClass const* OCompiler::type_id_to_class( HHuginn::type_id_t typeId_ ) const {
	HHuginn::HClass const* c( nullptr );
	HObjectFactory const& of( *_runtime->object_factory() );
	switch ( static_cast<HHuginn::TYPE>( typeId_.get() ) ) {
		case ( HHuginn::TYPE::INTEGER ):             c = of.integer_class();             break;
		case ( HHuginn::TYPE::REAL ):                c = of.real_class();                break;
		case ( HHuginn::TYPE::STRING ):              c = of.string_class();              break;
		case ( HHuginn::TYPE::NUMBER ):              c = of.number_class();              break;
		case ( HHuginn::TYPE::CHARACTER ):           c = of.character_class();           break;
		case ( HHuginn::TYPE::BOOLEAN ):             c = of.boolean_class();             break;
		case ( HHuginn::TYPE::TUPLE ):               c = of.tuple_class();               break;
		case ( HHuginn::TYPE::LIST ):                c = of.list_class();                break;
		case ( HHuginn::TYPE::DEQUE ):               c = of.deque_class();               break;
		case ( HHuginn::TYPE::DICT ):                c = of.dict_class();                break;
		case ( HHuginn::TYPE::LOOKUP ):              c = of.lookup_class();              break;
		case ( HHuginn::TYPE::ORDER ):               c = of.order_class();               break;
		case ( HHuginn::TYPE::SET ):                 c = of.set_class();                 break;
		case ( HHuginn::TYPE::BLOB ):                c = of.blob_class();                break;
		case ( HHuginn::TYPE::NONE ):                c = of.none_class();                break;
		case ( HHuginn::TYPE::OBSERVER ):            c = of.observer_class();            break;
		case ( HHuginn::TYPE::REFERENCE ):           c = of.reference_class();           break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ):  c = of.function_reference_class();  break;
		case ( HHuginn::TYPE::OBJECT_REFERENCE ):    c = of.object_reference_class();    break;
		case ( HHuginn::TYPE::METHOD ):              c = of.method_class();              break;
		case ( HHuginn::TYPE::UNBOUND_METHOD ):      c = of.unbound_method_class();      break;
		case ( HHuginn::TYPE::BOUND_METHOD ):        c = of.bound_method_class();        break;
		case ( HHuginn::TYPE::VARIADIC_PARAMETERS ): c = of.variadic_parameters_class(); break;
		case ( HHuginn::TYPE::NAMED_PARAMETERS ):    c = of.named_parameters_class();    break;
		case ( HHuginn::TYPE::NOT_BOOLEAN ):
		case ( HHuginn::TYPE::UNKNOWN ):             c = nullptr;            break;
	}
	return ( c );
}

void OCompiler::note_type( HHuginn::identifier_id_t identifierId_, HHuginn::HClass const* class_ ) {
	f()._scopeStack.top()->note_type( identifierId_, class_ );
}

HHuginn::HClass const* OCompiler::function_ref_to_class( HHuginn::identifier_id_t identifierId_ ) {
	HHuginn::type_id_t t( type_id( HHuginn::TYPE::UNKNOWN ) );
	typedef HHashMap<HHuginn::identifier_id_t, HHuginn::type_id_t> fun_to_type_t;
	static fun_to_type_t const funToType = {
		{ IDENTIFIER::BUILTIN::INTEGER,   type_id( HHuginn::TYPE::INTEGER ) },
		{ IDENTIFIER::BUILTIN::REAL,      type_id( HHuginn::TYPE::REAL ) },
		{ IDENTIFIER::BUILTIN::NUMBER,    type_id( HHuginn::TYPE::NUMBER ) },
		{ IDENTIFIER::BUILTIN::STRING,    type_id( HHuginn::TYPE::STRING ) },
		{ IDENTIFIER::BUILTIN::CHARACTER, type_id( HHuginn::TYPE::CHARACTER ) },
		{ IDENTIFIER::BUILTIN::BOOLEAN,   type_id( HHuginn::TYPE::BOOLEAN ) },
		{ IDENTIFIER::BUILTIN::SIZE,      type_id( HHuginn::TYPE::INTEGER ) },
		{ IDENTIFIER::BUILTIN::TUPLE,     type_id( HHuginn::TYPE::TUPLE ) },
		{ IDENTIFIER::BUILTIN::LIST,      type_id( HHuginn::TYPE::LIST ) },
		{ IDENTIFIER::BUILTIN::DEQUE,     type_id( HHuginn::TYPE::DEQUE ) },
		{ IDENTIFIER::BUILTIN::DICT,      type_id( HHuginn::TYPE::DICT ) },
		{ IDENTIFIER::BUILTIN::LOOKUP,    type_id( HHuginn::TYPE::LOOKUP ) },
		{ IDENTIFIER::BUILTIN::ORDER,     type_id( HHuginn::TYPE::ORDER ) },
		{ IDENTIFIER::BUILTIN::SET,       type_id( HHuginn::TYPE::SET ) },
		{ IDENTIFIER::BUILTIN::BLOB,      type_id( HHuginn::TYPE::BLOB ) },
		{ IDENTIFIER::BUILTIN::OBSERVE,   type_id( HHuginn::TYPE::OBSERVER ) },
		{ IDENTIFIER::BUILTIN::TYPE,      type_id( HHuginn::TYPE::FUNCTION_REFERENCE ) }
	};
	fun_to_type_t::const_iterator it( funToType.find( identifierId_ ) );
	if ( it != funToType.end() ) {
		t = it->second;
	}
	return ( type_id_to_class( t ) );
}

HHuginn::HHuginn::HClass const* OCompiler::congruent( HHuginn::HClass const* c1_, HHuginn::HClass const* c2_ ) const {
	HHuginn::HClass const* c( type_to_class( HHuginn::TYPE::NOT_BOOLEAN ) );
	if ( c1_ == c2_ ) {
		HHuginn::type_id_t t1( c1_ ? c1_->type_id() : type_id( HHuginn::TYPE::UNKNOWN ) );
		if ( ( t1 != HHuginn::TYPE::UNKNOWN ) && ( t1 != HHuginn::TYPE::REFERENCE ) ) {
			c = c1_;
		}
	} else if ( ! ( is_unknown( c1_ ) && is_unknown( c2_ ) ) ) {
		if ( is_unknown( c1_ ) ) {
			c = c2_;
		} else {
			c = c1_;
		}
	}
	return ( c );
}

}

}

}

