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
public:
	List();
	List( const SS::STRING&, bool Static = false, bool Const = false );
	~List();

	void AcceptVisitor( ScopeObjectVisitor& );
	
	ListPointer GetListPtr();
	const ListPointer GetListPtr() const;

	void Resize( const NumType& );

	VariableBasePointer Push( VariableBasePointer );
	VariableBasePointer Pop();

	void Append( const ListPointer );
	
	void AppendWithoutCopy( VariableBasePointer );
	void AppendListWithoutCopy( ListPointer );
	
	ListPointer MakeFlatList() const;

	VariableBasePointer operator[]( const VariableBasePointer Index );
	VariableBasePointer operator[]( unsigned int Index );
	
	void operator=( const List& );
	VariableBasePointer operator=(const VariableBase&);
	void CopyExactly( const ListPointer );

	VariableBasePointer Remove( const VariableBasePointer Index );
	VariableBasePointer Insert( const VariableBasePointer Index ); 

	VariableBasePointer Length() const;

	const ListType& GetInternalList() const;
	ListType& GetInternalList();
	
	StringType GetStringData() const;
	NumType GetNumData() const;
	BoolType GetBoolData() const;
	
	VariablePointer GetVariablePtr();
	const VariablePointer GetVariablePtr() const;

	//virtual VariableBasePointer op_not() const;



private:
	void RegisterPredefinedVars();
	unsigned int DetermineRealIndex( const VariableBase& Index );
	
	VariablePointer MakeVariable() const;

	ListType mList;
};


extern ListPointer gpEmptyList;



}//namespace
#endif

