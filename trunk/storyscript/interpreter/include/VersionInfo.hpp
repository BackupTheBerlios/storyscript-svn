/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file VersionInfo.hpp
	\brief Several strings containing information about the interpreter version.
*/

#if !defined(SS_VersionInfo)
#define SS_VersionInfo


#include "Unicode.hpp"
#include "Defines.hpp"

namespace SS
{

/**
	\brief The major version of the interpreter.
	
	This major version system is a little odd.  A single number is used as well
	as a nickname.  This number is incremented and (the name is changed) when
	the interpreter has reached some kind of milestone (ie. Arbitrarily, whenever
	I feel like it).  The name is always some disaster.  Version 0 was called
	Chernobyl.  Version 1 is Hindenberg.
*/
extern const SS::Char* SSI_MAJORVERSION;

/**
	\brief The platform the interpreter has been compiled for.
	
	Right now it just says windows or linux, which is very
	dubious and quite franky you shouldn't really use this
	for anything as it is unreliable.
*/
extern const SS_API SS::Char* SSI_PLATFORM;

///The date at which the interpreter was compiled
extern const SS_API SS::Char* SSI_COMPILEDATE;

/**
	\brief The SVN revision.  This is the best way to keep track of versions.
	
	This gets set in SVNRevision.cpp which is automatically generated at each compile.
*/
extern const SS_API SS::Char* SSI_SVNRevision;

///Outputs a nice fancy string containing the major version, platform, date, and svn revision.
SS_API const SS::Char* GetVersionString();


}




#endif

