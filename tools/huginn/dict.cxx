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
	virtual HHuginn::value_t do_value( void ) override {
		return ( _it->first );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _dict->end() );
	}
	virtual void do_next( void ) override {
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
	HHuginn::HDict* d( dynamic_cast<HHuginn::HDict*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	bool hasKey( d->has_key( values_[0], position_ ) );
	return ( thread_->object_factory().create_boolean( hasKey ) );
	M_EPILOG
}

inline HHuginn::value_t get( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.get", values_, 1, 2, position_ );
	HHuginn::HDict* d( dynamic_cast<HHuginn::HDict*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	HHuginn::value_t v;
	if ( values_.get_size() > 1 ) {
		bool hasKey( d->try_get( values_[0], v, position_ ) );
		if ( ! hasKey ) {
			v = values_[1];
		}
	} else {
		v = d->get( values_[0], position_ );
	}
	return ( v );
	M_EPILOG
}

inline HHuginn::value_t erase( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "dict.erase", values_, 1, 1, position_ );
	HHuginn::HDict* d( dynamic_cast<HHuginn::HDict*>( object_->raw() ) );
	M_ASSERT( d != nullptr );
	d->erase( values_[0], position_ );
	return ( *object_ );
	M_EPILOG
}

HHuginn::class_t get_class( HHuginn* );
HHuginn::class_t get_class( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			type_id( HHuginn::TYPE::DICT ),
			huginn_->identifier_id( type_name( HHuginn::TYPE::DICT ) ),
			nullptr,
			HHuginn::field_names_t{
				"has_key",
				"get",
				"erase"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::has_key, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::get, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &dict::erase, _1, _2, _3, _4 ) )
			}
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HDict::HDict( HHuginn::HClass const* class_ )
	: HIterable( class_ )
	, _data( &value_builtin::less_low )
	, _keyType( &huginn::_noneClass_ ) {
	return;
}

HHuginn::HDict::HDict( HHuginn::HClass const* class_, values_t const& data_, HHuginn::HClass const* keyType_ )
	: HIterable( class_ )
	, _data( &value_builtin::less_low )
	, _keyType( keyType_ ) {
	HHuginn* huginn( class_->huginn() );
	for ( values_t::value_type const& v : data_ ) {
		_data.insert( make_pair( v.first->clone( huginn ), v.second->clone( huginn ) ) );
	}
	return;
}

int long HHuginn::HDict::do_size( void ) const {
	return ( _data.get_size() );
}

void HHuginn::HDict::verify_key_type( HHuginn::HClass const* keyType_, int position_ ) const {
	if ( ( _keyType->type_id() != TYPE::NONE ) && ( keyType_ != _keyType ) ) {
		throw HHuginnRuntimeException( "Non-uniform key types.", position_ );
	}
	if ( ! OCompiler::is_comparable( keyType_->type_id() ) ) {
		throw HHuginnRuntimeException( "Key type `"_ys.append( keyType_->name() ).append( "' is not a comparable." ), position_ );
	}
	return;
}

HHuginn::value_t HHuginn::HDict::get( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	values_t::iterator it( _data.find( key_ ) );
	if ( ! ( it != _data.end() ) ) {
		throw HHuginnRuntimeException( "Key does not exist in `dict'.", position_ );
	}
	return ( it->second );
	M_EPILOG
}

bool HHuginn::HDict::try_get( HHuginn::value_t const& key_, HHuginn::value_t& result_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	values_t::iterator it( _data.find( key_ ) );
	bool found( false );
	if ( it != _data.end() ) {
		result_ = it->second;
		found = true;
	}
	return ( found );
	M_EPILOG
}

bool HHuginn::HDict::has_key( HHuginn::value_t const& key_, int position_ ) const {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	return ( _data.find( key_ ) != _data.end() );
	M_EPILOG
}

void HHuginn::HDict::erase( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_data.erase( key_ );
	return;
	M_EPILOG
}

HHuginn::value_t HHuginn::HDict::get_ref( HHuginn::value_t const& key_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_keyType = key_->get_class();
	return ( make_pointer<HReference>( _data[key_] ) );
	M_EPILOG
}

void HHuginn::HDict::insert( HHuginn::value_t const& key_, HHuginn::value_t const& value_, int position_ ) {
	M_PROLOG
	verify_key_type( key_->get_class(), position_ );
	_data.insert( make_pair( key_, value_ ) );
	_keyType = key_->get_class();
	return;
	M_EPILOG
}

HHuginn::HDict::values_t const& HHuginn::HDict::value( void ) const {
	return ( _data );
}

HHuginn::HDict::values_t& HHuginn::HDict::value( void ) {
	return ( _data );
}

HHuginn::HIterable::HIterator HHuginn::HDict::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HDictIterator( &_data ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HDict::do_clone( HHuginn* huginn_ ) const {
	return ( huginn_->object_factory()->create_dict( _data, _keyType ) );
}

}

}

