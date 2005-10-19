/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: An implementation of the Visitor Pattern for quickly adding
	special operations onto ScopeObjects.
*/

#if !defined(SS_ScopeObjectVisitor)
#define SS_ScopeObjectVisitor


#include "Types.hpp"
#include "Defines.hpp"
#include "Unicode.hpp"

namespace SS{



//~~~~~~~INTERFACE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObjectVisitor
// NOTES: 
//
class ScopeObjectVisitor
{
public:
	virtual void VisitScopeObject (ScopeObject*) = 0;
	virtual void VisitScope       (Scope*) = 0;
	virtual void VisitVariableBase(VariableBase*) = 0;
	virtual void VisitVariable    (Variable*) = 0;
	virtual void VisitCharacter   (Character*) = 0;
	virtual void VisitBlock       (Block*) = 0;
	virtual void VisitList        (List*) = 0;
	virtual void VisitOperator    (Operator*) = 0;

private:
};




//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TypeCheckVisitor
// NOTES: Checks the type of a ScopeObject.  (Without any crappy RTTI stuff).
//
class TypeCheckVisitor : public ScopeObjectVisitor
{
public:
	TypeCheckVisitor() : mType(SCOPEOBJ_NULL) {}

	void VisitScopeObject (ScopeObject*);
	void VisitScope       (Scope*);
	void VisitVariableBase(VariableBase*);
	void VisitVariable    (Variable*);
	void VisitCharacter   (Character*);
	void VisitBlock       (Block*);
	void VisitList        (List*);
	void VisitOperator    (Operator*);

	ScopeObjectType ReturnType() const;
	SS::STRING ReturnTypeString() const;

private:
	ScopeObjectType mType;
};


ScopeObjectType GetScopeObjectType( ScopeObjectPointer );


}
#endif

