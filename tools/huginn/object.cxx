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

namespace hobject_destructor_helper {

void deleter( HHuginn::HValue* );
void deleter( HHuginn::HValue* ) {
}

template<typename T>
struct allocator {
	template<typename U>
	struct rebind {
		typedef allocator<U> other;
	};
	T* _mem;
	allocator( T* mem_ )
		: _mem( mem_ ) {
	}
	template<typename U>
	allocator( allocator<U> const& other_ )
		: _mem( reinterpret_cast<T*>( const_cast<U*>( other_._mem ) ) ) {
	}
	allocator( allocator const& ) = default;
	allocator& operator = ( allocator const& ) = default;
	T* allocate( int ) {
		return ( _mem );
	}
	void deallocate( T*, int ) {
	}
};

typedef yaal::hcore::HSharedDeleterAllocator<HHuginn::HValue, void(*)( HHuginn::HValue*), allocator<HHuginn::HObject>> holder_t;

static int const BUFFER_SIZE = sizeof ( holder_t );
typedef typename memory::aligned<BUFFER_SIZE, holder_t>::type buffer_t;

}

HHuginn::HObject::HObject( HClass const* class_, fields_t const& fields_ )
	: HValue( class_ )
	, _fields( fields_ ) {
}

HHuginn::HObject::~HObject( void ) {
	M_PROLOG
	HHuginn::HClass const* clss( get_class() );
	HRuntime* runtime( clss->runtime() );
	huginn::HThread* t( runtime ? runtime->current_thread() : nullptr );
	if ( t && ! t->has_runtime_exception() ) {
		hobject_destructor_helper::buffer_t buffer;
		hobject_destructor_helper::allocator<hobject_destructor_helper::holder_t> allocator( buffer.mem() );
		HHuginn::value_t nonOwning( static_cast<HHuginn::HValue*>( this ), &hobject_destructor_helper::deleter, allocator );
		int destructorIdx( clss->field_index( KEYWORD::DESTRUCTOR_IDENTIFIER ) );
		HClass const* c( clss->super() );
		if ( destructorIdx >= 0 ) {
			try {
				M_ASSERT( _fields[destructorIdx]->type_id() == HHuginn::TYPE::METHOD );
				static_cast<HClass::HMethod const*>(
					_fields[destructorIdx].raw()
				)->function()( t, &nonOwning, HArguments( t ), 0 );
			} catch ( HHuginnRuntimeException const& e ) {
				t->break_execution( HFrame::STATE::RUNTIME_EXCEPTION );
				t->set_exception( e.message(), e.file_id(), e.position() );
				c = nullptr;
			}
		}
		while ( c ) {
			destructorIdx = c->field_index( KEYWORD::DESTRUCTOR_IDENTIFIER );
			if ( destructorIdx >= 0 ) {
				try {
					c->function( destructorIdx )( t, &nonOwning, HArguments( t ), 0 );
				} catch ( HHuginnRuntimeException const& e ) {
					t->break_execution( HFrame::STATE::RUNTIME_EXCEPTION );
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

HHuginn::value_t HHuginn::HObject::init_base( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( object_->raw() ) );
	HHuginn::HClass const* super( o->get_class() );
	while ( super->type() != HHuginn::HClass::TYPE::BUILTIN ) {
		super = super->super();
	}
	o->_fields.push_back(
		super->create_instance( thread_, object_, values_, position_ )
	);
	for ( int i( 0 ), COUNT( static_cast<int>( super->field_identifiers().get_size() ) ); i < COUNT; ++ i ) {
		HHuginn::HValue const& v( *super->field( i ) );
		o->_fields[i] = ( v.type_id() == HHuginn::TYPE::METHOD )
			? thread_->runtime().object_factory()->create_method(
				&HHuginn::HObject::forward_call,
				static_cast<HHuginn::HClass::HMethod const&>( v ).function()
			)
			: super->get_default( thread_, i, position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObject::forward_call( HHuginn::function_t func_, huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	HObject* o( static_cast<HObject*>( object_->raw() ) );
	return ( func_( thread_, &o->_fields.back(), values_, position_ ) );
	M_EPILOG
}

HHuginn::value_t const& HHuginn::HObject::do_field( int index_ ) const {
	M_PROLOG
	return ( _fields[index_] );
	M_EPILOG
}

HHuginn::value_t& HHuginn::HObject::field_ref( int index_ ) {
	M_PROLOG
	return ( _fields[index_] );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObject::do_field( HHuginn::value_t const& object_, int index_ ) const {
	M_PROLOG
	value_t const& f( _fields[index_] );
	bool isMethod( f->type_id() == TYPE::METHOD );
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

HHuginn::value_t HHuginn::HObject::get_method(
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

HHuginn::function_t const& HHuginn::HObject::get_method(
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
				.append( "' does not have `" )
				.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
				.append( "' method." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	HHuginn::value_t const& f( _fields[ idx ] );
	if ( f->type_id() != HHuginn::TYPE::METHOD ) {
		throw HHuginn::HHuginnRuntimeException(
			"`"_ys
				.append( thread_->runtime().identifier_name( methodIdentifier_ ) )
				.append( "' in class `" )
				.append( get_class()->name() )
				.append( "' is not a method." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( static_cast<HHuginn::HClass::HMethod const*>( f.raw() )->function() );
	M_EPILOG
}

HHuginn::value_t HHuginn::HObject::call_method(
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

HHuginn::value_t HHuginn::HObject::do_clone( huginn::HThread* thread_, HHuginn::value_t* object_, int position_ ) const {
	M_PROLOG
	int idx( field_index( INTERFACE::CLONE_IDENTIFIER ) );
	HHuginn::value_t copy;
	if ( idx >= 0 ) {
		HHuginn::value_t const& cloneMember( _fields[ idx ] );
		if ( cloneMember->type_id() != HHuginn::TYPE::METHOD ) {
			throw HHuginn::HHuginnRuntimeException(
				"`clone' in class `"_ys
					.append( get_class()->name() )
					.append( "' is not a method." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		HHuginn::HClass::HMethod const* m( static_cast<HHuginn::HClass::HMethod const*>( cloneMember.raw() ) );
		copy = m->function()( thread_, object_, HArguments( thread_ ), position_ );
	} else {
		values_t fields;
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

