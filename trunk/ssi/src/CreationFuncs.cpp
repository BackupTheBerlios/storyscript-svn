/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains functions for safely creating derivatives of ScopeObject.
*/

#include "CreationFuncs.hpp"
using namespace SS;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: The proper way to create a variable on the heap.
*/
VariablePointer SS::CreateVariable( SS_DECLARE_BASE_ARGS, const Variable& Foo )
{
	VariablePointer pNewVar( new Variable( SS_BASE_ARGS, Foo ) );
	pNewVar->SetSharedPtr( pNewVar );
	return pNewVar;	
}

VariablePointer SS::CreateVariable( SS_DECLARE_BASE_ARGS, const NumType& Foo )
{
	VariablePointer pNewVar( new Variable( SS_BASE_ARGS, Foo ) );
	pNewVar->SetSharedPtr( pNewVar );
	return pNewVar;	
}

VariablePointer SS::CreateVariable( SS_DECLARE_BASE_ARGS, const StringType& Foo )
{
	VariablePointer pNewVar( new Variable( SS_BASE_ARGS, Foo ) );
	pNewVar->SetSharedPtr( pNewVar );
	return pNewVar;	
}

VariablePointer SS::CreateVariable( SS_DECLARE_BASE_ARGS, const BoolType& Foo )
{
	VariablePointer pNewVar( new Variable( SS_BASE_ARGS, Foo ) );
	pNewVar->SetSharedPtr( pNewVar );
	return pNewVar;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Creation of blocks.
*/
BlockPointer SS::CreateBlock( SS_DECLARE_BASE_ARGS, const Bookmark& Position, unsigned int ListIndex )
{
	BlockPointer pNewBlock( new Block( SS_BASE_ARGS, Position, ListIndex ) );
	pNewBlock->SetSharedPtr( pNewBlock );
	return pNewBlock;		
}



