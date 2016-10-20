/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  lookup.cxx - this file is integral part of `yaal' project.

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

class HLookupIterator : public HIteratorInterface {
	HHuginn::HLookup::values_t* _lookup;
	HHuginn::HLookup::values_t::iterator _it;
public:
	HLookupIterator( HHuginn::HLookup::values_t* lookup_ )
		: _lookup( lookup_ ), _it( lookup_->begin() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _lookup->end() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HLookupIterator( HLookupIterator const& ) = delete;
	HLookupIterator& operator = ( HLookupIterator const& ) = delete;
};

namespace lookup {

inline HHuginn::value_t has_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.has_key", values_, 1, 1, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	bool hasKey( l->has_key( values_[0] ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.get", values_, 1, 2, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	HHuginn::value_t v;
	if ( values_.get_size() > 1 ) {
		bool hasKey( l->try_get( values_[0], v ) );
		if ( ! hasKey ) {
			v = values_[1];
		}
	} else {
		v = l->get( values_[0], position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.erase", values_, 1, 1, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	l->erase( values_[0] );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t clear( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "lookup.clear", values_, 0, 0, position_ );
	HHuginn::HLookup* l( dynamic_cast<HHuginn::HLookup*>( object_->raw() ) );
	M_ASSERT( l != nullptr );
	l->value().clear();
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime* );
HHuginn::class_t get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::LOOKUP ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::LOOKUP ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "has_key", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::has_key, _1, _2, _3, _4 ) ), "( *key* ) - tell if given *key* can be found in this `lookup`" },
				{ "get",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::get, _1, _2, _3, _4 ) ), "( *key*, *default* ) - get value for given *key* from this `lookup`, or *default* if given *key* is not present in the `lookup`" },
				{ "erase",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::erase, _1, _2, _3, _4 ) ), "( *key* ) - remove given *key* from this `lookup`" },
				{ "clear",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &lookup::clear, _1, _2, _3, _4 ) ), "erase `lookup`'s content, `lookup` becomes empty" }
			},
			"The `lookup` is a collection providing a sorted key to value map. It supports operations of iteration, key-value insertion, key removal and key search."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HLookup::HLookup( HHuginn::HClass const* class_ )
	: HIterable( class_ )
	, _data( &value_builtin::hash, &value_builtin::key_equals ) {
	return;
}

HHuginn::HLookup::HLookup( HHuginn::HClass const* class_, values_t const& data_ )
	: HIterable( class_ )
	, _data( &value_builtin::hash, &value_builtin::key_equals ) {
	HRuntime* runtime( class_->runtime() );
	for ( values_t::value_type const& v : data_ ) {
		_data.insert( make_pair( v.first->clone( runtime ), v.second->clone( runtime ) ) );
	}
	return;
}

int long HHuginn::HLookup::do_size( void ) const {
	return ( _data.get_size() );
}

HHuginn::value_t HHuginn::HLookup::get( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in `lookup'.", position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HLookup::try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_ ) {
	M_PROLOG
	values_t::iterator it( _data.find( key_ ) );
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return ( found );
	M_EPILOG
}

bool HHuginn::HLookup::has_key( HHuginn::value_t const& key_ ) const {
	M_PROLOG
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HLookup::erase( HHuginn::value_t const& key_ ) {
	M_PROLOG
	_data.erase( key_ );
	return;
	M_EPILOG
}

HHuginn::value_t& HHuginn::HLookup::get_ref( HHuginn::value_t const& key_ ) {
	M_PROLOG
	return ( _data[key_] );
	M_EPILOG
}

void HHuginn::HLookup::insert( HHuginn::value_t const& key_, HHuginn::value_t const& value_ ) {
	M_PROLOG
	_data.insert( make_pair( key_, value_ ) );
	return;
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HLookup::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HLookupIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HLookup::do_clone( HRuntime* runtime_ ) const {
	return ( runtime_->object_factory()->create_lookup( _data ) );
}

}

}

