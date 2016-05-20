/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  algorithms.cxx - this file is integral part of `yaal' project.

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
#include "thread.hxx"
#include "value_builtin.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HRange : public HHuginn::HIterable {
	int _from;
	int _stop;
	int _step;
public:
	HRange( HHuginn::HClass* class_, int from_, int stop_, int step_ )
		: HIterable( class_ )
		, _from( from_ )
		, _stop( stop_ )
		, _step( step_ ) {
	}
	int step( void ) const {
		return ( _step );
	}
	int stop( void ) const {
		return ( _stop );
	}
	static HHuginn::class_t get_class( HRuntime* huginn_ ) {
		M_PROLOG
		return (
			huginn_->create_class(
				"Range",
				nullptr,
				HHuginn::field_definitions_t{}
			)
		);
		M_EPILOG
	}
protected:
	virtual int long do_size( void ) const override {
		return ( ( _stop - _from ) / _step );
	}
private:
	virtual HIterator do_iterator( void ) override;
};

class HRangeIterator : public HIteratorInterface {
	int _i;
	HRange const* _range;
	HObjectFactory* _objectFactory;
public:
	HRangeIterator( int from_, HRange const* range_ )
		: _i( from_ )
		, _range( range_ )
		, _objectFactory( range_->HIterable::get_class()->runtime()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _objectFactory->create_integer( _i ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _i < _range->stop() );
	}
	virtual void do_next( void ) override {
		_i += _range->step();
	}
private:
	HRangeIterator( HRangeIterator const& ) = delete;
	HRangeIterator& operator = ( HRangeIterator const& ) = delete;
};

HRange::HIterator HRange::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HRangeIterator( _from, this ) );
	return ( HIterator( yaal::move( impl ) ) );
}

class HAlgorithms : public HHuginn::HObject {
	HHuginn::class_t _rangeClass;
public:
	HAlgorithms( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _rangeClass( HRange::get_class( class_->runtime() ) ) {
		class_->runtime()->huginn()->register_class( _rangeClass );
		return;
	}
	static HHuginn::value_t range( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		return ( static_cast<HAlgorithms*>( object_->raw() )->do_range( values_, position_ ) );
	}
	static HHuginn::value_t sorted( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		char const name[] = "Algorithms.sorted";
		verify_arg_count( name, values_, 1, 2, position_ );
		HHuginn::type_id_t t( verify_arg_collection( name, values_, 0, false, position_ ) );
		HHuginn::value_t key;
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::FUNCTION_REFERENCE, false, position_ );
			key = values_[1];
		}
		HHuginn::value_t v( thread_->object_factory().create_list() );
		HHuginn::HList::values_t& dest( static_cast<HHuginn::HList*>( v.raw() )->value() );
		if ( t == HHuginn::TYPE::LIST ) {
			HHuginn::HList::values_t const& s( static_cast<HHuginn::HList const*>( values_[0].raw() )->value() );
			dest = s;
		} else if ( t == HHuginn::TYPE::DEQUE ) {
			HHuginn::HDeque::values_t const& d( static_cast<HHuginn::HDeque const*>( values_[0].raw() )->value() );
			dest.assign( d.begin(), d.end() );
		} else if ( t == HHuginn::TYPE::ORDER ) {
			HHuginn::HOrder::values_t const& s( static_cast<HHuginn::HOrder const*>( values_[0].raw() )->value() );
			dest.assign( s.begin(), s.end() );
		} else if ( t == HHuginn::TYPE::SET ) {
			HHuginn::HSet::values_t const& s( static_cast<HHuginn::HSet const*>( values_[0].raw() )->value() );
			dest.assign( s.begin(), s.end() );
		} else if ( t == HHuginn::TYPE::DICT ) {
			HHuginn::HDict::values_t const& s( static_cast<HHuginn::HDict const*>( values_[0].raw() )->value() );
			for ( HHuginn::HDict::values_t::value_type const& e : s ) {
				dest.push_back( e.first );
			}
		} else if ( t == HHuginn::TYPE::LOOKUP ) {
			HHuginn::HLookup::values_t const& s( static_cast<HHuginn::HLookup const*>( values_[0].raw() )->value() );
			for ( HHuginn::HLookup::values_t::value_type const& e : s ) {
				dest.push_back( e.first );
			}
		} else if ( t == HHuginn::TYPE::STRING ) {
			HString const& s( get_string( values_[0] ) );
			for ( char c : s ) {
				dest.push_back( thread_->object_factory().create_character( c ) );
			}
		} else {
			M_ASSERT( ! "bad code path"[0] );
		}
		if ( ! key ) {
			sort( dest.begin(), dest.end(), &value_builtin::less_low );
		} else {
			HHuginn::values_t l( 1 );
			HHuginn::values_t r( 1 );
			HHuginn::function_t k( static_cast<HHuginn::HFunctionReference*>( key.raw() )->function() );
			sort(
				dest.begin(), dest.end(),
				[&l, &r, &k, &thread_, &position_]( HHuginn::value_t const& l_, HHuginn::value_t const& r_ ) {
					l[0] = l_;
					r[0] = r_;
					return (
						value_builtin::less(
							k( thread_, nullptr, l, position_ ),
							k( thread_, nullptr, r, position_ ),
							position_
						)
					);
				}
			);
		}
		return ( v );
	}
private:
	HHuginn::value_t do_range( HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Algorithms.range";
		verify_arg_count( name, values_, 1, 3, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, false, position_ );
		int from( 0 );
		int stop( 0 );
		int step( 1 );
		int s( static_cast<int>( values_.get_size() ) );
		switch ( s ) {
			case ( 1 ): {
				stop = static_cast<int>( get_integer( values_[0] ) );
			} break;
			case ( 3 ): {
				verify_arg_type( name, values_, 2, HHuginn::TYPE::INTEGER, false, position_ );
				step = static_cast<int>( get_integer( values_[2] ) );
			} /* fall-through */
			case ( 2 ): {
				verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
				from = static_cast<int>( get_integer( values_[0] ) );
				stop = static_cast<int>( get_integer( values_[1] ) );
			} break;
		}
		return ( make_pointer<HRange>( _rangeClass.raw(), from, stop, step ) );
		M_EPILOG
	}
};

namespace package_factory {

class HAlgorithmsCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} algorithmsCreator;

HHuginn::value_t HAlgorithmsCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Algorithms",
			nullptr,
			HHuginn::field_definitions_t{
				{ "range",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HAlgorithms::range, _1, _2, _3, _4 ) ) },
				{ "sorted", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HAlgorithms::sorted, _1, _2, _3, _4 ) ) }
			}
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HAlgorithms>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactoryInstance::get_instance();
	factory.register_package_creator( "Algorithms", &algorithmsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

