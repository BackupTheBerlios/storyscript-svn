/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Macros: can't live with 'em, can't live without 'em.
*/

#if !defined(SS_Macros)
#define SS_Macros

//Macros obviously don't obey namespaces.  This is just for consistency.
namespace SS{

//A little shortcut, considering this may soon change.
//This allows me the treat the base arguments for ScopeObject constructors as one thing.
#define SS_DECLARE_DEFAULTED_BASE_ARGS const SS::STRING& BaseName = SS::STRING(), bool IsConst = false
#define SS_DECLARE_BASE_ARGS const SS::STRING& BaseName, bool IsConst
#define SS_BASE_ARGS BaseName, IsConst
#define SS_BASE_ARGS_DEFAULTS UNNAMMED, false







}


#endif

