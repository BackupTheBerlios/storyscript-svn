/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Scope.hpp
	\brief Declarations for Scope.  
*/


#if !defined(SS_Scope)
#define SS_Scope

#include "Unicode.hpp"
#include "ScopeObject.hpp"
#include "ParserAnomaly.hpp"
#include "Types.hpp"
#include "Word.hpp"


namespace SS{
	
	
//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/**
	\brief Base class for any objects that have their own scope.
	
	This includes most language objects, as most object in storyscript have
	some kind of attributes of some sort.
	
*/
class SS_API Scope : public ScopeObject
{
public:
	SS_FRIENDIFY_GENERIC_CREATOR(Scope);
	
	/// Destructor
	virtual ~Scope() {};
	
	/**
		\brief Return the Scope's doc(ument) string.
		
		Read the section on doc strings in the storyscript lagnuage
		manual if this doesn't make sense.
		
		\return The Scope's doc(ument) string.
	*/
	STRING& GetDocString();

	virtual void AcceptVisitor( ScopeObjectVisitor& );

	/**
		\brief Retrieve a named object from the scope.
		
		The name may be a complex name, in which case the scope will
		travel up the heirarchy searching for a match.
		
		The function is simply an alias for Scope::GetScopeObject.
		
		\param Name The name of the the object to be retrieved.
		\return A pointer to the retrieved object.
	*/
	ScopeObjectPtr operator[]( const SS::CompoundString& Name );
	
	/**
		\brief Register an object with the given name in the Scope.
		
		If the object already is registered with another Scope it will
		automatically be un-registered before being registered.
		
		\param Obj A pointer to he object to be registered.
		\return The registered object.
	*/
	ScopeObjectPtr Register  ( ScopeObjectPtr Obj );
	
	/**
		\brief Unregister an object with the given name.
		
		The object must be simple name (non-compound) and belonging to
		the scope in question.
		
		\param Name The name of the the object to un-register.
		\return A pointer to the object that was un-registered.
	*/
	ScopeObjectPtr UnRegister( const SS::STRING& Name );

	/// Unregister all object from the scope.
	void Clear();

	/**
		\brief Checks if a object exists in the local scope.
		
		This searches only the local scope (and imported scoeps)
		to see if an object is registered by that name.
		
		\param ID A simple (non-compound) object name.
		\return True if the object is in the scope, false otherwise.
	*/ 
	bool Exists( const SS::STRING& ID );
	
	/**
		\brief Retrieve a named object from the scope.
		
		The name may be a complex name, in which case the scope will
		travel up the heirarchy searching for a match.
		
		\param Name The name of the the object to be retrieved.
		\return A pointer to the retrieved object.
	*/
	ScopeObjectPtr GetScopeObject( const SS::CompoundString& Name );
	
	/**
		\brief Retrieve a named object from the scope. (Non-throwing)
		
		This function is the same as Scope::GetScopeObject with the exception
		that if the object is not found, instead of throwing an anomaly,
		it return a null pointer.  This is handy mainly for performance
		reasons.
		
		\sa Scope::GetScopeObject
				
		\param Name The name of the the object to be retrieved.
		\return A pointer to the retrieved object.
	*/
	virtual ScopeObjectPtr GetScopeObject_NoThrow( const SS::CompoundString& Identifier, unsigned long Level = 0 );
	
	/**
		\brief Retrieve a name from this scope and only this scope.
		
		The name must be a simple (non-compound) name and only
		the local scope is searched.  (It will not travel up the
		heiarchy.  
		
		The function is provided for performance and convenience reasons.
		
		\param Name The name of the the object to be retrieved.
		\return A pointer to the retrieved object.
	*/
	ScopeObjectPtr GetScopeObjectLocal( const SS::STRING& Name );
	
	
	/**
		\brief Retrieve a name from this scope and only this scope.
		
		This function is the same as Scope::GetScopeObject with the exception
		that if the object is not found, instead of throwing an anomaly,
		it return a null pointer.  This is handy mainly for performance
		reasons.
		
		\sa Scope::GetScopeOjbectLocal
		
		\param Name The name of the the object to be retrieved.
		\return A pointer to the retrieved object.
	*/
	ScopeObjectPtr GetScopeObjectLocal_NoThrow( const SS::STRING& Name );
	
	/**
		\brief Import a scope into the current scope.
		
		When a scope is "imported" it behaves like a magical portal
		between scopes, so that when the local scope is searched for
		an object, it will also check the imported scopes before
		traveling up the heiarchy.
		
		Read more about this in the storyscript language manual.
		
		\sa Scope::UnImport
		
		\param Obj The scope to be imported.
	*/
	void Import( ScopePtr Obj );
	
	/**
		\brief Un-import a scope that has been imported.
		
		\sa Scope::Import
		
		\param Obj The scope to be un-imported.
	*/
	void UnImport( ScopePtr Obj );
	
	ScopePtr CastToScope();
	const ScopePtr CastToScope() const;

protected:

	/// Constructor
	Scope( const SS::STRING& Name = SS::STRING(), bool Const = false );
	
	/**
		\brief Overridable hook fucntion for GetScopeObject
		
		This function is called by GetScopeObject before it searches for an
		object.  This is provided so this class as well as child classes can
		spring certain object into existance right before the first time they
		get called on.
		
		This can save tremendous amounts of memory and time, so if you are
		encouraged to override it in your class.  Please remember to
		call the parents version when you version is done, or your
		dog will get hit by a truck.
	*/
	virtual ScopeObjectPtr GetScopeObjectHook( const STRING& );


private:
	/// Used by the contructor to initialize private data.
	void RegisterPredefinedVars();
	
	//typedef boost::shared_ptr<const STRING> StringPtr;
	//typedef std::vector<StringPtr> TokenizedID;
	//ScopeObjectPtr GetScopeObjectInternal( const CompoundString& Identifier, unsigned long Level = 0 );
	//void SplitUpID( const STRING& ID, TokenizedID& TokenizedID );


	/**
		\brief Return a reference to the global scope.
		
		The function simply travels up the scope heiarchy until a dead
		end is reached, then returns it.  So it is possible the the scope
		returned isn't _the_ global scope if this is called on a free
		floating object or group of objects.
		
		\return A reference to the global scope.
	*/ 
	Scope& GetGlobalScope();
		
	/// The internal list of registered objects.
	ScopeListType mList;
	
	/// The internal list of imported scopes.
	std::vector< ScopePtr > mImportedScopes;
	
	bool mNameCreated; ///< Flag marking whether the name variable has been created yet.
	bool mFullNameCreated; ///< Flag marking whether the fullname (magic) variable has been created yet.
	bool mDocStringCreated; ///< Flag marking whether the doc string variable has been created yet.
};


	
}
#endif

