/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "dict.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "iterator.hxx"
#include "compiler.hxx"
#include "thread.hxx"
#include "instruction.hxx"
#include "helper.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace dict {

class HDictIterator : public HSkippingIterator {
public:
	enum class TYPE {
		KEYS,
		KEY_VALUES
	};
	typedef HHuginn::value_t (HDictIterator::*value_getter_t)( void );
private:
	huginn::HDict::values_t* _dict;
	huginn::HDict::values_t::iterator _it;
	value_getter_t _valueGetter;
	HObjectFactory& _objectFactory;
public:
	HDictIterator( huginn::HDict* owner_, HObjectFactory& objectFactory_, TYPE type_ )
		: HSkippingIterator( owner_ )
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
	virtual void do_skip( HThread*, int ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HDictIterator( HDictIterator const& ) = delete;
	HDictIterator& operator = ( HDictIterator const& ) = delete;
};

class HKeyValuesDictView : public huginn::HIterable {
	HHuginn::value_t _dict;
public:
	HKeyValuesDictView( HClass const* class_, HHuginn::value_t const& dict_ )
		: HIterable( class_ )
		, _dict( dict_ ) {
		M_ASSERT( _dict->type_id() == HHuginn::TYPE::DICT );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				objectFactory_,
				"KeyValuesDictView",
				"The `KeyValuesDictView` class represents *lazy* *iterable* view of a `dict` consisted of key-value pairs.",
				origin_
			)
		);
		return c;
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<huginn::HDict const*>( _dict.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override {
		return (
			make_pointer<HDictIterator>(
				static_cast<huginn::HDict*>( _dict.raw() ),
				*_dict->get_class()->runtime()->object_factory(),
				HDictIterator::TYPE::KEY_VALUES
			)
		);
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HKeyValuesDictView>( HIterable::get_class(), _dict ) );
	}
};

class HDictReverseIterator : public HSkippingIterator {
	huginn::HDict::values_t* _dict;
	huginn::HDict::values_t::reverse_iterator _it;
public:
	HDictReverseIterator( huginn::HDict* owner_ )
		: HSkippingIterator( owner_ )
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
	virtual void do_skip( HThread*, int ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.raw().node_id() );
	}
private:
	HDictReverseIterator( HDictReverseIterator const& ) = delete;
	HDictReverseIterator& operator = ( HDictReverseIterator const& ) = delete;
};

class HReversedDict : public huginn::HIterable {
	HHuginn::value_t _dict;
public:
	HReversedDict( HClass const* class_, HHuginn::value_t const& dict_ )
		: HIterable( class_ )
		, _dict( dict_ ) {
		M_ASSERT( _dict->type_id() == HHuginn::TYPE::DICT );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				objectFactory_,
				"ReversedDictView",
				"The `ReversedDictView` class represents *lazy* *iterable* reversed view of a `dict`.",
				origin_
			)
		);
		return c;
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<huginn::HDict const*>( _dict.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override {
		return ( make_pointer<HDictReverseIterator>( static_cast<huginn::HDict*>( _dict.raw() ) ) );
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
	bool hasKey( static_cast<huginn::HDict*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->runtime().boolean_value( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.get", values_, 1, 2, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	huginn::HDict* d( static_cast<huginn::HDict*>( object_->raw() ) );
	HHuginn::value_t v;
	if ( values_.get_size() > 1 ) {
		bool hasKey( d->try_get( thread_, values_[0], v, position_ ) );
		if ( ! hasKey ) {
			v = values_[1];
		}
	} else {
		v = d->get( thread_, values_[0], position_ );
	}
	return v;
	M_EPILOG
}

inline HHuginn::value_t ensure( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.ensure", values_, 2, 2, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	huginn::HDict& d( *static_cast<huginn::HDict*>( object_->raw() ) );
	HHuginn::value_t const& key( values_[0] );
	d.update_key_type( thread_, key->get_class(), position_ );
	HAnchorGuard<HDict> ag( d, thread_, position_ );
	return ( d.value().insert( make_pair( key, values_[1] ) ).first->second );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.erase", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	static_cast<huginn::HDict*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	static_cast<huginn::HDict*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.update", values_, { HHuginn::TYPE::DICT }, thread_, position_ );
	(*object_)->operator_add( thread_, *object_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t values( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::DICT );
	verify_signature( "dict.values", values_, {}, thread_, position_ );
	return ( dict::key_values_view( thread_, *object_ ) );
	M_EPILOG
}

class HDictClass : public HClass {
public:
	typedef HDictClass this_type;
	typedef HClass base_type;
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
	) : HClass(
			runtime_,
			objectFactory_,
			typeId_,
			identifierId_,
			doc_,
			&builtin::dict
		)
		, _keyValuesDictViewClass()
		, _reversedDictClass() {
		HHuginn::field_definitions_t fd{
			{ "has_key", objectFactory_->create_method( &dict::has_key ), "( *key* ) - tell if given *key* can be found in this `dict`" },
			{ "get",     objectFactory_->create_method( &dict::get ),     "( *key*, *default* ) - get value for given *key* from this `dict`, or *default* if given *key* is not present in the `dict`" },
			{ "erase",   objectFactory_->create_method( &dict::erase ),   "( *key* ) - remove given *key* from this `dict`" },
			{ "ensure",  objectFactory_->create_method( &dict::ensure ),  "( *key*, *default* ) - get value for given *key* from this `dict`, if given *key* is not present in the `dict` insert *default* into this `dict` before returning it" },
			{ "clear",   objectFactory_->create_method( &dict::clear ),   "erase `dict`'s content, `dict` becomes empty" },
			{ "update",  objectFactory_->create_method( &dict::update ),  "( *other* ) - update content of this `dict` with key/value pairs from *other* `dict`" },
			{ "values",  objectFactory_->create_method( &dict::values ),  "get key-value pairs view of this `dict`" }
		};
		redefine( nullptr, fd );
		_keyValuesDictViewClass = add_class_as_member( this, HKeyValuesDictView::get_class( runtime_, objectFactory_, this ), HClass::MEMBER_TYPE::STATIC );
		_reversedDictClass = add_class_as_member( this, HReversedDict::get_class( runtime_, objectFactory_, this ), HClass::MEMBER_TYPE::STATIC );
		return;
	}
	HClass const* key_values_dict_view_class( void ) const {
		return ( _keyValuesDictViewClass.raw() );
	}
	HClass const* reversed_dict_class( void ) const {
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
	return c;
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

HDict::HDict( HClass const* class_, allocator_t const& allocator_ )
	: HInvalidatingIterable( class_ )
	, _helper( &instruction::less )
	, _data( _helper, allocator_ )
	, _keyType( nullptr ) {
	return;
}

int long HDict::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

void HDict::verify_key_type( huginn::HThread* thread_, HClass const* keyType_, int position_ ) const {
	M_ASSERT( keyType_ );
	if ( _keyType && ( keyType_ != _keyType ) ) {
		throw HHuginn::HHuginnRuntimeException( "Non-uniform key types, got "_ys.append( a_type_name( keyType_ ) ).append( " instead of " ).append( a_type_name( _keyType ) ).append( "." ), thread_->file_id(), position_ );
	}
	return;
}

void HDict::update_key_type( huginn::HThread* thread_, HClass const* keyType_, int position_ ) {
	verify_key_type( thread_, keyType_, position_ );
	_keyType = keyType_;
	return;
}

HHuginn::value_t HDict::get( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HDict> ag( *this, thread_, position_ );
	values_t::const_iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginn::HHuginnRuntimeException( "Key does not exist in `dict`.", thread_->file_id(), position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HDict::try_get( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HDict> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return found;
	M_EPILOG
}

bool HDict::has_key( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HDict> ag( *this, thread_, position_ );
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HDict::erase( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HDict> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( it != _data.end() ) {
		skip( thread_, it.node_id(), position_ );
		_data.erase( it );
	}
	return;
	M_EPILOG
}

void HDict::insert( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	update_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HDict> ag( *this, thread_, position_ );
	_data.insert( make_pair( key_, value_ ) );
	return;
	M_EPILOG
}

HClass const* HDict::key_type( void ) const {
	return ( _keyType );
}

void HDict::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	_keyType = nullptr;
	return;
	M_EPILOG
}

HIterable::iterator_t HDict::do_iterator( huginn::HThread*, int ) {
	return (
		make_pointer<dict::HDictIterator>(
			this,
			*get_class()->runtime()->object_factory(),
			dict::HDictIterator::TYPE::KEYS
		)
	);
}

HHuginn::value_t HDict::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	HHuginn::value_t res( thread_->runtime().object_factory()->create_dict() );
	HDict* dict( static_cast<HDict*>( res.raw() ) );
	dict->_keyType = _keyType;
	values_t& data( dict->value() );
	HAnchorGuard<HDict> ag( *dict, thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert(
			data.end(),
			make_pair(
				v.first->clone( thread_, const_cast<HHuginn::value_t*>( &v.first ), position_ ),
				v.second->clone( thread_, const_cast<HHuginn::value_t*>( &v.second ), position_ )
			)
		);
	}
	return res;
}

bool HDict::do_operator_equals( huginn::HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	HDict const& otherValue( *static_cast<huginn::HDict const*>( other_.raw() ) );
	huginn::HDict::values_t const& otherData( otherValue.value() );
	bool equal( ( _data.get_size() == otherData.get_size() ) && ( _keyType == otherValue._keyType ) );
	for ( huginn::HDict::values_t::const_iterator lit( _data.begin() ), rit( otherData.begin() ), end( _data.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		HHuginn::value_t const& lf( lit->first );
		HHuginn::value_t const& ls( lit->second );
		HHuginn::value_t const& rf( rit->first );
		HHuginn::value_t const& rs( rit->second );
		equal = ( ls->type_id() == rs->type_id() )
			&& lf->operator_equals( thread_, lf, rf, position_ )
			&& ls->operator_equals( thread_, ls, rs, position_ );
	}
	return equal;
	M_EPILOG
}

void HDict::do_operator_add( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	huginn::HDict const& otherValue( *static_cast<huginn::HDict const*>( other_.raw() ) );
	HClass const* otherKeyType( otherValue.key_type() );
	if ( otherKeyType && ( otherKeyType->type_id() != HHuginn::TYPE::NONE ) ) {
		update_key_type( thread_, otherValue.key_type(), position_ );
	}
	huginn::HDict::values_t const& otherData( otherValue.value() );
	HAnchorGuard<huginn::HDict> ag( *this, thread_, position_ );
	for ( huginn::HDict::values_t::const_iterator it( otherData.begin() ), end( otherData.end() ); it != end; ++ it ) {
		_data.insert( *it );
	}
	return;
}

bool HDict::do_operator_contains( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	return ( has_key( thread_, other_, position_ ) );
}

HHuginn::value_t HDict::do_operator_subscript( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& key_, int position_ ) const {
	return ( get( thread_, key_, position_ ) );
}

void HDict::do_operator_subscript_assign( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& key_, HHuginn::value_t&& value_, int position_ ) {
	update_key_type( thread_, key_->get_class(), position_ );
	HAnchorGuard<HDict> ag( *this, thread_, position_ );
	_data[key_] = yaal::move( value_ );
}

hash_value_t HDict::do_operator_hash( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	hash_value_t hashValue( static_cast<hash_value_t>( HHuginn::TYPE::DICT ) );
	for ( huginn::HDict::values_t::value_type const& v : _data ) {
		hashValue *= 3;
		hashValue += v.first->operator_hash( thread_, v.first, position_ );
		hashValue *= 3;
		hashValue += v.second->operator_hash( thread_, v.second, position_ );
	}
	return hashValue;
}

yaal::hcore::HString HDict::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "dict()" );
	}
	hcore::HString str( "[" );
	bool next( false );
	for ( huginn::HDict::values_t::value_type const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v.first->code( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( v.second->code( thread_, v.second, cycleTracker_, position_ ) );
	}
	str.append( "]" );
	return str;
}

yaal::hcore::HString HDict::do_to_string( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "dict()" );
	}
	hcore::HString str( "[" );
	bool next( false );
	for ( huginn::HDict::values_t::value_type const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v.first->to_string( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( v.second->to_string( thread_, v.second, cycleTracker_, position_ ) );
	}
	str.append( "]" );
	return str;
}

}

}

}

