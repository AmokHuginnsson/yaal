/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "lookup.hxx"
#include "runtime.hxx"
#include "iterator.hxx"
#include "value_builtin.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace lookup {

class HLookupIterator : public HNotifableIterator {
public:
	enum class TYPE {
		KEYS,
		KEY_VALUES
	};
	typedef HHuginn::value_t (HLookupIterator::*value_getter_t)( void );
private:
	HHuginn::HLookup::values_t* _lookup;
	HHuginn::HLookup::values_t::iterator _it;
	value_getter_t _valueGetter;
	HObjectFactory& _objectFactory;
public:
	HLookupIterator( HHuginn::HLookup* owner_, HObjectFactory& objectFactory_, TYPE type_ )
		: HNotifableIterator( owner_ )
		, _lookup( &owner_->value() )
		, _it( _lookup->begin() )
		, _valueGetter( type_ == TYPE::KEYS ? &HLookupIterator::get_key : &HLookupIterator::get_key_value )
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
		return ( _it != _lookup->end() );
	}
	virtual void do_next( HThread*, int ) override {
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _lookup->end();
	}
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_node_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HLookupIterator( HLookupIterator const& ) = delete;
	HLookupIterator& operator = ( HLookupIterator const& ) = delete;
};

class HKeyValuesLookupView : public HHuginn::HIterable {
	HHuginn::value_t _lookup;
public:
	HKeyValuesLookupView( HHuginn::HClass const* class_, HHuginn::value_t const& lookup_ )
		: HIterable( class_ )
		, _lookup( lookup_ ) {
		M_ASSERT( _lookup->type_id() == HHuginn::TYPE::LOOKUP );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				runtime_,
				objectFactory_,
				"KeyValuesLookupView",
				"The `KeyValuesLookupView` class represents *lazy* *iterable* view of a `lookup` consisted of key-value pairs."
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HLookup const*>( _lookup.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override {
		HIterator::iterator_implementation_t impl(
			new ( memory::yaal ) HLookupIterator(
				static_cast<HHuginn::HLookup*>( _lookup.raw() ),
				*_lookup->get_class()->runtime()->object_factory(),
				HLookupIterator::TYPE::KEY_VALUES
			)
		);
		return ( HIterator( yaal::move( impl ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HKeyValuesLookupView>( HIterable::get_class(), _lookup ) );
	}
};

class HLookupReverseIterator : public HNotifableIterator {
	HHuginn::HLookup::values_t* _lookup;
	HHuginn::HLookup::values_t::reverse_iterator _it;
public:
	HLookupReverseIterator( HHuginn::HLookup* owner_ )
		: HNotifableIterator( owner_ )
		, _lookup( &owner_->value() )
		, _it( _lookup->rbegin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( HThread*, int ) override {
		return ( _it != _lookup->rend() );
	}
	virtual void do_next( HThread*, int ) override {
		if ( _skip == 0 ) {
			++ _it;
		} else {
			-- _skip;
		}
	}
	virtual void do_invalidate( void ) override {
		_it = _lookup->rend();
	}
	virtual void do_skip( void ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_node_id( void ) const override {
		return ( _it.base().node_id() );
	}
private:
	HLookupReverseIterator( HLookupReverseIterator const& ) = delete;
	HLookupReverseIterator& operator = ( HLookupReverseIterator const& ) = delete;
};

class HReversedLookup : public HHuginn::HIterable {
	HHuginn::value_t _lookup;
public:
	HReversedLookup( HHuginn::HClass const* class_, HHuginn::value_t const& lookup_ )
		: HIterable( class_ )
		, _lookup( lookup_ ) {
		M_ASSERT( _lookup->type_id() == HHuginn::TYPE::LOOKUP );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HHuginn::HClass>(
				runtime_,
				objectFactory_,
				"ReversedLookupView",
				"The `ReversedLookupView` class represents *lazy* *iterable* reversed view of a `lookup`."
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<HHuginn::HLookup const*>( _lookup.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual HIterator do_iterator( HThread*, int ) override {
		HIterator::iterator_implementation_t impl(
			new ( memory::yaal ) HLookupReverseIterator( static_cast<HHuginn::HLookup*>( _lookup.raw() ) )
		);
		return ( HIterator( yaal::move( impl ) ) );
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HReversedLookup>( HIterable::get_class(), _lookup ) );
	}
};

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.has_key", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	bool hasKey( static_cast<HHuginn::HLookup*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.get", values_, 1, 2, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	HHuginn::HLookup* l( static_cast<HHuginn::HLookup*>( object_->raw() ) );
	HHuginn::value_t v;
	if ( values_.get_size() > 1 ) {
		bool hasKey( l->try_get( thread_, values_[0], v, position_ ) );
		if ( ! hasKey ) {
			v = values_[1];
		}
	} else {
		v = l->get( thread_, values_[0], position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t ensure( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.ensure", values_, 2, 2, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	HHuginn::HLookup* l( static_cast<HHuginn::HLookup*>( object_->raw() ) );
	HHuginn::value_t& v( l->get_ref( thread_, values_[0], position_ ) );
	if ( ! v ) {
		v = values_[1];
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.erase", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	static_cast<HHuginn::HLookup*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	static_cast<HHuginn::HLookup*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	verify_signature( "lookup.update", values_, { HHuginn::TYPE::LOOKUP }, thread_, position_ );
	static_cast<HHuginn::HLookup*>( object_->raw() )->update( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	HHuginn::HLookup::values_t const& values( static_cast<HHuginn::HLookup*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::LOOKUP ) );
	for ( HHuginn::HLookup::values_t::value_type const& v : values ) {
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
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	verify_signature( "lookup.equals", values_, { HHuginn::TYPE::LOOKUP }, thread_, position_ );
	HHuginn::HLookup::values_t const& l( static_cast<HHuginn::HLookup*>( object_->raw() )->value() );
	HHuginn::HLookup::values_t const& r( static_cast<HHuginn::HLookup const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( HHuginn::HLookup::values_t::const_iterator lit( l.begin() ), rit( r.begin() ), end( l.end() ); equal && ( lit != end ); ++ lit, ++ rit ) {
		equal = value_builtin::equals( thread_, lit->first, rit->first, position_ ) && value_builtin::equals( thread_, lit->second, rit->second, position_ );
	}
	return ( thread_->object_factory().create_boolean( equal ) );
	M_EPILOG
}

inline HHuginn::value_t values( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	verify_signature( "lookup.values", values_, {}, thread_, position_ );
	return ( lookup::key_values_view( thread_, *object_ ) );
	M_EPILOG
}

class HLookupClass : public HHuginn::HClass {
public:
	typedef HLookupClass this_type;
	typedef HHuginn::HClass base_type;
private:
	HHuginn::class_t _keyValuesLookupViewClass;
	HHuginn::class_t _reversedLookupClass;
public:
	HLookupClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_,
		yaal::hcore::HString const& doc_
	) : HHuginn::HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::LOOKUP ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::LOOKUP ) ),
			doc_,
			&huginn_builtin::lookup
		)
		, _keyValuesLookupViewClass( HKeyValuesLookupView::get_class( runtime_, objectFactory_ ) )
		, _reversedLookupClass( HReversedLookup::get_class( runtime_, objectFactory_ ) ) {
		return;
	}
	HHuginn::HClass const* key_values_lookup_view_class( void ) const {
		return ( _keyValuesLookupViewClass.raw() );
	}
	HHuginn::HClass const* reversed_lookup_class( void ) const {
		return ( _reversedLookupClass.raw() );
	}
protected:
	void do_finalize_registration( huginn::HRuntime* runtime_ ) {
		runtime_->huginn()->register_class( _keyValuesLookupViewClass );
		runtime_->huginn()->register_class( _reversedLookupClass );
	}
};

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HLookupClass>(
			runtime_,
			objectFactory_,
			"The `lookup` is a collection providing a sorted key to value map. It supports operations of iteration, key-value insertion, key removal and key search."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "has_key", objectFactory_->create_method( &lookup::has_key ), "( *key* ) - tell if given *key* can be found in this `lookup`" },
		{ "get",     objectFactory_->create_method( &lookup::get ),     "( *key*, *default* ) - get value for given *key* from this `lookup`, or *default* if given *key* is not present in the `lookup`" },
		{ "ensure",  objectFactory_->create_method( &lookup::ensure ),  "( *key*, *default* ) - get value for given *key* from this `lookup`, if given *key* is not present in the `lookup` insert *default* into this `lookup` before returning it" },
		{ "erase",   objectFactory_->create_method( &lookup::erase ),   "( *key* ) - remove given *key* from this `lookup`" },
		{ "clear",   objectFactory_->create_method( &lookup::clear ),   "erase `lookup`'s content, `lookup` becomes empty" },
		{ "add",     objectFactory_->create_method( &lookup::update ),  "( *other* ) - update content of this `lookup` with key/value pairs from *other* `lookup`" },
		{ "update",  objectFactory_->create_method( &lookup::update ),  "( *other* ) - update content of this `lookup` with key/value pairs from *other* `lookup`" },
		{ "hash",    objectFactory_->create_method( &lookup::hash ),    "calculate hash value for this `lookup`" },
		{ "equals",  objectFactory_->create_method( &lookup::equals ),  "( *other* ) - test if *other* `lookup` has the same content" },
		{ "values",  objectFactory_->create_method( &lookup::values ),  "get key-value pairs view of this `lookup`" }
	};
	c->redefine( nullptr, fd );
	return ( c );
	M_EPILOG
}

HHuginn::value_t key_values_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::LOOKUP );
	HLookupClass const* lc( static_cast<HLookupClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HKeyValuesLookupView>( lc->key_values_lookup_view_class(), value_ ) );
	M_EPILOG
}

HHuginn::value_t reversed_view( huginn::HThread* thread_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	M_ASSERT( value_->type_id() == HHuginn::TYPE::LOOKUP );
	HLookupClass const* lc( static_cast<HLookupClass const*>( value_->get_class() ) );
	return ( thread_->object_factory().create<HReversedLookup>( lc->reversed_lookup_class(), value_ ) );
	M_EPILOG
}

}

}

HHuginn::HLookup::HLookup( HHuginn::HClass const* class_, allocator_t const& allocator_ )
	: HInvalidatingIterable( class_ )
	, _helper()
	, _data( _helper, _helper, allocator_ ) {
	return;
}

int long HHuginn::HLookup::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

HHuginn::value_t HHuginn::HLookup::get( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in `lookup'.", thread_->current_frame()->file_id(), position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HLookup::try_get( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return ( found );
	M_EPILOG
}

bool HHuginn::HLookup::has_key( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HLookup::erase( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( it != _data.end() ) {
		invalidate( it.node_id() );
		_data.erase( it );
	}
	return;
	M_EPILOG
}

HHuginn::value_t& HHuginn::HLookup::get_ref( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	return ( _data[key_] );
	M_EPILOG
}

void HHuginn::HLookup::insert( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	_data.insert( make_pair( key_, value_ ) );
	return;
	M_EPILOG
}

void HHuginn::HLookup::update( huginn::HThread* thread_, HHuginn::value_t const& lookup_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HHuginn::HLookup> ag( *this, thread_, position_ );
	HHuginn::HLookup::values_t const& r( static_cast<HHuginn::HLookup const*>( lookup_.raw() )->value() );
	for ( HHuginn::HLookup::values_t::const_iterator it( r.begin() ), end( r.end() ); it != end; ++ it ) {
		_data.insert( *it );
	}
	return;
	M_EPILOG
}

void HHuginn::HLookup::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HLookup::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl(
		new ( memory::yaal ) huginn::lookup::HLookupIterator(
			this,
			*get_class()->runtime()->object_factory(),
			huginn::lookup::HLookupIterator::TYPE::KEYS
		)
	);
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HLookup::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	HHuginn::value_t res( thread_->runtime().object_factory()->create_lookup() );
	HLookup* lookup( static_cast<HLookup*>( res.raw() ) );
	values_t& data( lookup->value() );
	HAnchorGuard<HHuginn::HLookup> ag( *lookup, thread_, position_ );
	for ( values_t::value_type const& v : _data ) {
		data.insert(
			make_pair(
				v.first->clone( thread_, const_cast<HHuginn::value_t*>( &v.first ), position_ ),
				v.second->clone( thread_, const_cast<HHuginn::value_t*>( &v.second ), position_ )
			)
		);
	}
	return ( res );
	M_EPILOG
}

}

}

