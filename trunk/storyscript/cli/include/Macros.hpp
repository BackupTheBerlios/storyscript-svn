/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Macros: can't live with 'em, can't live without 'em.
*/

#if !defined(SS_Macros)
#define SS_Macros

//This is really pointless, macros won't obey it.
//Its just for aestetics really.
namespace SS{

//A little shortcut, considering this may soon change.
//This allows me the treat the base arguments for ScopeObject constructors as one thing.
#define SS_DECLARE_BASE_ARGS const SS::STRING& BaseName, bool IsStatic, bool IsConst
#define SS_BASE_ARGS BaseName, IsStatic, IsConst
#define SS_BASE_ARGS_DEFAULTS UNNAMMED, false, true







}


#endif

