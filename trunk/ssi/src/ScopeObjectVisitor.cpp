/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: An implementation of the Visitor Pattern for quickly adding
	special operations onto ScopeObjects.
*/

#include "ScopeObjectVisitor.hpp"
#include "ScopeObject.hpp"
#include "Character.hpp"
#include "Block.hpp"
#include "ParserAnomaly.hpp"
#include "Variable.hpp"



using namespace SS;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 GetScopeObjectType
 NOTES: A wrapper for the TypeCheckVisitor
*/
ScopeObjectType SS::GetScopeObjectType( ScopeObjectPointer pSO )
{
	TypeCheckVisitor TypeChecker;
	pSO->AcceptVisitor( TypeChecker );
	return TypeChecker.ReturnType();
}




//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TypeCheckVisitor:: Visit*
// NOTES: Functions that set mType to the type that of the visited
//		  ScopeObject.
//
void TypeCheckVisitor::VisitScopeObject(ScopeObject* pO){
	mType = SCOPEOBJ_SCOPEOBJECT;
}

void TypeCheckVisitor::VisitScope(Scope* pO){
	mType = SCOPEOBJ_SCOPE;
}

void TypeCheckVisitor::VisitVariableBase(VariableBase*){
	mType = SCOPEOBJ_VARIABLEBASE;
}

void TypeCheckVisitor::VisitVariable(Variable* pO){
	mType = SCOPEOBJ_VARIABLE;
}

void TypeCheckVisitor::VisitBlock(Block* pO){
	mType = SCOPEOBJ_BLOCK;
}

void TypeCheckVisitor::VisitCharacter(Character*){
	mType = SCOPEOBJ_CHARACTER;
}

void TypeCheckVisitor::VisitList( List* ){
	mType = SCOPEOBJ_LIST;
}

void TypeCheckVisitor::VisitOperator( Operator* ){
	mType = SCOPEOBJ_OPERATOR;
}

//~~~~~~~STRUCTURE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TypeCheckVisitor::ReturnType
// NOTES: Returns the type of the last visited 
//
ScopeObjectType TypeCheckVisitor::ReturnType() const
{
	return mType;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 TypeCheckVisitor::ReturnTypeString
 NOTES: Returns a string with the name of the type.  Useful for error output and stuff.
*/
SS::STRING TypeCheckVisitor::ReturnTypeString() const
{
	SS::STRING S;


	//I could just use a list with the type as an index, but this is much safer.

	switch( mType ){
		case SCOPEOBJ_NULL:		    S = TXT("NullObject");   break;
		case SCOPEOBJ_SCOPEOBJECT:  S = TXT("ScopeObject");  break;
		case SCOPEOBJ_SCOPE:	    S = TXT("Scope");        break;
		case SCOPEOBJ_BLOCK:	    S = TXT("Block");        break;
		case SCOPEOBJ_CHARACTER:    S = TXT("Character");    break;
		case SCOPEOBJ_VARIABLEBASE: S = TXT("VariableBase"); break;
		case SCOPEOBJ_VARIABLE:		S = TXT("Variable");     break;
		case SCOPEOBJ_LIST:	        S = TXT("List");         break;

		default: S = TXT("UnkownType");
	}

	return S;
}

