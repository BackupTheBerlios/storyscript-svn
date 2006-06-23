/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: These are a bunch of defines and typedefs that get included when unicode
	mode is turned off.
*/

#if !defined(SS_UseNarrowChar)
#define SS_UseNarrowChar

#include <string>
#include <cstdio>

namespace SS{

	
typedef char CHAR;
typedef std::string STRING;
typedef std::fstream FSTREAM;
typedef std::ifstream IFSTREAM;
typedef std::ofstream OFSTREAM;
typedef std::stringstream STRINGSTREAM;
	



	
//Does nothing in non-unicode
#define TXT

//Overide these macros if necessary
#define SS_FILE       __FILE__
#define SS_DATE       __DATE__
#define SS_TIME       __TIME__
#define SS_FUNC       __FUNCTION__
#define SS_TIMESTAMP  __TIMESTAMP__
#define SS_LINE       __LINE__


inline unsigned int STRLEN( const CHAR* s ){
	return (unsigned int)strlen( s );
}



/*
All these functions are for interfacing with stuff that uses
normal characters.  These functions don't really do anything
when we too are using normal characters.
*/

inline const SS::STRING& NormalizeString( const std::string& x ){
	return x;
}

inline SS::STRING& NormalizeString( std::string& x ){
	return x;
}

inline SS::STRING NormalizeString( const char* x ){
	return SS::STRING( x );
}


inline const std::string& NarrowizeString( const SS::STRING& x ){
	return x;
}

inline std::string& NarrowizeString( SS::STRING& x ){
	return x;
}

inline std::string NarrowizeString( const CHAR* x ){
	return std::string(x);
}


}
#endif

