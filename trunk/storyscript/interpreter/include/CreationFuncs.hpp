/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file CreationFuncs.hpp
	\brief Shortcut functions for correctly creating ScopeObjects and derivatives. 
*/

#if !defined(SS_CreationFuncs)
#define SS_CreationFuncs

#include "Macros.hpp"
#include "Types.hpp"
//#include "Interpreter.hpp"
//#include "ScopeObject.hpp"
//#include "Bookmark.hpp"

namespace SS{

struct Bookmark;
class Interpreter;

/**
	\page CreationFuncs Creation Functions
	
	If you read about StoryScripts 
	\ref SelfCasting "Self-Casting Kung-Fu" 
	you would know that derivatives of ScopeObject require complex creation.
	One way to accomplish this is with external creation functions.
	
	These are templated functions that handle all the dirty work in one call.
	
	They are: CreateGeneric, CreateBasic, CreateVariable, CreateBlock
	
	The only way they differ from each other are their parameters.  These are
	templated function so you have to pass the type.
	
	For example:  
	CreateGeneric<Scope>( "Foo", false )  or  CreateVariable<Variable>( "Foo", false, "Some string." )
	
	Most of the objects in storyscript can't be created directly through their
	constructor (they are protected).  This is for your own protection as objects
	that are not properly created will throw exceptions when you try to cast them.
	 
	
	\sa CreateGeneric CreateBasic CreateVariable CreateBlock	
*/

/**
	\brief Macro of befriending the CreateGeneric function.
	
	Add this to your class declaration to allow it to be created by CreateGeneric.
	
	\param x The name of your class
*/
#define SS_FRIENDIFY_GENERIC_CREATOR(x) \
friend boost::shared_ptr<x> CreateGeneric<x> ( SS_DECLARE_BASE_ARGS )

/**
	\brief Macro of befriending the CreateBasic function.
	
	Add this to your class declaration to allow it to be created by CreateBasic.
	
	\param x The name of your class
*/
#define SS_FRIENDIFY_BASIC_CREATOR(x) \
friend boost::shared_ptr<x> CreateBasic<x> ()

/**
	\brief Macro of befriending the CreateVariable function.
	
	Add this to your class declaration to allow it to be created by CreateVariable.
	
	\param x The name of your class
*/
#define SS_FRIENDIFY_VARIABLE_CREATOR(x) \
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const Variable& );\
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const NumType& );\
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const StringType& );\
friend boost::shared_ptr<x> CreateVariable<x> ( SS_DECLARE_BASE_ARGS, const BoolType& )

/**
	\brief Macro of befriending the CreateBlock function.
	
	Add this to your class declaration to allow it to be created by CreateBlock.
	
	\param x The name of your class
*/
#define SS_FRIENDIFY_BLOCK_CREATOR(x) \
friend boost::shared_ptr<x> CreateBlock<x> ( SS_DECLARE_BASE_ARGS, Interpreter& I, \
const Bookmark& Position, BlockIndex ListIndex )


/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param I A reference to the interpreter.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateGeneric( Interpreter& I );


/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
	\param Const Whether the object is constant (modifiable) or not.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateGeneric( const SS::STRING& Name = SS::STRING(),
                                    bool Const = false );
/**
	\brief Create new instances of ScopeObject derivatives.
	
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern boost::shared_ptr<T> CreateBasic( );


/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
	\param Const Whether the object is constant (modifiable) or not.
	\param V Reference to a variable to copy.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateVariable( const SS::STRING& Name = SS::STRING,
                                     bool Const, const Variable& V );

/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
	\param Const Whether the object is constant (modifiable) or not.
	\param N Reference to a NumType to initialize the variable.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const NumType& N );

/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
	\param Const Whether the object is constant (modifiable) or not.
	\param S Reference to a StringType to initialize the variable.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const StringType& S );

/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
	\param Const Whether the object is constant (modifiable) or not.
	\param B Reference to a BoolType to initialize the variable.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, const BoolType& B );


/**
	\brief Create new instances of ScopeObject derivatives.
	
	\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
	\param Const Whether the object is constant (modifiable) or not.
	\param I A reference to the Interpreter in use.
	\param Position A bookmark to the position of the blocks body.
	\param Index The index of the block, as assigned by the Interpreter.
	\return A shared pointer to the new object.
	
	\sa CreationFuncs
*/
template <typename T > extern
boost::shared_ptr<T> CreateBlock( SS_DECLARE_BASE_ARGS, Interpreter& I, const Bookmark& Position, BlockIndex Index );

}//end namespace SS
#endif


