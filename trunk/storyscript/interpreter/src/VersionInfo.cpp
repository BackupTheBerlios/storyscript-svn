/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Just some quick defines for keeping track of the version.
	See the header for details.
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

const SS::CHAR* SS::SSI_MAJORVERSION = TXT("SS-1 : Hindenburg");
//const SS::STRING SS::SSI_BUILDNUM( TXT("0 (Safe for now.)") );

//This is platform business is really silly how it is.
const SS::CHAR* SS::SSI_PLATFORM = 
	#if defined(PLAT_WIN32)
		TXT("Win32");
	#elif defined(PLAT_LINUX)
		TXT("Linux");
	#else
		TXT("Unkown Platform")
	#endif 
	
const SS::CHAR* SS::SSI_COMPILEDATE = 
	__DATE__ "  " __TIME__;



SS::STRING VersionString;



SS_API const SS::CHAR* SS::GetVersionString()
{
	static bool BuildVersionString = true;
	
	if( BuildVersionString )
	{
		VersionString =
			STRING(SSI_MAJORVERSION) + STRING(TXT("  (")) + STRING(SSI_PLATFORM) + 
			STRING(TXT(")\nSVN Revision: ")) + STRING(SSI_SVNRevision) +
			STRING(TXT("  ::  Compiled: ")) + STRING(SSI_COMPILEDATE);
		
		BuildVersionString = false;
	}
	
	return VersionString.c_str();
}


