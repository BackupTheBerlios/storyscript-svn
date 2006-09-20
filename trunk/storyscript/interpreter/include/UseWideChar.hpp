/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file UseWideChar.hpp
	Typdefs and defines for when wide char's are being used.
	
	\see Unicode.hpp UseNarrowChar.hpp
*/

#if !defined(SS_UseWideChar)
#define SS_UseWideChar

#include <stdexcept>
#include <wchar.h>

namespace SS{

/// The standard character type.
typedef wchar_t CHAR;
typedef std::wstring STRING;
typedef std::wfstream FSTREAM;
typedef std::wifstream IFSTREAM;
typedef std::wofstream OFSTREAM;
typedef std::wstringstream STRINGSTREAM;


//Text macro for strings literals
//use it like: TXT("Hello World")
#define WIDEN(x) TXT(x)
#define TXT(quote) L ## quote

inline unsigned int STRLEN( const CHAR* s ){
	return wcslen( s );
}


std::string NarrowString( const wchar_t* wide );
std::wstring WidenString( const char* narrow );





inline SS::STRING NormalizeString( const std::string& x ){
	return WidenString( x.c_str() );
}

inline SS::STRING NormalizeString( const char* x ){
	return WidenString( x );
}


inline std::string NarrowizeString( const SS::STRING& x ){
	return NarrowString( x.c_str() );
}

inline std::string NarrowizeString( const CHAR* x ){
	return NarrowString( x );
	
	

//Override these macros if necessary
#define SS_FILE       WIDEN(__FILE__)
#define SS_DATE       WIDEN(__DATE__)
#define SS_TIME       WIDEN(__TIME__)
//I'm not exactly sure why, but this macro doesn't work the same as the others.
//That's why I have to use NormalizeString instead of WIDEN.
#define SS_FUNC       NormalizeString(__FUNCTION__)
#define SS_TIMESTAMP  WIDEN(__TIMESTAMP__)
#define SS_LINE       __LINE__

}


}
#endif

