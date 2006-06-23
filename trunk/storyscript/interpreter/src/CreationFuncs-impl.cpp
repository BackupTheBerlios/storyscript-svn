/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Implementation of various creation function templates enumerated here.
*/

#include "ScopeObject.hpp"
#include "Scope.hpp"
#include "Variable.hpp"
#include "Block.hpp"
#include "List.hpp"
#include "Character.hpp"

#include "Slib-Common.hpp"
#include "Slib-Time.hpp"
#include "Slib-List.hpp"
#include "Slib-Math.hpp"

#include "BuiltInFunctions.hpp"


#include "CreationFuncs.cpp"

using namespace SS;

#define Implement_CreateGeneric(x) template boost::shared_ptr<x> SS::CreateGeneric<x> ( SS_DECLARE_BASE_ARGS );\
template boost::shared_ptr<x> SS::CreateGeneric<x> ()

#define Implement_CreateGeneric_NoArgs(x) template boost::shared_ptr<x> SS::CreateGeneric<x> ()

#define Implement_CreateVariable(x) template boost::shared_ptr<x> SS::CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const Variable& );\
template boost::shared_ptr<x> SS::CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const NumType& );\
template boost::shared_ptr<x> SS::CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const StringType& );\
template boost::shared_ptr<x> SS::CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const BoolType& )

#define Implement_CreateBlock(x) template boost::shared_ptr<x> SS::CreateBlock<x> ( SS_DECLARE_BASE_ARGS, const Bookmark& Position, unsigned int ListIndex )

Implement_CreateGeneric(ScopeObject);
Implement_CreateGeneric(Scope);
Implement_CreateGeneric(List);
Implement_CreateGeneric(Character);
Implement_CreateGeneric_NoArgs(SLib::Common);
Implement_CreateGeneric_NoArgs(SLib::List);
Implement_CreateGeneric_NoArgs(SLib::Math);
Implement_CreateGeneric_NoArgs(SLib::Time);
Implement_CreateGeneric_NoArgs(SLib::LangOpts);
Implement_CreateGeneric_NoArgs(ImportOperator);
Implement_CreateGeneric_NoArgs(UnImportOperator);
Implement_CreateGeneric_NoArgs(ReturnOperator);

Implement_CreateVariable(Variable);

Implement_CreateBlock(Block);

