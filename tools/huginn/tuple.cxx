/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  tuple.cxx - this file is integral part of `yaal' project.

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
#include "helper.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HTupleIterator : public HIteratorInterface {
	HHuginn::HTuple* _tuple;
	int long _index;
public:
	HTupleIterator( HHuginn::HTuple* tuple_ )
		: _tuple( tuple_ )
		, _index( 0 ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _tuple->get( _index ) );
	}
	virtual bool do_is_valid( huginn::HThread* thread_, int position_ ) override {
		return ( _index < _tuple->size( thread_, position_ ) );
	}
	virtual void do_next( HThread*, int ) override {
		++ _index;
	}
private:
	HTupleIterator( HTupleIterator const& ) = delete;
	HTupleIterator& operator = ( HTupleIterator const& ) = delete;
};

namespace tuple {

inline HHuginn::value_t add( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "tuple.add", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	HHuginn::HTuple::values_t& dst( static_cast<HHuginn::HTuple*>( object_->raw() )->value() );
	HHuginn::HTuple::values_t const& src( static_cast<HHuginn::HTuple const*>( values_[0].raw() )->value() );
	dst.insert( dst.end(), src.begin(), src.end() );
	return ( *object_ );
	M_EPILOG
}

inline HHuginn::value_t hash( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_arg_count( "tuple.hash", values_, 0, 0, thread_, position_ );
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	HHuginn::HTuple::values_t const& values( static_cast<HHuginn::HTuple*>( object_->raw() )->value() );
	int long hashValue( static_cast<int long>( HHuginn::TYPE::TUPLE ) );
	for ( HHuginn::value_t const& v : values ) {
		hashValue *= 3;
		hashValue += value_builtin::hash( thread_, v, position_ );
	}
	return ( thread_->object_factory().create_integer( hashValue ) );
	M_EPILOG
}

inline HHuginn::value_t less( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.less", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	HHuginn::HTuple::values_t const& l( static_cast<HHuginn::HTuple*>( object_->raw() )->value() );
	HHuginn::HTuple::values_t const& r( static_cast<HHuginn::HTuple const*>( values_[0].raw() )->value() );
	HHuginn::HValueLessHelper lessHelper;
	lessHelper.anchor( thread_, position_ );
	bool res( lexicographical_compare( l.begin(), l.end(), r.begin(), r.end(), cref( lessHelper ) ) );
	return ( thread_->object_factory().create_boolean( res ) );
	M_EPILOG
}

inline HHuginn::value_t equals( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	M_ASSERT( (*object_)->type_id() == HHuginn::TYPE::TUPLE );
	verify_signature( "tuple.equals", values_, { HHuginn::TYPE::TUPLE }, thread_, position_ );
	HHuginn::HTuple::values_t const& l( static_cast<HHuginn::HTuple*>( object_->raw() )->value() );
	HHuginn::HTuple::values_t const& r( static_cast<HHuginn::HTuple const*>( values_[0].raw() )->value() );
	bool equal( l.get_size() == r.get_size() );
	for ( int long i( 0 ), c( l.get_size() ); equal && ( i < c ); ++ i ) {
		equal = value_builtin::equals( thread_, l[i], r[i], position_ );
	}
	return ( thread_->object_factory().create_boolean( equal ) );
	M_EPILOG
}

HHuginn::class_t get_class( HRuntime*, HObjectFactory* );
HHuginn::class_t get_class( HRuntime* runtime_, HObjectFactory* objectFactory_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			runtime_,
			type_id( HHuginn::TYPE::TUPLE ),
			runtime_->identifier_id( type_name( HHuginn::TYPE::TUPLE ) ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "add",    objectFactory_->create_method( hcore::call( &tuple::add, _1, _2, _3, _4 ) ),    "( *other* ) - append all elements from *other* `tuple` at the end of this `tuple`" },
				{ "hash",   objectFactory_->create_method( hcore::call( &tuple::hash, _1, _2, _3, _4 ) ),   "calculate hash value for this `tuple`" },
				{ "less",   objectFactory_->create_method( hcore::call( &tuple::less, _1, _2, _3, _4 ) ),   "( *other* ) - test if this `tuple` comes lexicographically before *other* `tuple`" },
				{ "equals", objectFactory_->create_method( hcore::call( &tuple::equals, _1, _2, _3, _4 ) ), "( *other* ) - test if *other* `tuple` has the same content" }
			},
			"The `tuple` is a collection type that is used to represent and operate on `tuple` of values. "
			"It supports basic subscript and range operators."
		)
	);
	return ( c );
	M_EPILOG
}

}

}

HHuginn::HTuple::HTuple( HHuginn::HClass const* class_, values_t&& data_ )
	: HIterable( class_ )
	, _data( yaal::move( data_ ) ) {
	return;
}

int long HHuginn::HTuple::do_size( huginn::HThread*, int ) const {
	return ( _data.get_size() );
}

HHuginn::value_t HHuginn::HTuple::get( int long long index_ ) {
	M_PROLOG
	M_ASSERT( ( index_ >= 0 ) && ( index_ < _data.get_size() ) );
	return ( _data[static_cast<int>( index_ )] );
	M_EPILOG
}

HHuginn::HIterable::HIterator HHuginn::HTuple::do_iterator( huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) huginn::HTupleIterator( this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

HHuginn::value_t HHuginn::HTuple::do_clone( huginn::HThread* thread_, int position_ ) const {
	values_t data;
	data.reserve( _data.get_size() );
	for ( values_t::value_type const& v : _data ) {
		data.push_back( v->clone( thread_, position_ ) );
	}
	return ( thread_->runtime().object_factory()->create_tuple( yaal::move( data ) ) );
}

}

}

