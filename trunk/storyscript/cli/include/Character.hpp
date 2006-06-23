/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains declarations for the Character class.
*/

#if !defined(SS_Character)
#define SS_Character


#include "Scope.hpp"

namespace SS{

//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Character
// NOTES: The internal representation of a character.  This is a stub, it has
//		  yet to be implemented.
//
class SS_API Character : public Scope
{
public:
	void AcceptVisitor( ScopeObjectVisitor& );

protected:
	SS_FRIENDIFY_GENERIC_CREATOR(Character);
	Character();
	Character( const SS::STRING& Name, bool Static = false, bool Const = false );
	//void RegisterPredefinedVars();
};



} //namespace
#endif

