#include <vector>
#include <set>
#include <string>
#include <sstream>

namespace string_helper {

template<typename T>
void add_element( std::set<T>& s_, T const& e_ )
	{ s_.insert( e_ ); }

template<typename T>
void add_element( std::vector<T>& s_, T const& e_ )
	{ s_.push_back( e_ ); }

template<typename T>
void tokenize( std::string const& str, T& tokens, std::string const& delimiters = " \t") {
	/* Skip delimiters at beginning. */
	std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	/* Find first "non-delimiter". */
	std::string::size_type pos     = str.find_first_of( delimiters, lastPos );

	while ( ( std::string::npos != pos ) || ( std::string::npos != lastPos ) ) {
		/* Found a token, add it to the vector. */
		add_element( tokens, str.substr( lastPos, pos - lastPos ) );
		/* Skip delimiters.  Note the "not_of" */
		lastPos = str.find_first_not_of( delimiters, pos );
		/* Find next "non-delimiter" */
		pos = str.find_first_of( delimiters, lastPos );
	}
}

}

template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& from ) {
	std::stringstream ss;
	ss << from;
	to_t to;
	ss >> to;
	return to;
}
