/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "keyword.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

HObject::HObject( HClass const* class_, fields_t const& fields_ )
	: HValue( class_ )
	, _fields( fields_ ) {
}

void HObject::do_destroy( HHuginn::value_t* object_ ) {
	M_PROLOG
	huginn::HClass const* clss( get_class() );
	HRuntime* runtime( clss->runtime() );
	huginn::HThread* t( runtime ? runtime->current_thread() : nullptr );
	if ( t && ! t->has_runtime_exception() ) {
		int destructorIdx( clss->field_index( IDENTIFIER::KEYWORD::DESTRUCTOR ) );
		HClass const* c( clss->super() );
		if ( destructorIdx >= 0 ) {
			try {
				M_ASSERT( _fields[destructorIdx]->type_id() == HHuginn::TYPE::METHOD );
				static_cast<HClass::HMethod const*>(
					_fields[destructorIdx].raw()
				)->function()( t, object_, HArguments( t ), 0 );
			} catch ( HHuginn::HHuginnRuntimeException const& e ) {
				t->state_set( HThread::STATE::RUNTIME_EXCEPTION );
				t->set_exception( e.message(), e.file_id(), e.position() );
				c = nullptr;
			}
		}
		while ( c ) {
			destructorIdx = c->field_index( IDENTIFIER::KEYWORD::DESTRUCTOR );
			if ( destructorIdx >= 0 ) {
				try {
					c->function( destructorIdx )( t, object_, HArguments( t ), 0 );
				} catch ( HHuginn::HHuginnRuntimeException const& e ) {
					t->state_set( HThread::STATE::RUNTIME_EXCEPTION );
					t->set_exception( e.message(), e.file_id(), e.position() );
					break;
				}
			}
			c = c->super();
		}
	}
	return;
	M_DESTRUCTOR_EPILOG
}

HHuginn::value_t HObject::init_base( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( object_->raw() ) );
	huginn::HClass const* super( o->get_class() );
	while ( super->type() != huginn::HClass::TYPE::BUILTIN ) {
		super = super->super();
	}
	o->_fields.push_back(
		super->create_instance( thread_, object_, values_, position_ )
	);
	for ( int i( 0 ), COUNT( static_cast<int>( super->field_identifiers().get_size() ) ); i < COUNT; ++ i ) {
		huginn::HValue const& v( *super->field( i ) );
		o->_fields[i] = ( v.type_id() == HHuginn::TYPE::METHOD )
			? thread_->runtime().object_factory()->create_method(
				&HObject::forward_call,
				static_cast<huginn::HClass::HMethod const&>( v ).function()
			)
			: super->get_default( thread_, i, position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HObject::forward_call( HHuginn::function_t func_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( object_->raw() ) );
	return ( func_( thread_, &o->_fields.back(), values_, position_ ) );
	M_EPILOG
}

HHuginn::value_t const& HObject::do_field( int index_ ) const {
	M_PROLOG
	return ( _fields[index_] );
	M_EPILOG
}

HHuginn::value_t& HObject::field_ref( int index_ ) {
	M_PROLOG
	return ( _fields[index_] );
	M_EPILOG
}

HHuginn::value_t HObject::do_field( HHuginn::value_t const& object_, int index_ ) const {
	M_PROLOG
	value_t const& f( _fields[index_] );
	bool isMethod( f->type_id() == HHuginn::TYPE::METHOD );
	if ( isMethod ) {
		return (
			get_class()->runtime()->object_factory()->create_bound_method(
				static_cast<HClass::HMethod const*>( f.raw() )->function(),
				object_
			)
		);
	}
	return ( f );
	M_EPILOG
}

HHuginn::value_t HObject::get_method(
	huginn::HThread* thread_,
	HHuginn::value_t const& object_,
	HHuginn::identifier_id_t const& methodIdentifier_,
	int position_
) const {
	M_PROLOG
	M_ASSERT( object_.raw() == this );
	return (
		get_class()->runtime()->object_factory()->create_bound_method(
			get_method( thread_, methodIdentifier_, position_ ),
			object_
		)
	);
	M_EPILOG
}

HHuginn::function_t const& HObject::get_method(
	huginn::HThread* thread_,
	HHuginn::identifier_id_t const& methodIdentifier_,
	int position_
) const {
	M_PROLOG
	int idx( field_index( methodIdentifier_ ) );
	if ( idx < 0 ) {
		throw HHuginn::HHuginnRuntimeException(
			"Class `"_ys
				.append( get_class()->name() )
				.append( "` does not have `" )
				.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
				.append( "` method." ),
			thread_->file_id(),
			position_
		);
	}
	HHuginn::value_t const& f( _fields[ idx ] );
	if ( f->type_id() != HHuginn::TYPE::METHOD ) {
		throw HHuginn::HHuginnRuntimeException(
			"`"_ys
				.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
				.append( "` in class `" )
				.append( get_class()->name() )
				.append( "` is not a method." ),
			thread_->file_id(),
			position_
		);
	}
	return ( static_cast<huginn::HClass::HMethod const*>( f.raw() )->function() );
	M_EPILOG
}

HHuginn::value_t HObject::call_method(
	huginn::HThread* thread_,
	HHuginn::value_t const& object_,
	HHuginn::identifier_id_t const& methodIdentifier_,
	HHuginn::values_t& arguments_,
	int position_
) const {
	M_PROLOG
	HHuginn::function_t const& f( get_method( thread_, methodIdentifier_, position_ ) );
	return ( f( thread_, const_cast<HHuginn::value_t*>( &object_ ), arguments_, position_ ) );
	M_EPILOG
}

HHuginn::value_t HObject::do_clone( huginn::HThread* thread_, HHuginn::value_t* object_, int position_ ) const {
	M_PROLOG
	int idx( field_index( IDENTIFIER::INTERFACE::CLONE ) );
	HHuginn::value_t copy;
	if ( idx >= 0 ) {
		HHuginn::value_t const& cloneMember( _fields[ idx ] );
		if ( cloneMember->type_id() != HHuginn::TYPE::METHOD ) {
			throw HHuginn::HHuginnRuntimeException(
				"`clone` in class "_ys
					.append( a_type_name( get_class() ) )
					.append( " is not a method." ),
				thread_->file_id(),
				position_
			);
		}
		huginn::HClass::HMethod const* m( static_cast<huginn::HClass::HMethod const*>( cloneMember.raw() ) );
		copy = m->function()( thread_, object_, HArguments( thread_ ), position_ );
		if ( copy->get_class() != get_class() ) {
			throw HHuginn::HHuginnRuntimeException(
				"`clone` in class "_ys
					.append( a_type_name( get_class() ) )
					.append( " returned object of an invalid type: " )
					.append( a_type_name( copy->get_class() ) )
					.append( "." ),
				thread_->file_id(),
				position_
			);
		}
	} else {
		HHuginn::values_t fields;
		for ( value_t const& v : _fields ) {
			fields.push_back( v->clone( thread_, const_cast<HHuginn::value_t*>( &v ), position_ ) );
		}
		copy = thread_->runtime().object_factory()->create_object( get_class(), fields );
	}
	return ( copy );
	M_EPILOG
}

}

}

