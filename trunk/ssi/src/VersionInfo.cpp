/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Just some quick defines for keeping track of the version.
	See the header for details.
*/

#include "VersionInfo.hpp"

using namespace SS;


const SS::STRING SS::SSI_VERSION( TXT("0 (Chernobyl)") );
const SS::STRING SS::SSI_BUILDNUM( TXT("23") );
const SS::STRING SS::SSI_PLATFORM = 
	#if defined(PLAT_WIN32)
		TXT("Win32");
	#elif defined(PLAT_LINUX)
		TXT("Linux");
	#endif

const SS::STRING SS::SSI_NAME = (SS::STRING)(TXT("SS-")) + (SSI_VERSION) + (SS::STRING)(TXT(" Build: ")) + SSI_BUILDNUM +
							 (SS::STRING)(TXT("  ")) + SSI_PLATFORM;

const SS::STRING SS::SSI_LASTCOMPILE = 
	(SS::STRING)(SS_DATE) + (SS::STRING)(TXT("  ")) +
	(SS::STRING)(SS_TIME);


SS::STRING SS::GetInterpreterVersion(){
	return SSI_VERSION;
}

SS::STRING SS::GetInterpreterName(){
	return SSI_NAME;
}

SS::STRING SS::GetLastCompileTime(){
	return SSI_LASTCOMPILE;
}

SS::STRING SS::GetBuildNumber(){
	return SSI_BUILDNUM;
}

