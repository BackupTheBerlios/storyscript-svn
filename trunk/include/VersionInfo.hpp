/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Just some quick defines for keeping track of the version.
	My versioning system changes every few weeks it seems, but heres
	what I use now:
	
	SSI_VERSION denotes the major revision.  This is incremented when
	I have made significant modifications to the language that seem to
	be stable.  I also give each major version a nickname.  For version
	0 it is "Chernoby".  Don't ask me why.
	
	SSI_BUILNUM is the minor version which I increment every time I make
	a significant bug fix or change.
	
	SSI_PLATFORM should say windows or linux (posix).
	
	SSI_NAME gives a more readable version.
	
	SSI_LASTCOMPILE is the date and time of the last full compile.

*/

#if !defined(SS_VersionInfo)
#define SS_VersionInfo


#include "Unicode.hpp"
#include "Defines.hpp"

namespace SS
{

extern const SS_API SS::STRING SSI_VERSION;
extern const SS_API SS::STRING SSI_BUILDNUM;
extern const SS_API SS::STRING SSI_PLATFORM;
    
extern const SS_API SS::STRING SSI_NAME;

extern const SS_API SS::STRING SSI_LASTCOMPILE;
    
SS_API SS::STRING GetInterpreterVersion();
SS_API SS::STRING GetInterpreterName();
SS_API SS::STRING GetLastCompileTime();
SS_API SS::STRING GetBuildNumber();

}

#endif

