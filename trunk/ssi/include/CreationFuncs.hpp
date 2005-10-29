/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains functions for safely creating derivatives of ScopeObject.
*/

#if !defined(SS_CreationFuncs)
#define SS_CreationFuncs

#include "Macros.hpp"
#include "Types.hpp"
//#include "ScopeObject.hpp"
//#include "Bookmark.hpp"

namespace SS{

class Bookmark;

/*
Gotta love macros.

These are shortcuts for making classe that only may be created
by the creators.  (Which is the case with most of the base
classes. (Do ad ';' at the end of these.  KDevelop gets confused
if I have them at the end and don't include a ';'.
*/
#define SS_FRIENDIFY_GENERIC_CREATOR(x) friend boost::shared_ptr<x> CreateGeneric<x> ( SS_DECLARE_BASE_ARGS );\
friend boost::shared_ptr<x> CreateGeneric<x> ()

#define SS_FRIENDIFY_VARIABLE_CREATOR(x) friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const Variable& );\
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const NumType& );\
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const StringType& );\
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const BoolType& )

#define SS_FRIENDIFY_BLOCK_CREATOR(x) friend boost::shared_ptr<x> CreateBlock<x> ( SS_DECLARE_BASE_ARGS, const Bookmark& Position, unsigned int ListIndex )

template <typename T > extern
boost::shared_ptr<T> CreateGeneric();

template <typename T > extern
boost::shared_ptr<T> CreateGeneric( SS_DECLARE_BASE_ARGS );

template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const Variable& );

template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const NumType& );

template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const StringType& );

template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const BoolType& );

template <typename T > extern
boost::shared_ptr<T> CreateBlock( SS_DECLARE_BASE_ARGS, const Bookmark& Position, BlockIndex Index );

}//end namespace SS
#endif


