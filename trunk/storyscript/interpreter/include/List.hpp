/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file List.hpp
	\brief Declarations for List.
*/

#if !defined(SS_List)
#define SS_List

#include "DLLExport.hpp"
#include "Variable.hpp"
#include "Operator.hpp"
#include <vector>


namespace SS{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The basic all-purpose, dynamic, resizeable, evironment-friendly,
	storyscript list object.
	
	Read all about these in the language manual.
*/
class SS_API List : public VariableBase
{
protected:
	SS_FRIENDIFY_GENERIC_CREATOR(List);
	///Constructor
	List( SS_DECLARE_DEFAULTED_BASE_ARGS );
	
public:
	///Destructor
	~List();

	void AcceptVisitor( ScopeObjectVisitor& );
	
	ListPtr CastToList();
	const ListPtr CastToList() const;

	/**
		\brief Reize the list to a given size.
		
		\param N The given size
	*/
	void Resize( const NumType& N );

	/**
		\brief Add a given VariableBase onto the end of the list.
		
		\param X The object to push onto the list.
		\return The object that was added.
	*/
	VariableBasePtr Push( VariableBasePtr X );
	
	/**
		\brief Remove an object from the end of the list and return it.
		\return The object that was removed.
	*/
	VariableBasePtr Pop();

	/**
		\brief Append another list onto the end of this list.
		
		\param The other list.
	*/
	void AppendList( const ListPtr X );
	
	//ListPtr MakeFlatList() const;
	
	/**
		\brief Pushed the given object-pointer directy, without copying it.
		
		Typically when push is called, a new Variable is created and the
		value of the one passed is copied.  That new variable is then added
		to the end of the list.  This just adds the given pointer to the end
		without making a copy.
		
		\param X The object to push.
	*/
	void PushWithoutCopy( VariableBasePtr X );
	
	/**
		\brief Pushed the given object-pointer directy, without copying it.
		
		Typically when append is called, new Variables are created and the 
		values of the object in the appended list are copied over.  
		This function on the other hand just directly appends the pointer held
		by the other list without making a copies.
		
		\param X The object to push.
	*/
	void AppendListWithoutCopy( ListPtr X );
	
	/**
		\brief Return the value stored at the given index.
		
		Negative value access the list from the back.   If a vlue that is
		>= to the size is used it will either throw an error, or add more
		elements onto the list until it is up to size, depending on what
		LangOpts::UseStrictLists is set to.
		
		\param Index The list index in VariableBase form.
	*/
	VariableBasePtr operator[]( const VariableBasePtr Index );
	
		/**
		\brief Return the value stored at the given index.
		
		Negative value access the list from the back.   If a vlue that is
		>= to the size is used it will either throw an error, or add more
		elements onto the list until it is up to size, depending on what
		LangOpts::UseStrictLists is set to.
		
		\param Index The list index as an unsigned integer.
	*/
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
	virtual ScopeObjectPtr GetScopeObjectHook( const SS::String& );

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
		InternalListFunc( const String& Name, List& Parent )
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

