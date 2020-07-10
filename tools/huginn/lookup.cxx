/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "lookup.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "iterator.hxx"
#include "instruction.hxx"
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
	huginn::HLookup::values_t* _lookup;
	huginn::HLookup::values_t::iterator _it;
	value_getter_t _valueGetter;
	HObjectFactory& _objectFactory;
public:
	HLookupIterator( huginn::HLookup* owner_, HObjectFactory& objectFactory_, TYPE type_ )
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
	virtual void do_skip( HThread*, int ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.node_id() );
	}
private:
	HLookupIterator( HLookupIterator const& ) = delete;
	HLookupIterator& operator = ( HLookupIterator const& ) = delete;
};

class HKeyValuesLookupView : public huginn::HIterable {
	HHuginn::value_t _lookup;
public:
	HKeyValuesLookupView( HClass const* class_, HHuginn::value_t const& lookup_ )
		: HIterable( class_ )
		, _lookup( lookup_ ) {
		M_ASSERT( _lookup->type_id() == HHuginn::TYPE::LOOKUP );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				objectFactory_,
				"KeyValuesLookupView",
				"The `KeyValuesLookupView` class represents *lazy* *iterable* view of a `lookup` consisted of key-value pairs.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<huginn::HLookup const*>( _lookup.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override {
		return (
			make_pointer<HLookupIterator>(
				static_cast<huginn::HLookup*>( _lookup.raw() ),
				*_lookup->get_class()->runtime()->object_factory(),
				HLookupIterator::TYPE::KEY_VALUES
			)
		);
	}
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override {
		return ( thread_->object_factory().create<HKeyValuesLookupView>( HIterable::get_class(), _lookup ) );
	}
};

class HLookupReverseIterator : public HNotifableIterator {
	huginn::HLookup::values_t* _lookup;
	huginn::HLookup::values_t::reverse_iterator _it;
public:
	HLookupReverseIterator( huginn::HLookup* owner_ )
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
	virtual void do_skip( HThread*, int ) override {
		++ _it;
		++ _skip;
	}
	virtual void const* do_id( void ) const override {
		return ( _it.raw().node_id() );
	}
private:
	HLookupReverseIterator( HLookupReverseIterator const& ) = delete;
	HLookupReverseIterator& operator = ( HLookupReverseIterator const& ) = delete;
};

class HReversedLookup : public huginn::HIterable {
	HHuginn::value_t _lookup;
public:
	HReversedLookup( HClass const* class_, HHuginn::value_t const& lookup_ )
		: HIterable( class_ )
		, _lookup( lookup_ ) {
		M_ASSERT( _lookup->type_id() == HHuginn::TYPE::LOOKUP );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_, HClass const* origin_ ) {
		M_PROLOG
		HHuginn::class_t c(
			make_pointer<HClass>(
				runtime_,
				objectFactory_,
				"ReversedLookupView",
				"The `ReversedLookupView` class represents *lazy* *iterable* reversed view of a `lookup`.",
				origin_
			)
		);
		return ( c );
		M_EPILOG
	}
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( safe_int::cast<int long>( static_cast<huginn::HLookup const*>( _lookup.raw() )->size( thread_, position_ ) ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override {
		return ( make_pointer<HLookupReverseIterator>( static_cast<huginn::HLookup*>( _lookup.raw() ) ) );
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
	bool hasKey( static_cast<huginn::HLookup*>( object_->raw() )->has_key( thread_, values_[0], position_ ) );
	return ( thread_->runtime().boolean_value( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.get", values_, 1, 2, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	huginn::HLookup* l( static_cast<huginn::HLookup*>( object_->raw() ) );
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
	huginn::HLookup& l( *static_cast<huginn::HLookup*>( object_->raw() ) );
	HAnchorGuard<HLookup> ag( l, thread_, position_ );
	return ( l.value().insert( make_pair( values_[0], values_[1] ) ).first->second );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.erase", values_, 1, 1, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	static_cast<huginn::HLookup*>( object_->raw() )->erase( thread_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.clear", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	static_cast<huginn::HLookup*>( object_->raw() )->clear();
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t update( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	verify_signature( "lookup.update", values_, { HHuginn::TYPE::LOOKUP }, thread_, position_ );
	(*object_)->operator_add( thread_, *object_, values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t values( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::LOOKUP );
	verify_signature( "lookup.values", values_, {}, thread_, position_ );
	return ( lookup::key_values_view( thread_, *object_ ) );
	M_EPILOG
}

class HLookupClass : public HClass {
public:
	typedef HLookupClass this_type;
	typedef HClass base_type;
private:
	HHuginn::class_t _keyValuesLookupViewClass;
	HHuginn::class_t _reversedLookupClass;
public:
	HLookupClass(
		HRuntime* runtime_,
		HObjectFactory* objectFactory_,
		yaal::hcore::HString const& doc_
	) : HClass(
			runtime_,
			objectFactory_,
			huginn::type_id( HHuginn::TYPE::LOOKUP ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::LOOKUP ) ),
			doc_,
			&builtin::lookup
		)
		, _keyValuesLookupViewClass()
		, _reversedLookupClass() {
		HHuginn::field_definitions_t fd{
			{ "has_key", objectFactory_->create_method( &lookup::has_key ), "( *key* ) - tell if given *key* can be found in this `lookup`" },
			{ "get",     objectFactory_->create_method( &lookup::get ),     "( *key*, *default* ) - get value for given *key* from this `lookup`, or *default* if given *key* is not present in the `lookup`" },
			{ "ensure",  objectFactory_->create_method( &lookup::ensure ),  "( *key*, *default* ) - get value for given *key* from this `lookup`, if given *key* is not present in the `lookup` insert *default* into this `lookup` before returning it" },
			{ "erase",   objectFactory_->create_method( &lookup::erase ),   "( *key* ) - remove given *key* from this `lookup`" },
			{ "clear",   objectFactory_->create_method( &lookup::clear ),   "erase `lookup`'s content, `lookup` becomes empty" },
			{ "update",  objectFactory_->create_method( &lookup::update ),  "( *other* ) - update content of this `lookup` with key/value pairs from *other* `lookup`" },
			{ "values",  objectFactory_->create_method( &lookup::values ),  "get key-value pairs view of this `lookup`" }
		};
		redefine( nullptr, fd );
		_keyValuesLookupViewClass = add_class_as_type_reference( this, HKeyValuesLookupView::get_class( runtime_, objectFactory_, this ), HClass::MEMBER_TYPE::STATIC );
		_reversedLookupClass = add_class_as_type_reference( this, HReversedLookup::get_class( runtime_, objectFactory_, this ), HClass::MEMBER_TYPE::STATIC );
		return;
	}
	HClass const* key_values_lookup_view_class( void ) const {
		return ( _keyValuesLookupViewClass.raw() );
	}
	HClass const* reversed_lookup_class( void ) const {
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

HLookup::HLookup( HClass const* class_, allocator_t const& allocator_ )
	: HInvalidatingIterable( class_ )
	, _helper()
	, _data( _helper, _helper, allocator_ ) {
	return;
}

int long HLookup::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

HHuginn::value_t HLookup::get( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	values_t::const_iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginn::HHuginnRuntimeException( "Key does not exist in `lookup`.", thread_->file_id(), position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HLookup::try_get( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return ( found );
	M_EPILOG
}

bool HLookup::has_key( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HLookup::erase( huginn::HThread* thread_, HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( it != _data.end() ) {
		skip( thread_, it.node_id(), position_ );
		_data.erase( it );
	}
	return;
	M_EPILOG
}

void HLookup::insert( huginn::HThread* thread_, HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	_data.insert( make_pair( key_, value_ ) );
	return;
	M_EPILOG
}

void HLookup::do_operator_add( huginn::HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	M_PROLOG
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	HLookup::values_t const& r( static_cast<HLookup const*>( other_.raw() )->value() );
	for ( HLookup::values_t::const_iterator it( r.begin() ), end( r.end() ); it != end; ++ it ) {
		_data.insert( *it );
	}
	return;
	M_EPILOG
}

void HLookup::clear( void ) {
	M_PROLOG
	invalidate();
	_data.clear();
	return;
	M_EPILOG
}

HIterable::iterator_t HLookup::do_iterator( huginn::HThread*, int ) {
	return (
		make_pointer<lookup::HLookupIterator>(
			this,
			*get_class()->runtime()->object_factory(),
			lookup::HLookupIterator::TYPE::KEYS
		)
	);
}

HHuginn::value_t HLookup::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	HHuginn::value_t res( thread_->runtime().object_factory()->create_lookup() );
	HLookup* lookup( static_cast<HLookup*>( res.raw() ) );
	values_t& data( lookup->value() );
	HAnchorGuard<huginn::HLookup> ag( *lookup, thread_, position_ );
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

inline bool HLookup::do_operator_equals( huginn::HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	M_PROLOG
	huginn::HLookup const& otherValue( *static_cast<huginn::HLookup const*>( other_.raw() ) );
	huginn::HLookup::values_t const& otherData( otherValue.value() );
	HAnchorGuard<huginn::HLookup> ag( otherValue, thread_, position_ );
	bool equal( _data.get_size() == otherData.get_size() );
	for ( huginn::HLookup::values_t::const_iterator it( _data.begin() ), end( _data.end() ); equal && ( it != end ); ++ it ) {
		huginn::HLookup::values_t::const_iterator otherIt( otherData.find( it->first ) );
		HHuginn::value_t const& lv( it->second );
		equal = ( otherIt != otherData.end() )
			&& ( lv->type_id() == otherIt->second->type_id() )
			&& lv->operator_equals( thread_, lv, otherIt->second, position_ );
	}
	return ( equal );
	M_EPILOG
}

bool HLookup::do_operator_contains( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	return ( has_key( thread_, other_, position_ ) );
}

HHuginn::value_t HLookup::do_operator_subscript( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& key_, int position_ ) const {
	return ( get( thread_, key_, position_ ) );
}

void HLookup::do_operator_subscript_assign( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& key_, HHuginn::value_t&& value_, int position_ ) {
	HAnchorGuard<HLookup> ag( *this, thread_, position_ );
	_data[key_] = yaal::move( value_ );
}

hash_value_t HLookup::do_operator_hash( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	M_PROLOG
	hash_value_t hashValue( static_cast<hash_value_t>( HHuginn::TYPE::LOOKUP ) );
	for ( huginn::HLookup::values_t::value_type const& v : _data ) {
		hashValue *= 3;
		hashValue += v.first->operator_hash( thread_, v.first, position_ );
		hashValue *= 3;
		hashValue += v.second->operator_hash( thread_, v.second, position_ );
	}
	return ( hashValue );
	M_EPILOG
}

yaal::hcore::HString HLookup::do_code( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "{}" );
	}
	hcore::HString str( "{" );
	bool next( false );
	for ( huginn::HLookup::values_t::value_type const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v.first->code( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( v.second->code( thread_, v.second, cycleTracker_, position_ ) );
	}
	str.append( "}" );
	return ( str );
}

yaal::hcore::HString HLookup::do_to_string( huginn::HThread* thread_, HHuginn::value_t const&, HCycleTracker& cycleTracker_, int position_ ) const {
	if ( _data.is_empty() ) {
		return ( "{}" );
	}
	hcore::HString str( "{" );
	bool next( false );
	for ( huginn::HLookup::values_t::value_type const& v : _data ) {
		if ( next ) {
			str.append( ", " );
		}
		next = true;
		str.append( v.first->to_string( thread_, v.first, cycleTracker_, position_ ) ).append( ": " ).append( v.second->to_string( thread_, v.second, cycleTracker_, position_ ) );
	}
	str.append( "}" );
	return ( str );
}

}

}

}

