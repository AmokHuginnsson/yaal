find_package( Threads REQUIRED )
find_package( LibXml2 REQUIRED )
find_package( LibXslt REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( ZLIB REQUIRED )
find_package( Curses REQUIRED )
string( REPLACE "libcurses." "libncursesw." CURSES_LIBRARIES "${CURSES_LIBRARIES}" )

