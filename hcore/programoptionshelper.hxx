/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/programoptionshelper.hxx
 *
 * \brief Helpers for HProgramOptionsHandler class.
 *
 * It contains helper getters/setters for program options.
 */

#ifndef YAAL_HCORE_PROGRAMOPTIONSHELPER_HXX_INCLUDED
#define YAAL_HCORE_PROGRAMOPTIONSHELPER_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/harray.hxx"
#include "hcore/hdeque.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hset.hxx"
#include "hcore/hhashset.hxx"
#include "hcore/base.hxx"

namespace yaal {

/*! \brief Helper functions for program options handling infrastructure.
 */
namespace program_options_helper {

template<typename tType>
void set_option_value_from_string( tType& object, yaal::hcore::HString const& value );

template<typename tType>
yaal::hcore::HString get_string_from_option_value( tType const& value_ ) {
	M_PROLOG
	return ( lexical_cast<yaal::hcore::HString>( value_ ) );
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( yaal::hcore::HArray<tType>& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	object.push_back( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( yaal::hcore::HDeque<tType>& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	object.push_back( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( yaal::hcore::HList<tType>& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	object.push_back( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( yaal::hcore::HSet<tType>& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	object.insert( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

template<typename tType>
void set_option_value_from_string( yaal::hcore::HHashSet<tType>& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	object.insert( lexical_cast<tType>( value ) );
	return;
	M_EPILOG
}

/*! \brief Set variable value based on string.
 */
template<typename tType>
void set_option_value_from_string( tType& object, yaal::hcore::HString const& value ) {
	M_PROLOG
	object = lexical_cast<tType>( value );
	return;
	M_EPILOG
}

int reload_configuration( void );

}

}

#endif /* #ifndef YAAL_HCORE_PROGRAMOPTIONSHELPER_HXX_INCLUDED */

