/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "Bookmark.hpp"
using namespace SS;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Bookmark::Bookmark()
	: Position(0), Line(0)
{
}

Bookmark::Bookmark( const SS::String& FileName,
					ReaderPos Position,
				    unsigned long Line,
					ScopePtr CurrentScope /*= ScopePtr()*/,
					ScopePtr CurrentStaticScope /*= ScopePtr()*/ )
	: FileName(FileName),
      Position(Position),
	  Line(Line),
	  CurrentScope(CurrentScope),
	  CurrentStaticScope(CurrentStaticScope)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Bookmark::IsVoid() const
{
	if( FileName.empty() && Position == 0 && Line == 0 ) return true;
	else return false;	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Bookmark::operator==( const Bookmark& Other ) const
{
	if( FileName == Other.FileName &&
		Position == Other.Position &&
		Line     == Other.Line /*&&
		CurrentScope       == Other.CurrentScope &&
		CurrentStaticScope == Other.CurrentStaticScope*/ )	
	{
		return true;
	}
	else return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Bookmark& Bookmark::operator=( const Bookmark& Other )
{
	FileName = Other.FileName;
	Position = Other.Position;
	Line = Other.Line;
	CurrentScope = Other.CurrentScope;
	CurrentStaticScope = Other.CurrentStaticScope;
	
	return *this;	
}

