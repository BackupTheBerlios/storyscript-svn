/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains functions for safely creating derivatives of ScopeObject.
*/


using namespace SS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Creation of generic objects that derive from ScopeObject.
*/
template< typename T = ScopeObject >
boost::shared_ptr<T> CreateGeneric( SS_DECLARE_BASE_ARGS )
{
	boost::shared_ptr<T> pNewObj( new T( SS_BASE_ARGS ) );
	pNewObj->SetSharedPtr( pNewObj );
	
	return pNewObj;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Creation of objects that are initialized with the same interface as
 		Block.
*/
template< typename T = Block >
boost::shared_ptr<T> CreateBlock(  SS_DECLARE_BASE_ARGS,
									const Bookmark& Position,
									unsigned int ListIndex )
{
	boost::shared_ptr<T> pNewBlock( new T( SS_BASE_ARGS, Position, ListIndex ) );
	pNewBlock->SetSharedPtr( pNewBlock );
	
	return pNewBlock;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Creation of variables and similar objects.
*/
template< typename T = Variable >
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, Variable& Var )
{
	boost::shared_ptr<T> pNewVar( new T( SS_BASE_ARGS, Var ) );
	pNewVar->SetSharedPtr( pNewVar );
		
	return pNewVar;
}

template< typename T = Variable >
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, NumType& Num )
{
	boost::shared_ptr<T> pNewVar( new T( SS_BASE_ARGS, Num ) );
	pNewVar->SetSharedPtr( pNewVar );
		
	return pNewVar;
}

template< typename T = Variable >
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, StringType& String )
{
	boost::shared_ptr<T> pNewVar( new T( SS_BASE_ARGS, String ) );
	pNewVar->SetSharedPtr( pNewVar );
		
	return pNewVar;
}

template< typename T = Variable >
boost::shared_ptr<T> CreateVariable( SS_DECLARE_BASE_ARGS, BoolType& Boolean )
{
	boost::shared_ptr<T> pNewVar( new T( SS_BASE_ARGS, Boolean ) );
	pNewVar->SetSharedPtr( pNewVar );
		
	return pNewVar;
}




