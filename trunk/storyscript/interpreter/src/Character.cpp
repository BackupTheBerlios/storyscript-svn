/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Character defs.
*/

#include "Character.hpp"
#include "DLLExport.hpp"

using namespace SS;




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Character::Character
NOTES: Adds all the predefined variables
*/
Character::Character( SS_DECLARE_BASE_ARGS )
: Scope(SS_BASE_ARGS)
{
}




//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Character::AcceptVisitor
// NOTES: ...
//
void Character::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitCharacter(this);
}
