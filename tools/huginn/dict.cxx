/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  dict.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "iterator.hxx"
#include "compiler.hxx"
#include "thread.hxx"
#include "value_builtin.hxx"
#include "helper.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HDictIterator : public HIteratorInterface {
	HHuginn::HDict::values_t* _dict;
	HHuginn::HDict::values_t::iterator _it;
public:
	HDictIterator( HHuginn::HDict::values_t* dict_ )
		: _dict( dict_ ), _it( dict_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _dict->end() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HDictIterator( HDictIterator const& ) = delete;
	HDictIterator& operator = ( HDictIterator const& ) = delete;
};

namespace dict {

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.has_key", values_, 1, 1, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	bool hasKey( static_cast<HHuginn::HDict*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.get", values_, 1, 2, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	HHuginn::HDict* d( static_cast<HHuginn::HDict*>( object_->raw() ) );
	HHuginn::value_t v;
	if ( values_.get_size() > 1 ) {
		bool hasKey( d->try_get( thread_, values_[0], v, position_ ) );
		if ( ! hasKey ) {
			v = values_[1];
		}
	} else {
		v = d->get( thread_, values_[0], position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.erase", values_, 1, 1, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	static_cast<HHuginn::HDict*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.clear", values_, 0, 0, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	static_cast<HHuginn::HDict*>( object_->raw() )->value().clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.update", values_, { HHuginn::TYPE::DICT }, position_ );
	HHuginn::HDict& l( *static_cast<HHuginn::HDict*>( object_->raw() ) );
	HHuginn::HDict const& r( *static_cast<HHuginn::HDict const*>( values_[0].raw() ) );
	if ( r.key_type()->type_id() != HHuginn::TYPE::NONE ) {
		l.verify_key_type( r.key_type(), position_ );
	}
	HHuginn::HDict::values_t& lv( l.value() );
	HHuginn::HDict::values_t const& rv( r.value() );
	l.anchor( thread_, position_ );
	for ( HHuginn::HDict::values_t::const_iterator it( rv.begin() ), end( rv.end() ); it != end; ++ it ) {
		lv.insert( *it );
	}
	l.detach();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.equals", values_, { HHuginn::TYPE::DICT }, position_ );
	HHuginn::HDict::values_t const& l( static_cast<HHuginn::HDict*>( object_->raw() )->value() );
	HHuginn::HDict::values_t const& r( static_cast<HHuginn::HDict const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( HHuginn::HDict::values_t::const_iterator lit( l.begin() ), rit( r.begin() ), end( l.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		equal = value_builtin::equals( thread_, lit->first, rit->first, position_ ) && value_builtin::equals( thread_, lit->second, rit->second, position_ );
	}
	return ( thread_->object_factory().create_boolean( equal ) );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime* );
HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::DICT ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::DICT ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "has_key", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::has_key, _1, _2, _3, _4 ) ), "( *key* ) - tell if given *key* can be found in this `dict`" },
				{ "get",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::get, _1, _2, _3, _4 ) ),     "( *key*, *default* ) - get value for given *key* from this `dict`, or *default* if given *key* is not present in the `dict`" },
				{ "erase",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::erase, _1, _2, _3, _4 ) ),   "( *key* ) - remove given *key* from this `dict`" },
				{ "clear",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::clear, _1, _2, _3, _4 ) ),   "erase `dict`'s content, `dict` becomes empty" },
				{ "update",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::update, _1, _2, _3, _4 ) ),  "( *other* ) - update content of this `dict` with key/value pairs from *other* `dict`" },
				{ "equals",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::equals, _1, _2, _3, _4 ) ),  "( *other* ) - test if *other* `dict` has the same content" }
			},
			"The `dict` is a collection providing a sorted key to value map. It supports operations of iteration, key-value insertion, key removal and key search. The keys stored in given `dict` instance must be of uniform type."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HDict::HDict( HHuginn::HClass const* class_, allocator_t const& allocator_ )
	: HIterable( class_ )
	, _helper()
	, _data( _helper, allocator_ )
	, _keyType( &huginn::_noneClass_ ) {
	return;
}

int long HHuginn::HDict::do_size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HDict::verify_key_type( HHuginn::HClass const* keyType_, int position_ ) const {
	if ( ( _keyType->type_id() != TYPE::NONE ) && ( keyType_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ), position_ );
	}
	if ( ! OCompiler::is_comparable( keyType_->type_id() ) ) {
		throw HHuginnRuntimeException( "Key type `"_ys.append( keyType_->name() ).append( "' is not a comparable." ), position_ );
	}
	return;
}

HHuginn::value_t HHuginn::HDict::get( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	_helper.detach();
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in `dict'.", position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HDict::try_get( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	_helper.detach();
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return ( found );
	M_EPILOG
}

bool HHuginn::HDict::has_key( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	bool has( _data.find( key_ ) != _data.end() );
	_helper.detach();
	return ( has );
	M_EPILOG
}

void HHuginn::HDict::erase( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	_data.erase( key_ );
	_helper.detach();
	return;
	M_EPILOG
}

HHuginn::value_t& HHuginn::HDict::get_ref( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_keyType = key_->get_class();
	_helper.anchor( thread_, position_ );
	HHuginn::value_t& ref( _data[key_] );
	_helper.detach();
	return ( ref );
	M_EPILOG
}

void HHuginn::HDict::insert( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_helper.anchor( thread_, position_ );
	_data.insert( make_pair( key_, value_ ) );
	_helper.detach();
	_keyType = key_->get_class();
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HDict::key_type( void ) const {
	return ( _keyType );
}

HHuginn::HIterable::HIterator HHuginn::HDict::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HDictIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HDict::do_clone( huginn::HThread* thread_, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_dict() );
	HDict* dict( static_cast<HDict*>( res.raw() ) );
	dict->_keyType = _keyType;
	values_t& data( dict->value() );
	dict->anchor( thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert( data.end(), make_pair( v.first->clone( thread_, position_ ), v.second->clone( thread_, position_ ) ) );
	}
	dict->detach();
	return ( res );
}

}

}

