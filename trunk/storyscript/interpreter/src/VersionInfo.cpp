/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "VersionInfo.hpp"

using namespace SS;

/*
For future versions, here are some notable distasters:

Lusitania
Titanic
Pompey
...
*/

const SS::Char* SS::SSI_MAJORVERSION = TXT("SS-1 : Hindenburg");
//const SS::String SS::SSI_BUILDNUM( TXT("0 (Safe for now.)") );

//This is platform business is really silly how it is.
const SS::Char* SS::SSI_PLATFORM = 
	#if defined(PLAT_WIN32)
		TXT("Win32");
	#elif defined(PLAT_LINUX)
		TXT("Linux");
	#else
		TXT("Unkown Platform")
	#endif 
	
const SS::Char* SS::SSI_COMPILEDATE = 
	__DATE__ "  " __TIME__;



SS::String VersionString;



SS_API const SS::Char* SS::GetVersionString()
{
	static bool BuildVersionString = true;
	
	if( BuildVersionString )
	{
		VersionString =
			String(SSI_MAJORVERSION) + String(TXT("  (")) + String(SSI_PLATFORM) + 
			String(TXT(")\nSVN Revision: ")) + String(SSI_SVNRevision) +
			String(TXT("  ::  Compiled: ")) + String(SSI_COMPILEDATE);
		
		BuildVersionString = false;
	}
	
	return VersionString.c_str();
}


