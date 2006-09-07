/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Declaratios for the List class; a dynamicaly sized list.
*/

#if !defined(SS_List)
#define SS_List

#include "DLLExport.hpp"
#include "Variable.hpp"
#include "Operator.hpp"
#include <vector>


namespace SS{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 List
 NOTES: 
*/
class SS_API List : public VariableBase
{
protected:
	SS_FRIENDIFY_GENERIC_CREATOR(List);
	List( SS_DECLARE_DEFAULTED_BASE_ARGS );
	
public:
	~List();

	void AcceptVisitor( ScopeObjectVisitor& );
	
	ListPtr CastToList();
	const ListPtr CastToList() const;

	void Resize( const NumType& );

	VariableBasePtr Push( VariableBasePtr );
	VariableBasePtr Pop();

	void AppendList( const ListPtr );
	
	//ListPtr MakeFlatList() const;
	void PushWithoutCopy( VariableBasePtr );
	void AppendListWithoutCopy( ListPtr );

	VariableBasePtr operator[]( const VariableBasePtr Index );
	VariableBasePtr operator[]( unsigned int Index );
	
	void operator=( const List& );
	VariableBasePtr operator=(const VariableBase&);
	//void CopyExactly( const ListPtr );

	VariableBasePtr Remove( const VariableBasePtr Index );
	VariableBasePtr Insert( const VariableBasePtr Index ); 

	VariableBasePtr Length() const;

	const ListType& GetInternalList() const;
	ListType& GetInternalList();
	
	StringType GetStringData() const;
	NumType GetNumData() const;
	BoolType GetBoolData() const;
	
	VariablePtr CastToVariable();
	const VariablePtr CastToVariable() const;
	
	//virtual VariableBasePtr op_not() const;

protected:
	virtual ScopeObjectPtr GetScopeObjectHook( const SS::STRING& );

private:
	void RegisterPredefinedVars();
	bool mPopCreated, mPushCreated, mRemoveAllCreated,
		 mRemoveCreated, mLengthCreated;
	
	
	
	unsigned int DetermineRealIndex( const VariableBase& Index );
	
	VariablePtr MakeVariable() const;

	ListType mList;
	
	//This are built in list functions that I am moving from SLib.
	//It is syntactically simpler to have them members of list,
	//rather than having to pass a reference to an external list.
	class InternalListFunc : public Operator
	{
		public:
		InternalListFunc( const STRING& Name, List& Parent )
		: Operator( Name, true ), mParentList( Parent )
		{}
		
		protected:
		List& mParentList;
		
		private:
		InternalListFunc();			
	};
	
	class RemoveOp : public InternalListFunc
	{
		public:
		RemoveOp( List& Parent );
		VariableBasePtr Operate( VariableBasePtr );
	};
	
	class RemoveAllOp : public InternalListFunc
	{
		public:
		RemoveAllOp( List& Parent );
		VariableBasePtr Operate( VariableBasePtr );
		
	};
	
	class PushOp : public InternalListFunc
	{
		public:
		PushOp( List& Parent );
		VariableBasePtr Operate( VariableBasePtr );
		
	};
	
	class PopOp : public InternalListFunc
	{
		public:
		PopOp( List& Parent );
		VariableBasePtr Operate( VariableBasePtr );
		
	};
	
};


extern ListPtr gpEmptyList;



}//namespace
#endif

