/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "enumeration.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "hcore/unicode.hxx"
#include "tools/ansi.hxx"
#include "tools/color.hxx"
#include "tools/hterminal.hxx"
#include "tools/keycode.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HKeyPressEventClass : public HClass {
	enumeration::HEnumerationClass::ptr_t _modifierClass;
	enumeration::HEnumerationClass::ptr_t _keyClass;
public:
	HKeyPressEventClass( HRuntime* runtime_, HHuginn::type_id_t typeId_, HClass* origin_ )
		: HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "KeyPressEvent" ),
			"The `KeyPressEvent` represents information about key press event as received by terminal.",
			HHuginn::ACCESS::PUBLIC
		), _modifierClass(
			add_enumeration_as_member(
				this,
				enumeration::create_class(
					runtime_,
					"MODIFIER",
					enumeration::descriptions_t{
						{ "SHIFT",   "SHIFT modifier has been pressed",   static_cast<int>( KEY_CODE::SHIFT_BASE ) },
						{ "CONTROL", "CONTROL modifier has been pressed", static_cast<int>( KEY_CODE::CONTROL_BASE ) },
						{ "META",    "META modifier has beeb pressed",    static_cast<int>( KEY_CODE::META_BASE ) },
						{ "COMMAND", "key press in COMMAND context",      static_cast<int>( KEY_CODE::COMMAND_BASE ) }
					},
					"The `MODIFIER` is a set of known key press modifiers.",
					HHuginn::VISIBILITY::PACKAGE,
					this
				),
				"a set of key press modifiers"
			)
		), _keyClass(
			add_enumeration_as_member(
				this,
				enumeration::create_class(
					runtime_,
					"KEY",
					enumeration::descriptions_t{
						{ "TAB",       "Tab key",          static_cast<int>( KEY_CODE::TAB ) },
						{ "ENTER",     "Enter key",        static_cast<int>( KEY_CODE::ENTER ) },
						{ "BACKSPACE", "Backspace key",    static_cast<int>( KEY_CODE::BACKSPACE ) },
						{ "UP",        "Up Arrow key",     static_cast<int>( KEY_CODE::UP ) },
						{ "DOWN",      "Down Arrow key",   static_cast<int>( KEY_CODE::DOWN ) },
						{ "LEFT",      "Left Arrow key",   static_cast<int>( KEY_CODE::LEFT ) },
						{ "RIGHT",     "Right Arrow key",  static_cast<int>( KEY_CODE::RIGHT ) },
						{ "PAGE_UP",   "Page Up key",      static_cast<int>( KEY_CODE::PAGE_UP ) },
						{ "PAGE_DOWN", "Page Down key",    static_cast<int>( KEY_CODE::PAGE_DOWN ) },
						{ "INSERT",    "Insert key",       static_cast<int>( KEY_CODE::INSERT ) },
						{ "DELETE",    "Delete key",       static_cast<int>( KEY_CODE::DELETE ) },
						{ "HOME",      "Home key",         static_cast<int>( KEY_CODE::HOME ) },
						{ "END",       "End key",          static_cast<int>( KEY_CODE::END ) },
						{ "F1",        "F1 function key",  static_cast<int>( KEY_CODE::F1 ) },
						{ "F2",        "F2 function key",  static_cast<int>( KEY_CODE::F2 ) },
						{ "F3",        "F3 function key",  static_cast<int>( KEY_CODE::F3 ) },
						{ "F4",        "F4 function key",  static_cast<int>( KEY_CODE::F4 ) },
						{ "F5",        "F5 function key",  static_cast<int>( KEY_CODE::F5 ) },
						{ "F6",        "F6 function key",  static_cast<int>( KEY_CODE::F6 ) },
						{ "F7",        "F7 function key",  static_cast<int>( KEY_CODE::F7 ) },
						{ "F8",        "F8 function key",  static_cast<int>( KEY_CODE::F8 ) },
						{ "F9",        "F9 function key",  static_cast<int>( KEY_CODE::F9 ) },
						{ "F10",       "F10 function key", static_cast<int>( KEY_CODE::F10 ) },
						{ "F11",       "F11 function key", static_cast<int>( KEY_CODE::F11 ) },
						{ "F12",       "F12 function key", static_cast<int>( KEY_CODE::F12 ) }
					},
					"The `KEY` is a set of known keys on the terminal's keyboard.",
					HHuginn::VISIBILITY::PACKAGE,
					this
				),
				"a set of known key on the keyboard"
			)
		) {
		M_PROLOG
		HHuginn::field_definitions_t fd{
			{ "code",      runtime_->none_value(), "key pressed" },
			{ "key",       runtime_->none_value(), "the key pressed" },
			{ "modifiers", runtime_->none_value(), "modifiers used" },
			{ "to_string", runtime_->create_method( &HKeyPressEventClass::to_string ), "get `string` representation of this key press event" }
		};
		set_origin( origin_ );
		redefine( nullptr, fd );
		return;
		M_EPILOG
	}
	enumeration::HEnumerationClass const* modifier_class( void ) const {
		return ( _modifierClass.raw() );
	}
	enumeration::HEnumerationClass const* key_class( void ) const {
		return ( _keyClass.raw() );
	}
	static HHuginn::value_t to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "KeyPressEvent.to_string";
		verify_arg_count( name, values_, 0, 0, thread_, position_ );
		hcore::HString s( "KeyPressEvent" );
		HObject& o( *static_cast<HObject*>( object_->raw() ) );
		char const* extra( "(" );
		char const comma[] = ", ";
		for ( int i( 0 ); i < 3; ++ i ) {
			s.append( extra );
			s.append( tools::to_string( o.field_ref( i ), thread_->runtime().huginn() ) );
			extra = comma;
		}
		s.append( ")" );
		return ( thread_->object_factory().create_string( yaal::move( s ) ) );
		M_EPILOG
	}
};

class HTerminal : public HPackage {
	enumeration::HEnumerationClass::ptr_t _attributeClass;
	enumeration::HEnumerationClass::ptr_t _colorClass;
	HHuginn::class_t _keyPressEventClass;
public:
	HTerminal( HClass* class_ )
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
		, _keyPressEventClass() {
		M_PROLOG
		HRuntime* runtime( class_->runtime() );
		_keyPressEventClass = runtime->create_class(
			HRuntime::class_constructor_t(
				[&runtime, class_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HKeyPressEventClass>(
							runtime,
							typeId_,
							class_
						)
					);
				}
			)
		);
		runtime->huginn()->register_class( add_class_as_member( class_, _keyPressEventClass, "Representation of key press event." ) );
		return;
		M_EPILOG
	}
	static HHuginn::value_t is_valid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.is_valid", values_, 0, 0, thread_, position_ );
		return ( thread_->runtime().boolean_value( tools::HTerminal::get_instance().exists() ) );
		M_EPILOG
	}
	static HHuginn::value_t lines( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.lines", values_, 0, 0, thread_, position_ );
		int l( -1 );
		try {
			l = tools::HTerminal::get_instance().size().lines();
		} catch ( hcore::HException const& e ) {
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
		} catch ( hcore::HException const& e ) {
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
		} catch ( hcore::HException const& e ) {
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
				static_cast<HTerminal*>( object_->raw() )->exception_class(),
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
		HEnumeral::value_type val( get_enumeral( values_[0] ) );
		hcore::HString s;
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
		HEnumeral::value_type val( get_enumeral( values_[0] ) );
		hcore::HString s;
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
	static HHuginn::value_t get_character( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.get_character", values_, 0, 0, thread_, position_ );
		code_point_t cp( unicode::CODE_POINT::NUL );
		try {
			cp = tools::HTerminal::get_instance().get_character();
		} catch ( hcore::HException const& e ) {
			thread_->raise( static_cast<HTerminal*>( object_->raw() )->exception_class(), e.what(), position_ );
		}
		return ( thread_->object_factory().create_character( cp ) );
		M_EPILOG
	}
	static HHuginn::value_t get_key( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( "Terminal.get_key", values_, 0, 0, thread_, position_ );
		code_point_t cp( unicode::CODE_POINT::NUL );
		HTerminal* term( static_cast<HTerminal*>( object_->raw() ) );
		HHuginn::values_t data;
		HObjectFactory& of( *thread_->runtime().object_factory() );
		HKeyPressEventClass const* keyPressEventClass( static_cast<HKeyPressEventClass const*>( term->_keyPressEventClass.raw() ) );
		try {
			cp = tools::HTerminal::get_instance().get_key();
			int key( KEY_CODE::key( static_cast<int>( cp.get() ) ) );
			int modifiers( KEY_CODE::modifiers( static_cast<int>( cp.get() ) ) );
			if ( key < KEY_CODE::BASE ) {
				data.push_back( of.create_character( code_point_t( static_cast<code_point_t::value_type>( key ) ) ) );
			} else {
				data.push_back( thread_->runtime().none_value() );
			}
			if ( ( key >= KEY_CODE::BASE ) || ( key == KEY_CODE::TAB ) || ( key == KEY_CODE::ENTER ) || ( key == KEY_CODE::BACKSPACE ) ) {
				data.push_back( keyPressEventClass->key_class()->enumeral( key ) );
			} else {
				data.push_back( thread_->runtime().none_value() );
			}
			if ( modifiers ) {
				huginn::HTuple::values_t m;
				int mods[] = {
					KEY_CODE::COMMAND_BASE, KEY_CODE::META_BASE, KEY_CODE::SHIFT_BASE, KEY_CODE::CONTROL_BASE
				};
				for ( int mod : mods ) {
					if ( modifiers & mod ) {
						m.push_back( keyPressEventClass->modifier_class()->enumeral( mod ) );
					}
				}
				data.push_back( of.create_tuple( yaal::move( m ) ) );
			} else {
				data.push_back( thread_->runtime().none_value() );
			}
			data.push_back( keyPressEventClass->get_default( thread_, 3, position_ ) );
		} catch ( hcore::HException const& e ) {
			thread_->raise( term->exception_class(), e.what(), position_ );
		}
		return ( of.create_object( keyPressEventClass, data ) );
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
		{ "get_character", runtime_->create_method( &HTerminal::get_character ), "read single character from terminal" },
		{ "get_key",   runtime_->create_method( &HTerminal::get_key ),   "read single key press from the keyboard" },
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

