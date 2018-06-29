/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/stringalgo.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HText : public HHuginn::HValue {
public:
	HText( HHuginn::HClass* class_ )
		: HValue( class_ ) {
		return;
	}
	static HHuginn::value_t split( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.split", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		typedef HArray<HString> strings_t;
		strings_t strings( string::split<strings_t>( get_string( values_[0] ), get_string( values_[1] ) ) );
		HObjectFactory* of( thread_->runtime().object_factory() );
		HHuginn::value_t l( of->create_list() );
		for ( HString const& s : strings ) {
			static_cast<HHuginn::HList*>( l.raw() )->push_back( of->create_string( s ) );
		}
		return ( l );
		M_EPILOG
	}
	static HHuginn::value_t join( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Text.join";
		verify_arg_count( name, values_, 2, 2, thread_, position_ );
		HHuginn::type_id_t t( values_[0]->type_id() );
		if (
			( t != HHuginn::TYPE::TUPLE )
			&& ( t != HHuginn::TYPE::LIST )
			&& ( t != HHuginn::TYPE::DEQUE )
			&& ( t != HHuginn::TYPE::ORDER )
			&& ( t != HHuginn::TYPE::SET )
		) {
			throw HHuginn::HHuginnRuntimeException(
				""_ys.append( name )
					.append( "() first argument must be a flat uniform collection of strings, not " )
					.append( a_type_name( values_[0]->get_class() ) )
					.append( "." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		verify_arg_type( name, values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
		HString s;
		HHuginn::HIterable* coll( static_cast<HHuginn::HIterable*>( const_cast<HHuginn::HValue*>( values_[0].raw() ) ) );
		HString const& sep( get_string( values_[1] ) );
		HHuginn::HIterable::iterator_t it( coll->iterator( thread_, position_ ) );
		bool addSep( false );
		while ( thread_->can_continue() && it->is_valid( thread_, position_ ) ) {
			HHuginn::value_t v( it->value( thread_, position_ ) );
			if ( v->type_id() != HHuginn::TYPE::STRING ) {
				throw HHuginn::HHuginnRuntimeException(
					""_ys.append( name )
						.append( "() first argument must be a flat uniform collection of strings, but it contains " )
						.append( a_type_name( v->get_class() ) )
						.append( "." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			if ( addSep ) {
				s.append( sep );
			}
			s.append( get_string( v.raw() ) );
			addSep = true;
			it->next( thread_, position_ );
		}
		return ( thread_->runtime().object_factory()->create_string( s ) );
		M_EPILOG
	}
	static HHuginn::value_t distance( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.distance", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		return ( thread_->runtime().object_factory()->create_integer( string::distance( get_string( values_[0] ), get_string( values_[1] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t repeat( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Text.repeat", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::INTEGER }, thread_, position_ );
		HString out;
		HString const& s( get_string( values_[0] ) );
		int count( static_cast<int>( get_integer( values_[1] ) ) );
		if ( count < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Negative repeat count: "_ys.append( count ), thread_->current_frame()->file_id(), position_ );
		}
		out.reserve( count * s.get_length() );
		for ( int i( 0 ); i < count; ++ i ) {
			out.append( s );
		}
		return ( thread_->runtime().object_factory()->create_string( out ) );
		M_EPILOG
	}
	static HHuginn::value_t int_base_to_str( char const* name_, BASE base_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
		return (
			thread_->runtime().object_factory()->create_string(
				int_to_str( get_integer( values_[0] ), base_, true )
			)
		);
		M_EPILOG
	}
};

namespace package_factory {

class HTextCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} textCreator;

HHuginn::value_t HTextCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Text",
			"The `Text` package provides various text manipulation algorithms.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "split",    runtime_->create_method( &HText::split ),    "( *str*, *sep* ) - split `string` *str* by separator *sep* into a `list` of `string`s" },
		{ "join",     runtime_->create_method( &HText::join ),     "( *coll*, *sep* ) - join all string from *coll* into one `string` using *sep* as separator" },
		{ "distance", runtime_->create_method( &HText::distance ), "( *first*, *second* ) - calculate Damerau-Levenshtein distance between *first* and *second* `string`s" },
		{ "repeat",   runtime_->create_method( &HText::repeat ),   "( *seed*, *count* ) - construct new `string` by repeating *seed* `string` *count* times" },
		{ "hex",      runtime_->create_method( &HText::int_base_to_str, "Text.hex", BASE::HEX ), "( *int* ) - convert *int* value to a `string` using hexadecimal representation" },
		{ "oct",      runtime_->create_method( &HText::int_base_to_str, "Text.oct", BASE::OCT ), "( *int* ) - convert *int* value to a `string` using octal representation" },
		{ "bin",      runtime_->create_method( &HText::int_base_to_str, "Text.bin", BASE::BIN ), "( *int* ) - convert *int* value to a `string` using binary representation" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HText>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Text", &textCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

