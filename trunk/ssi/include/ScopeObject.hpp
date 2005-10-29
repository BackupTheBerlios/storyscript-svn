/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Anything that can reside in a scope derives from this (Scope).
*/

#if !defined(SS_ScopeObject)
#define SS_ScopeObject

#include "Defines.hpp"
#include "Unicode.hpp"
#include "Types.hpp"
#include "DLLExport.hpp"

#include "ScopeObjectVisitor.hpp"
#include <vector>
#include <map>


namespace SS
{

//

//Typedefs and Enumerations
/*
typedef unsigned int ScopeIndex;
static const ScopeIndex INIT_SCOPEINDEX = ~0U; 
//NOTE: ~0 is, of course, 0xffffffff on 32bit machines
//      I can't imagine there being 4,294,967,295 named objects
//      in the scope, so this should suffice.
	*/
	
//extern VariablePtr INITIAL_UID;
#define SS_INITIAL_UID -1



//This is for a special shared_ptr that I don't want deleted.
struct null_deleter
{
	void operator()(void const *) const
	{
	}
};


class Interpreter;

//A little shortcut, considering this may soon change.
//This allows me the treat the base arguments for ScopeObject constructors as one thing.
#define SS_DECLARE_BASE_ARGS const SS::STRING& BaseName, bool IsStatic, bool IsConst
#define SS_BASE_ARGS BaseName, IsStatic, IsConst
#define SS_BASE_ARGS_DEFAULTS UNNAMMED, false, true
extern SS_API const STRING UNNAMMED;

//
//There are certain classes that should not cast.  Because it is unsafe to do
//so.  (ie. classes that are not created by Creator).  This should be put in
//those classes declarations.
#define SS_CLASS_DOES_NOT_CAST \
	ScopeObjectPtr CastToScopeObject() { return ScopeObject::CastToScope(); }\
	const ScopeObjectPtr CastToScopeObject() const { return ScopeObject::CastToScope(); }\

	virtual ScopePtr CastToScope();
	virtual const ScopePtr CastToScope() const;

	virtual VariableBasePtr CastToVariableBase();
	virtual const VariableBasePtr CastToVariableBase() const;

	virtual VariablePtr CastToVariable();
	virtual const VariablePtr CastToVariable() const;

	virtual BlockPtr CastToBlock();
	virtual const BlockPtr CastToBlock() const;

	virtual ListPtr CastToList();
	virtual const ListPtr CastToList() const;

	virtual OperatorPtr CastToOperator();
	virtual const OperatorPtr CastToOperator() const;




//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject
// NOTES: The base class for any accessable object in a scope.
//
class SS_API ScopeObject
{
protected:
	friend class Creator;
	ScopeObject();
	ScopeObject( const SS::STRING& Name, bool Static = false, bool Const = false );

public:
	virtual ~ScopeObject();

	virtual void AcceptVisitor( ScopeObjectVisitor& );

	bool IsRegistered() const;
	ScopePtr GetParent() const;

	bool IsStatic() const;
	void SetStatic( bool Flag = true );

	bool IsConst() const;
	void SetConst( bool Flag = true );

	void          SetName(const SS::STRING&);
	const SS::STRING& GetName() const;
	SS::STRING        GetFullName() const;
	
	//These two do the same as the above, only they they write to a string
	//provided.  This is a workaround for memory corruption problems
	//involved when using SSI in dll form and not using the multi-threaded DLL
	//form of the standard libraries.  (in windows)
	SS::CHAR* GetName( SS::CHAR* Buffer, unsigned int BufferSize ) const;
	SS::CHAR* GetFullName( SS::CHAR* Buffer, unsigned int BufferSize ) const;

	void SetSharedPtr( const ScopeObjectPtr& );

	void UnRegister();


	//Conversion functions.
	void AssertCastingAllowed() const;
	
	ScopeObjectPtr CastToScopeObject();
	const ScopeObjectPtr CastToScopeObject() const;
	
	virtual ScopePtr CastToScope();
	virtual const ScopePtr CastToScope() const;

	virtual VariableBasePtr CastToVariableBase();
	virtual const VariableBasePtr CastToVariableBase() const;

	virtual VariablePtr CastToVariable();
	virtual const VariablePtr CastToVariable() const;

	virtual BlockPtr CastToBlock();
	virtual const BlockPtr CastToBlock() const;

	virtual ListPtr CastToList();
	virtual const ListPtr CastToList() const;

	virtual OperatorPtr CastToOperator();
	virtual const OperatorPtr CastToOperator() const;

	//To allow easy and safe scope registering.
	friend class Scope;

protected:
	void AssertNonConst();

	SS::STRING mName;
	ScopeObjectPtrWeak mpThis;

	bool mStatic;
	bool mConst;

private:

	void ThrowBadConversion( const STRING& Type, const STRING& Addendum = STRING()  ) const;


	void ZeroVars();

	//These are simply to keep track of the order in which objects were registered on a scope.
	NumType mUniqueID;
	
	Scope* mpParent;
};


}//namespace SS
#endif //if !defined(...

