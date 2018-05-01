/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "dict.hxx"
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

namespace dict {

class HDictIterator : public HNotifableIterator {
public:
	enum class TYPE {
		KEYS,
		KEY_VALUES
	};
	typedef HHuginn::value_t (HDictIterator::*value_getter_t)( void );
private:
	HHuginn::HDict::values_t* _dict;
	HHuginn::HDict::values_t::iterator _it;
	value_getter_t _valueGetter;
	HObjectFactory& _objectFactory;
public:
	HDictIterator( HHuginn::HDict* owner_, HObjectFactory& objectFactory_, TYPE type_ )
		: HNotifableIterator( owner_ )
		, _dict( &owner_->value() )
		, _it( _dict->begin() )
		, _valueGetter( type_ == TYPE::KEYS ? &HDictIterator::get_key : &HDictIterator::get_key_value )
		, _objectFactory( objectFactory_ ) {
		return;
	}
protected:
	HHuginn::value_t get_key( void ) {
		return ( _it->first );
	}
	HHuginn::value_t get_key_value( void ) {
		return ( _objectFactory.create_tuple( { _it->first, _it->second } ) );
	}
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( (this->*_valueGetter)() );
	}
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _it != _dict->end() );
	}
	virtual void do_next( HThread*, int ) override {
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _dict->end();
	}
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_node_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HDictIterator( HDictIterator const& ) = delete;
	HDictIterator& operator = ( HDictIterator const& ) = delete;
};

class HKeyValuesDictView : public HHuginn::HIterable {
	HHuginn::value_t _dict;
public:
	HKeyValuesDictView( HHuginn::HClass const* class_, HHuginn::value_t const& dict_ )
		: HIterable( class_ )
		, _dict( dict_ ) {
		M_ASSERT( _dict->type_id() == HHuginn::TYPE::DICT );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				runtime_,
				"KeyValuesDictView",
				"The `KeyValuesDictView` class represents *lazy* *iterable* view of a `dict` consisted of key-value pairs."
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HDict const*>( _dict.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override {
		HIterator::iterator_implementation_t impl(
			new ( memory::yaal ) HDictIterator(
				static_cast<HHuginn::HDict*>( _dict.raw() ),
				*_dict->get_class()->runtime()->object_factory(),
				HDictIterator::TYPE::KEY_VALUES
			)
		);
		return ( HIterator( yaal::move( impl ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HKeyValuesDictView>( HIterable::get_class(), _dict ) );
	}
};

class HDictReverseIterator : public HNotifableIterator {
	HHuginn::HDict::values_t* _dict;
	HHuginn::HDict::values_t::reverse_iterator _it;
public:
	HDictReverseIterator( HHuginn::HDict* owner_ )
		: HNotifableIterator( owner_ )
		, _dict( &owner_->value() )
		, _it( _dict->rbegin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _it != _dict->rend() );
	}
	virtual void do_next( HThread*, int ) override {
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _dict->rend();
	}
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_node_id( void ) const override {
		return ( _it.base().node_id() );
	}
private:
	HDictReverseIterator( HDictReverseIterator const& ) = delete;
	HDictReverseIterator& operator = ( HDictReverseIterator const& ) = delete;
};

class HReversedDict : public HHuginn::HIterable {
	HHuginn::value_t _dict;
public:
	HReversedDict( HHuginn::HClass const* class_, HHuginn::value_t const& dict_ )
		: HIterable( class_ )
		, _dict( dict_ ) {
		M_ASSERT( _dict->type_id() == HHuginn::TYPE::DICT );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				runtime_,
				"ReversedDictView",
				"The `ReversedDictView` class represents *lazy* *iterable* reversed view of a `dict`."
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HDict const*>( _dict.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override {
		HIterator::iterator_implementation_t impl( new ( memory::yaal ) HDictReverseIterator( static_cast<HHuginn::HDict*>( _dict.raw() ) ) );
		return ( HIterator( yaal::move( impl ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedDict>( HIterable::get_class(), _dict ) );
	}
};

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.has_key", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	bool hasKey( static_cast<HHuginn::HDict*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.get", values_, 1, 2, thread_, position_ );
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

inline HHuginn::value_t ensure( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.ensure", values_, 2, 2, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	HHuginn::HDict* l( static_cast<HHuginn::HDict*>( object_->raw() ) );
	HHuginn::value_t& v( l->get_ref( thread_, values_[0], position_ ) );
	if ( ! v ) {
		v = values_[1];
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.erase", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	static_cast<HHuginn::HDict*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	static_cast<HHuginn::HDict*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.update", values_, { HHuginn::TYPE::DICT }, thread_, position_ );
	HHuginn::HDict& l( *static_cast<HHuginn::HDict*>( object_->raw() ) );
	HHuginn::HDict const& r( *static_cast<HHuginn::HDict const*>( values_[0].raw() ) );
	if ( r.key_type()->type_id() != HHuginn::TYPE::NONE ) {
		l.verify_key_type( thread_, r.key_type(), position_ );
	}
	HHuginn::HDict::values_t& lv( l.value() );
	HHuginn::HDict::values_t const& rv( r.value() );
	HAnchorGuard<HHuginn::HDict> ag( l, thread_, position_ );
	for ( HHuginn::HDict::values_t::const_iterator it( rv.begin() ), end( rv.end() ); it != end; ++ it ) {
		lv.insert( *it );
	}
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	HHuginn::HDict::values_t const& values( static_cast<HHuginn::HDict*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::DICT ) );
	for ( HHuginn::HDict::values_t::value_type const& v : values ) {
		hashValue *= 3;
		hashValue += value_builtin::hash( thread_, v.first, position_ );
		hashValue *= 3;
		hashValue += value_builtin::hash( thread_, v.second, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.equals", values_, { HHuginn::TYPE::DICT }, thread_, position_ );
	HHuginn::HDict::values_t const& l( static_cast<HHuginn::HDict*>( object_->raw() )->value() );
	HHuginn::HDict::values_t const& r( static_cast<HHuginn::HDict const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( HHuginn::HDict::values_t::const_iterator lit( l.begin() ), rit( r.begin() ), end( l.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		equal = value_builtin::equals( thread_, lit->first, rit->first, position_ ) && value_builtin::equals( thread_, lit->second, rit->second, position_ );
	}
	return ( thread_->object_factory().create_boolean( equal ) );
	M_EPILOG
}

inline HHuginn::value_t values( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.values", values_, {}, thread_, position_ );
	return ( dict::key_values_view( thread_, *object_ ) );
	M_EPILOG
}

class HDictClass : public HHuginn::HClass {
public:
	typedef HDictClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _keyValuesDictViewClass;
	HHuginn::class_t _reversedDictClass;
public:
	HDictClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_,
		HHuginn::type_id_t typeId_,
		HHuginn::identifier_id_t identifierId_,
		yaal::hcore::HString const& doc_
	) : HHuginn::HClass(
			runtime_,
			objectFactory_,
			typeId_,
			identifierId_,
			doc_,
			&huginn_builtin::dict
		)
		, _keyValuesDictViewClass( HKeyValuesDictView::get_class( runtime_ ) )
		, _reversedDictClass( HReversedDict::get_class( runtime_ ) ) {
		return;
	}
	HHuginn::HClass const* key_values_dict_view_class( void ) const {
		return ( _keyValuesDictViewClass.raw() );
	}
	HHuginn::HClass const* reversed_dict_class( void ) const {
		return ( _reversedDictClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _keyValuesDictViewClass );
		runtime_->huginn()->register_class( _reversedDictClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HDictClass>(
			runtime_,
			objectFactory_,
			type_id( HHuginn::TYPE::DICT ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::DICT ) ),
			"The `dict` is a collection providing a sorted key to value map. It supports operations of iteration, key-value insertion, key removal and key search. The keys stored in given `dict` instance must be of uniform type."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "has_key", objectFactory_->create_method( &dict::has_key ), "( *key* ) - tell if given *key* can be found in this `dict`" },
		{ "get",     objectFactory_->create_method( &dict::get ),     "( *key*, *default* ) - get value for given *key* from this `dict`, or *default* if given *key* is not present in the `dict`" },
		{ "erase",   objectFactory_->create_method( &dict::erase ),   "( *key* ) - remove given *key* from this `dict`" },
		{ "ensure",  objectFactory_->create_method( &dict::ensure ),  "( *key*, *default* ) - get value for given *key* from this `dict`, if given *key* is not present in the `dict` insert *default* into this `dict` before returning it" },
		{ "clear",   objectFactory_->create_method( &dict::clear ),   "erase `dict`'s content, `dict` becomes empty" },
		{ "add",     objectFactory_->create_method( &dict::update ),  "( *other* ) - update content of this `dict` with key/value pairs from *other* `dict`" },
		{ "update",  objectFactory_->create_method( &dict::update ),  "( *other* ) - update content of this `dict` with key/value pairs from *other* `dict`" },
		{ "hash",    objectFactory_->create_method( &dict::hash ),    "calculate hash value for this `dict`" },
		{ "equals",  objectFactory_->create_method( &dict::equals ),  "( *other* ) - test if *other* `dict` has the same content" },
		{ "values",  objectFactory_->create_method( &dict::values ),  "get key-value pairs view of this `dict`" }
	};
	c->redefine( nullptr, fd );
	return ( c );
	M_EPILOG
}

HHuginn::value_t key_values_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::DICT );
	HDictClass const* dc( static_cast<HDictClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HKeyValuesDictView>( dc->key_values_dict_view_class(), value_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::DICT );
	HDictClass const* dc( static_cast<HDictClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedDict>( dc->reversed_dict_class(), value_ ) );
	M_EPILOG
}

}

}

HHuginn::HDict::HDict( HHuginn::HClass const* class_, allocator_t const& allocator_ )
	: HInvalidatingIterable( class_ )
	, _helper( &value_builtin::less )
	, _data( _helper, allocator_ )
	, _keyType( nullptr ) {
	return;
}

int long HHuginn::HDict::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HHuginn::HDict::verify_key_type( huginn::HThread* thread_, HHuginn::HClass const* keyType_, int position_ ) const {
	if ( _keyType && ( keyType_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ), thread_->current_frame()->file_id(), position_ );
	}
	if ( ! is_comparable( keyType_ ) ) {
		throw HHuginnRuntimeException(
			"Key type `"_ys.append( keyType_->name() ).append( "' is not a comparable." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return;
}

HHuginn::value_t HHuginn::HDict::get( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HHuginn::HDict> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in `dict'.", thread_->current_frame()->file_id(), position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HDict::try_get( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HHuginn::HDict> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
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
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HHuginn::HDict> ag( *this, thread_, position_ );
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HDict::erase( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HHuginn::HDict> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( it != _data.end() ) {
		invalidate( it.node_id() );
		_data.erase( it );
	}
	return;
	M_EPILOG
}

HHuginn::value_t& HHuginn::HDict::get_ref( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	_keyType = key_->get_class();
	HAnchorGuard<HHuginn::HDict> ag( *this, thread_, position_ );
	return ( _data[key_] );
	M_EPILOG
}

void HHuginn::HDict::insert( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HHuginn::HDict> ag( *this, thread_, position_ );
	_data.insert( make_pair( key_, value_ ) );
	_keyType = key_->get_class();
	return;
	M_EPILOG
}

HHuginn::HClass const* HHuginn::HDict::key_type( void ) const {
	return ( _keyType );
}

void HHuginn::HDict::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HDict::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl(
		new ( memory::yaal ) huginn::dict::HDictIterator(
			this,
			*get_class()->runtime()->object_factory(),
			huginn::dict::HDictIterator::TYPE::KEYS
		)
	);
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HDict::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_dict() );
	HDict* dict( static_cast<HDict*>( res.raw() ) );
	dict->_keyType = _keyType;
	values_t& data( dict->value() );
	HAnchorGuard<HHuginn::HDict> ag( *dict, thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert(
			data.end(),
			make_pair(
				v.first->clone( thread_, const_cast<HHuginn::value_t*>( &v.first ), position_ ),
				v.second->clone( thread_, const_cast<HHuginn::value_t*>( &v.second ), position_ )
			)
		);
	}
	return ( res );
}

}

}

