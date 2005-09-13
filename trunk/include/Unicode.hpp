/*
	Unicode.hpp
	----------------------
	Created: Thu, Jan 29 2004, 1:48
	Description: This is the DCJ standard unicode header.
	
	TODO:
	-Write alternative unicode functions.
	
	Changelog:
	
*/

#if !defined(SS_Unicode)
#define SS_Unicode

#include "Defines.hpp"
#include <string> 


/*
Use the following swith to toggle unicode.
You can define it somewhere else, but just make
sure it gets defiend before the first includsion
of this file.  Otherwise, you are screwed.
*/
//#define USING_UNICODE


#if defined(USING_UNICODE)
#include "UseWideChar.hpp"
#else //NOT USING_UNICODE
#include "UseNarrowChar.hpp"
#endif//defined(USING_UNICODE)

namespace SS{

SS_API CHAR* STRCPY( CHAR* dest, const CHAR* src, unsigned int dest_size );


}
	

/*
int STRLEN( const CHAR* Strng );

CHAR TOUPPER( const CHAR C );
STRING TOUPPER( const CHAR* S );
STRING TOUPPER( const STRING& S );

CHAR TOLOWER( const CHAR C );
STRING TOLOWER( const CHAR* S );
STRING TOLOWER( const STRING& S );

STRING SS_API STRINGCAST( const std::string& str );
STRING SS_API STRINGCAST( const char* str );

std::string SS_API stringCAST( const STRING& str );
std::string SS_API stringCAST( const CHAR* str );
*/

#endif //!defined(SS_Unicode)

