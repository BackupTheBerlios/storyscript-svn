/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file UseNarrowChar.hpp
	Typdefs and defines for when narrow char's are being used.
	
	\see Unicode.hpp UseWideChar.hpp
*/

#if !defined(SS_UseNarrowChar)
#define SS_UseNarrowChar

#include <string>
#include <cstdio>

namespace SS{

/// The standard character type.
typedef char Char;

/// Typedef for STL Strings.
typedef std::string String;

/// Typedef for 
typedef std::fstream FSTREAM;
typedef std::ifstream IFSTREAM;
typedef std::ofstream OFSTREAM;
typedef std::stringstream StringSTREAM;
	



	
//Does nothing in non-unicode
#define TXT

//Override these macros if necessary
#define SS_FILE       __FILE__
#define SS_DATE       __DATE__
#define SS_TIME       __TIME__
#define SS_FUNC       __FUNCTION__
#define SS_TIMESTAMP  __TIMESTAMP__
#define SS_LINE       __LINE__


inline unsigned int STRLEN( const Char* s ){
	return (unsigned int)strlen( s );
}



/*
All these functions are for interfacing with stuff that uses
normal characters.  These functions don't really do anything
when we too are using normal characters.
*/

inline const SS::String& NormalizeString( const std::string& x ){
	return x;
}

inline SS::String& NormalizeString( std::string& x ){
	return x;
}

inline SS::String NormalizeString( const char* x ){
	return SS::String( x );
}


inline const std::string& NarrowizeString( const SS::String& x ){
	return x;
}

inline std::string& NarrowizeString( SS::String& x ){
	return x;
}

inline std::string NarrowizeString( const Char* x ){
	return std::string(x);
}


}
#endif

