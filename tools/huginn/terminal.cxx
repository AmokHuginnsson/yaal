/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "enumeration.hxx"
#include "tools/ansi.hxx"
#include "tools/color.hxx"
#include "tools/hterminal.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HTerminal : public HPackage {
	enumeration::HEnumerationClass::ptr_t _attributeClass;
	enumeration::HEnumerationClass::ptr_t _colorClass;
	HHuginn::class_t _exceptionClass;
public:
	HTerminal( HHuginn::HClass* class_ )
		: HPackage( class_ )
		, _attributeClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"ATTRIBUTE",
					enumeration::descriptions_t{
						{ "BOLD",          "**bold** font attribute (use bright intensity colors)", static_cast<int>( COLOR::ATTR_BOLD ) },
						{ "UNDERLINE",     "**underline** font attribute",                          static_cast<int>( COLOR::ATTR_UNDERLINE ) },
						{ "REVERSE",       "**reverse**d colors attribute",                         static_cast<int>( COLOR::ATTR_REVERSE ) },
						{ "BLINK",         "**blink**ing text mode attribute",                      static_cast<int>( COLOR::ATTR_BLINK ) },
						{ "RESET",         "reset all attributes and colors",                       static_cast<int>( COLOR::ATTR_RESET ) }
					},
					"The `ATTRIBUTE` is a set of known terminal attributes.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"a set of known terminal attributes"
			)
		)
		, _colorClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"COLOR",
					enumeration::descriptions_t{
						{ "BLACK",         "black color",               static_cast<int>( COLOR::FG_BLACK ) },
						{ "RED",           "low intensity red color",   static_cast<int>( COLOR::FG_RED ) },
						{ "GREEN",         "low intensity green color", static_cast<int>( COLOR::FG_GREEN ) },
						{ "BROWN",         "brown color",               static_cast<int>( COLOR::FG_BROWN ) },
						{ "BLUE",          "low intensity blue color",  static_cast<int>( COLOR::FG_BLUE ) },
						{ "MAGENTA",       "low magenta color",         static_cast<int>( COLOR::FG_MAGENTA ) },
						{ "CYAN",          "low intensity cyan color",  static_cast<int>( COLOR::FG_CYAN ) },
						{ "LIGHTGRAY",     "light gray color",          static_cast<int>( COLOR::FG_LIGHTGRAY ) },
						{ "GRAY",          "dark gray color",           static_cast<int>( COLOR::FG_GRAY ) },
						{ "BRIGHTRED",     "bright red color",          static_cast<int>( COLOR::FG_BRIGHTRED ) },
						{ "BRIGHTGREEN",   "bright green color",        static_cast<int>( COLOR::FG_BRIGHTGREEN ) },
						{ "YELLOW",        "yellow color",              static_cast<int>( COLOR::FG_YELLOW ) },
						{ "BRIGHTBLUE",    "bright blue color",         static_cast<int>( COLOR::FG_BRIGHTBLUE ) },
						{ "BRIGHTMAGENTA", "bright magenta color",      static_cast<int>( COLOR::FG_BRIGHTMAGENTA ) },
						{ "BRIGHTCYAN",    "bright cyan color",         static_cast<int>( COLOR::FG_BRIGHTCYAN ) },
						{ "WHITE",         "white color",               static_cast<int>( COLOR::FG_WHITE ) }
					},
					"The `COLOR` is a set of colors from basic color palette.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"a set of color from basic color palette"
			)
		)
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t is_valid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.is_valid", values_, 0, 0, thread_, position_ );
		return ( tools::HTerminal::get_instance().exists() ? thread_->runtime().true_value() : thread_->runtime().false_value() );
		M_EPILOG
	}
	static HHuginn::value_t lines( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.lines", values_, 0, 0, thread_, position_ );
		int l( -1 );
		try {
			l = tools::HTerminal::get_instance().size().lines();
		} catch ( HException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
		}
		return ( thread_->object_factory().create_integer( l ) );
		M_EPILOG
	}
	static HHuginn::value_t columns( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.columns", values_, 0, 0, thread_, position_ );
		int c( -1 );
		try {
			c = tools::HTerminal::get_instance().size().columns();
		} catch ( HException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
		}
		return ( thread_->object_factory().create_integer( c ) );
		M_EPILOG
	}
	static HHuginn::value_t move( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Terminal.move", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, thread_, position_ );
		tools::HTerminal::HSize s;
		try {
			s = tools::HTerminal::get_instance().size();
		} catch ( HException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
		}
		int row( safe_int::cast<int>( get_integer( values_[0] ) ) );
		int column( safe_int::cast<int>( get_integer( values_[1] ) ) );
		if ( ( row < 0 ) || ( column < 0 ) ) {
			throw HHuginn::HHuginnRuntimeException(
				"Invalid "_ys.append( row < 0 ? "row: " : "column: " ).append( row < 0 ? row : column ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		if ( ( row >= s.lines() ) || ( column >= s.columns() ) ) {
			thread_->raise(
				static_cast<HTerminal*>( object_->raw() )->_exceptionClass.raw(),
				"Invalid "_ys.append( row >= s.lines() ? "row: " : "column: " ).append( row >= s.lines() ? row : column ),
				position_
			);
		}
		return ( thread_->object_factory().create_string( *ansi::move( row + 1, column + 1 ) ) );
		M_EPILOG
	}
	static HHuginn::value_t color( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HTerminal& t( *static_cast<HTerminal*>( object_->raw() ) );
		verify_signature_by_class( "Terminal.color", values_, { t._colorClass->enumeral_class(), thread_->object_factory().string_class() }, thread_, position_ );
		HHuginn::HEnumeral::value_type val( get_enumeral( values_[0] ) );
		HString s;
		switch ( static_cast<COLOR::color_t>( val ) ) {
			case ( COLOR::FG_BLACK ):         s.assign( *ansi::black );         break;
			case ( COLOR::FG_RED ):           s.assign( *ansi::red );           break;
			case ( COLOR::FG_GREEN ):         s.assign( *ansi::green );         break;
			case ( COLOR::FG_BROWN ):         s.assign( *ansi::brown );         break;
			case ( COLOR::FG_BLUE ):          s.assign( *ansi::blue );          break;
			case ( COLOR::FG_MAGENTA ):       s.assign( *ansi::magenta );       break;
			case ( COLOR::FG_CYAN ):          s.assign( *ansi::cyan );          break;
			case ( COLOR::FG_LIGHTGRAY ):     s.assign( *ansi::lightgray );     break;
			case ( COLOR::FG_GRAY ):          s.assign( *ansi::gray );          break;
			case ( COLOR::FG_BRIGHTRED ):     s.assign( *ansi::brightred );     break;
			case ( COLOR::FG_BRIGHTGREEN ):   s.assign( *ansi::brightgreen );   break;
			case ( COLOR::FG_YELLOW ):        s.assign( *ansi::yellow );        break;
			case ( COLOR::FG_BRIGHTBLUE ):    s.assign( *ansi::brightblue );    break;
			case ( COLOR::FG_BRIGHTMAGENTA ): s.assign( *ansi::brightmagenta ); break;
			case ( COLOR::FG_BRIGHTCYAN ):    s.assign( *ansi::brightcyan );    break;
			case ( COLOR::FG_WHITE ):         s.assign( *ansi::white );         break;
			default: {
				M_ASSERT( !"Invalid code path."[0] );
			}
		}
		s.append( get_string( values_[1] ) );
		s.append( *ansi::reset );
		return ( thread_->object_factory().create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t attribute( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HTerminal& t( *static_cast<HTerminal*>( object_->raw() ) );
		verify_signature_by_class( "Terminal.attribute", values_, { t._attributeClass->enumeral_class(), thread_->object_factory().string_class() }, thread_, position_ );
		HHuginn::HEnumeral::value_type val( get_enumeral( values_[0] ) );
		HString s;
		switch ( static_cast<COLOR::color_t>( val ) ) {
			case ( COLOR::ATTR_BOLD ):      s.assign( *ansi::bold );      break;
			case ( COLOR::ATTR_UNDERLINE ): s.assign( *ansi::underline ); break;
			case ( COLOR::ATTR_REVERSE ):   s.assign( *ansi::reverse );   break;
			case ( COLOR::ATTR_BLINK ):     s.assign( *ansi::blink );     break;
			case ( COLOR::ATTR_RESET ):     s.assign( *ansi::reset );     break;
			default: {
				M_ASSERT( !"Invalid code path."[0] );
			}
		}
		s.append( get_string( values_[1] ) );
		s.append( *ansi::reset );
		return ( thread_->object_factory().create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t seq_str( char const* name_, char const* seq_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 0, 0, thread_, position_ );
		return ( thread_->object_factory().create_string( seq_ ) );
		M_EPILOG
	}
};

namespace package_factory {

class HTerminalCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} terminalCreator;

HPackageCreatorInterface::HInstance HTerminalCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Terminal",
			"The `Terminal` package provides terminal (console) manipulation routines.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "is_valid",  runtime_->create_method( &HTerminal::is_valid ),  "tell if current standard I/O stream represents a valid terminal device" },
		{ "lines",     runtime_->create_method( &HTerminal::lines ),     "get number of lines for current terminal" },
		{ "columns",   runtime_->create_method( &HTerminal::columns ),   "get number of columns for current terminal" },
		{ "attribute", runtime_->create_method( &HTerminal::attribute ), "( *attr*, *str* ) - wrap given `string` *str* with given attribute *attr*" },
		{ "color",     runtime_->create_method( &HTerminal::color ),     "( *color*, *str* ) - wrap given `string` *str* with given *color*" },
		{ "move",      runtime_->create_method( &HTerminal::move ),      "( *row*, *column* ) - move cursor to a new position indicated by *row* and *column*" },
		{ "clear_to_eol", runtime_->create_method( &HTerminal::seq_str, "Terminal.clear_to_eol", *ansi::clrtoeol ), "clear content of the current line til the end" },
		{ "clear",     runtime_->create_method( &HTerminal::seq_str, "Terminal.clear",   *ansi::clear ),   "clear content of the terminal" },
		{ "save",      runtime_->create_method( &HTerminal::seq_str, "Terminal.save",    *ansi::save ),    "save current cursor position" },
		{ "restore",   runtime_->create_method( &HTerminal::seq_str, "Terminal.restore", *ansi::restore ), "restore previously saved cursor position" },
		{ "reset",     runtime_->create_method( &HTerminal::seq_str, "Terminal.reset",   *ansi::reset ),   "perform reset current terminal" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HTerminal>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Terminal", &terminalCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

