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
			  unsigned long Line, ScopePtr CurrentScope = ScopePtr(),
			  ScopePtr CurrentStaticScope = ScopePtr() );

	bool IsVoid() const;
	
	bool operator==( const Bookmark& ) const;
	Bookmark& operator=( const Bookmark& );

	SS::STRING FileName;
	ReaderPos Position;
	unsigned long Line;
	ScopePtr CurrentScope;
	ScopePtr CurrentStaticScope;
};

}//namespace SS
#endif

