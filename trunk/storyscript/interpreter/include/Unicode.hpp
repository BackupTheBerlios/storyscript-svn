/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Unicode.hpp
	\brief Code for compatibility with wide characters (ie. Unicode).
	
	This file is mostly just a wrapper for either UseWideChar.hpp or
	UseNarrowChar.hpp (depending on the wheter USING_UNICODE is set or not).
	
	\see UseWideChar.hpp UseNarrowChar.hpp USING_UNICODE	
*/


#if !defined(SS_Unicode)
#define SS_Unicode

#include "Defines.hpp"
#include <string> 


/*
Use the following switch to toggle unicode.
You can define it somewhere else, but just make
sure it gets defined before the first inclusion
of this file.  Otherwise, you are screwed.
*/
//#define USING_UNICODE


#if defined(USING_UNICODE)
#include "UseWideChar.hpp"
#else //NOT USING_UNICODE
#include "UseNarrowChar.hpp"
#endif//defined(USING_UNICODE)

namespace SS{

/**
	A char-size agnostic version of strcpy.
	
	\param dest Destination string.
	\param src Source string.
	\param dest_size Size of the destination string.
*/
SS_API Char* STRCPY( Char* dest, const Char* src, unsigned int dest_size );


}
	

/*
int STRLEN( const Char* Strng );

Char TOUPPER( const Char C );
String TOUPPER( const Char* S );
String TOUPPER( const String& S );

Char TOLOWER( const Char C );
String TOLOWER( const Char* S );
String TOLOWER( const String& S );

String SS_API StringCAST( const std::string& str );
String SS_API StringCAST( const char* str );

std::string SS_API stringCAST( const String& str );
std::string SS_API stringCAST( const Char* str );
*/

#endif //!defined(SS_Unicode)

