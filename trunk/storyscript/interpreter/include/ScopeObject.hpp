/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Word.hpp
	\Declarations for ScopeObject.  
*/


#if !defined(SS_ScopeObject)
#define SS_ScopeObject

#include "Defines.hpp"
#include "Unicode.hpp"
#include "Types.hpp"
#include "DLLExport.hpp"

#include "Macros.hpp"
#include "CreationFuncs.hpp"

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
	


//This is for a special shared_ptr that I don't want deleted.
struct null_deleter
{
	void operator()(void const *) const
	{
	}
};


class Interpreter;



extern SS_API const STRING UNNAMMED;

//
//There are certain classes that should not cast.  Because it is unsafe to do
//so.  (ie. classes that are not created by Creator).  This should be put in
//those classes declarations.

/*
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
	
*/





/**
	\page SelfCasting Self-Casting Kung-Fu
	
	In storyscript types are meant to be very loose and easily converted
	between each other.  This is usually achieved by calling one of the
	ScopeObject:CastTo____ functions.  This allows cool things.  For
	example, you can cast a ScopeObject to a Variable.  Since the ScopeObject
	itself of course can't be simply _cast_, instead a new Variable object
	is created holding the name of the ScopeObject.  The casting interface
	is all virtual functions, so the behavior of the functions can be overrided.
	Casting a ScopeObject to a Variable is different that casting a List to a
	Variable.
	
	This system is convenient and can be very powerful but it does lead to a few complications.
	
	When a "CastTo" function is called it sometimes simply down-casts
	(or up-casts if possible) itself, but other times it needs to create
	an entirely new object.  This leads to the problem of knowing whether
	or not an object returned needs to be deleted or not.
	
	The most convenient solution, the one that is used, is using reference
	counted pointers (boost::shared_ptr's).  This solution requires objects
	maintain a shared_ptr to itself, for when it simply casts itself.
	However this isn't possible without	a two-stage construction.  First
	the object is created, and then the 	it's shared_ptr is set 
	(using ScopeObject::SetSharedPtr).
	
	The user doesn't have to worry about this as long as she uses the creation
	functions (CreateGeneric, CreateVariable, CreateBlock, etc. ).  In fact, most
	of the standard objects _must_ be created with these functions; their constructors
	are private.
	
	If one makes a derived object and doesn't sets the shared_ptr, it will work fine
	unless the object tries to cast itself, in which case it will throw an anomaly.
	
	\sa ScopeObject::SetSharedPtr ScopeObject::CastToScopeObject ScopeObject::CastToScope
	\sa ScopeObject::CastToBlock ScopeObject::CastToList ScopeObject::CastToOperator
*/





//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/**
	\brief Base class for any object that can reside in a scope.
	
	All variables, blocks, and anything else in-language that can have a name and
	reside in a scope derives from this class.
	
	Note that ScopeObjects and derivatives typically should be created with the
	various creation functions (CreateGereric, etc).  This ensures that they
	are properly garbage collected.
*/
class SS_API ScopeObject
{
protected:
	SS_FRIENDIFY_GENERIC_CREATOR(ScopeObject);
	
	/**
		\brief Constructor
		
		\param Name The name of the object.  It should obey storyscript
		identifier naming rules if you want to be able to actually access it.
		\param Const Whether the object is constant (modifiable) or not. 
		
	*/
	ScopeObject( const SS::STRING& Name = SS::STRING(), bool Const = false );

public:
	/// Destructor
	virtual ~ScopeObject();
	
	
	/**
		\brief Used to accept visitor derivatives.
		
		\see ScopeObjectVisitor
	*/
	virtual void AcceptVisitor( ScopeObjectVisitor& V );

	/**
		\brief Returns true if registered.
		
		\return True if the object is registered to a scope, false otherwise.
	*/
	bool IsRegistered() const;
	
	/**
		\brief Returns a pointer the object's parent.
		
		If the object is not registered the pointer will be NULL.  Be sure
		to check if it is!  Or at least call IsRegistered first!
		
		\return A pointer to object' parent.
	*/
	ScopePtr GetParent() const;

	/**
		\brief Returns true if the object is constant.
	*/
	bool IsConst() const;
	
	/**
		\brief Turns the const flag on or off.
		
		\param Flag What to set the const flag to. 
	*/
	void SetConst( bool Flag = true );

	/**
		\brief Sets the name of the object.
		
		Names are not checked for validity, but if you want the 
		object to be accessible choose a name that conforms to
		storyscript identifier naming rules.
		
		Note that if the object has been registered it will be
		UnRegistered and then register itself again.
		
		\param S The new name for the oject.
	*/
	void          SetName(const SS::STRING& S);
	
	/**
		\brief Returns the name of the object.
	*/
	const SS::STRING& GetName() const;
	
	/**
		\brief Returns the full name of the object.
		
		For instance, ":test_ssconv:foo:bar".
	*/
	SS::STRING        GetFullName() const;
	
	/**
		\brief Overloaded GetName for c-style strings.
		
		This overload exists mainly because if a DLLs is linked with the single-threaded
		standard library, passing objects around causes bad things.  Or something like that.		
		
		\param Buffer The output string.
		\param BufferSize The size of the output buffer.
		\return The Buffer
	*/
	SS::CHAR* GetName( SS::CHAR* Buffer, unsigned int BufferSize ) const;
	
	/**
		\brief Overloaded GetFullName for c-style strings.
		
		This overload exists mainly because if a DLLs is linked with the single-threaded
		standard library, passing objects around causes bad things.  Or something like that.		
		
		\param Buffer The output string.
		\param BufferSize The size of the output buffer.
		\return The Buffer
	*/
	SS::CHAR* GetFullName( SS::CHAR* Buffer, unsigned int BufferSize ) const;

	/**
		\brief Set the internal shared_ptr of itself.
		
		Read about 
		\ref SelfCasting "Self-Casting Kung-Fu" 
		if this sounds confusing.
		
		 \param This A boost::shared_ptr holding a copy of this object.  If you try to
		 give it some other shared_ptr the four horemen of the Apocalypse will appear
		 marking the end times.
	*/
	void SetSharedPtr( const ScopeObjectPtr& This );

	/**
		\brief Un-register the object from its scope.
		
		This is the equivalent of calling Scope::UnRegister on this object.
		
		Nothing will happen if this is called on a unregistered object.
	*/
	void UnRegister();

	/// Casts itself to a ScopeObject
	ScopeObjectPtr CastToScopeObject();
	/// Cast to a ScopeObject (Const Version)
	const ScopeObjectPtr CastToScopeObject() const;
	
	/// Casts itself to a Scope
	virtual ScopePtr CastToScope();
	/// Casts itself to a ScopeO (Const Version)
	virtual const ScopePtr CastToScope() const;

	/// Casts itself to a VariableBase
	virtual VariableBasePtr CastToVariableBase();
	/// Casts itself to a VariableBase (Const Version)
	virtual const VariableBasePtr CastToVariableBase() const;
	
	/// Casts itself to a Variable
	virtual VariablePtr CastToVariable();
	/// Casts itself to a Variable (Const Version)
	virtual const VariablePtr CastToVariable() const;

	/// Casts itself to a Block
	virtual BlockPtr CastToBlock();
	/// Casts itself to a Block (Const Version)
	virtual const BlockPtr CastToBlock() const;
	
	/// Casts itself to a List
	virtual ListPtr CastToList();
	/// Casts itself to a List (Const Version)
	virtual const ListPtr CastToList() const;

	/// Casts itself to an Operator
	virtual OperatorPtr CastToOperator();
	/// Casts itself to an Operator (Const Version)
	virtual const OperatorPtr CastToOperator() const;

	/// To allow easy and safe scope registering.
	friend class Scope;

protected:
	/**
		\brief Throws an anomaly if an object is constant.
		
		Derived classes should be sure to call this before any operation
		that would modify the object.
	*/		
	void AssertNonConst() const;
	
	/**
		\brief Throws an anomaly if the object cannot be cast.
		
		Usually this mean, that the internal shared_ptr hasn't been set. 
		
		Read about 
		\ref SelfCasting "Self-Casting Kung-Fu" 
		if this sounds confusing.
	*/
	void AssertCastingAllowed() const;

	/// The object's name.
	SS::STRING mName;
	
	/**
		\brief This internal this shared_ptr.
		
		See \ref SelfCasting "Self-Casting Kung-Fu".
	*/
	ScopeObjectPtrWeak mpThis;
	
	/// The constant flag.
	bool mConst;

private:

	/**
		\brief Throws a bad conversion anomaly.
		
		This is called by the CastTo function when the cast is impossible.  
	*/
	void ThrowBadConversion( const STRING& Type, const STRING& Addendum = STRING()  ) const;

	/// Used by the constructors to set internals to their initial values.
	void ZeroVars();
	
	/// A pointer the objects parent.  (NULL if unregistered)
	Scope* mpParent;
};


}//namespace SS
#endif //if !defined(...

