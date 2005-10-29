/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Declaratios for the List class; a dynamicaly sized list.
*/

#if !defined(SS_List)
#define SS_List

#include "DLLExport.hpp"
#include "Variable.hpp"
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
	List();
	List( const SS::STRING&, bool Static = false, bool Const = false );
	
public:
	~List();

	void AcceptVisitor( ScopeObjectVisitor& );
	
	ListPtr CastToList();
	const ListPtr CastToList() const;

	void Resize( const NumType& );

	VariableBasePtr Push( VariableBasePtr );
	VariableBasePtr Pop();

	void Append( const ListPtr );
	
	void AppendWithoutCopy( VariableBasePtr );
	void AppendListWithoutCopy( ListPtr );
	
	ListPtr MakeFlatList() const;

	VariableBasePtr operator[]( const VariableBasePtr Index );
	VariableBasePtr operator[]( unsigned int Index );
	
	void operator=( const List& );
	VariableBasePtr operator=(const VariableBase&);
	void CopyExactly( const ListPtr );

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



private:
	void RegisterPredefinedVars();
	unsigned int DetermineRealIndex( const VariableBase& Index );
	
	VariablePtr MakeVariable() const;

	ListType mList;
};


extern ListPtr gpEmptyList;



}//namespace
#endif

