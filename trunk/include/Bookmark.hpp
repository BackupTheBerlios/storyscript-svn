/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains declarations for the base Bookmark class.
*/

#if !defined(SS_Bookmark)
#define SS_Bookmark

#include "Types.hpp"
#include "ReaderSource.hpp"
#include "DLLExport.hpp"

namespace SS{

/*~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bookmark
NOTES: Stores the a specific location in a script file to allow the interpreter
to jump back and forth without breaking anything.
*/
struct SS_API Bookmark
{
	Bookmark();
	Bookmark( const SS::STRING& FileName, ReaderPos Position,
			  unsigned long Line, ScopePointer CurrentScope = ScopePointer(),
			  ScopePointer CurrentStaticScope = ScopePointer() );

	bool IsVoid() const;

	SS::STRING FileName;
	ReaderPos Position;
	unsigned long Line;
	ScopePointer CurrentScope;
	ScopePointer CurrentStaticScope;
};

}//namespace SS
#endif

