/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The base Scope class.
*/


#if !defined(SS_Scope)
#define SS_Scope

#include "Unicode.hpp"
#include "ScopeObject.hpp"
#include "ParserAnomaly.hpp"
#include "Types.hpp"


namespace SS{
	
	
//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope
// NOTES: Basically a list of a bunch of ScopeObjects
//
class SS_API Scope : public ScopeObject
{
public:
	SS_FRIENDIFY_GENERIC_CREATOR(Scope);
	virtual ~Scope() {};

	virtual void AcceptVisitor( ScopeObjectVisitor& );

	ScopeObjectPtr operator[]( const SS::STRING& );
	
	void Register  ( ScopeObjectPtr );
	void UnRegister( const SS::STRING& );
	//void UnRegister( ScopeObjectPtr );

	void Clear();

	bool Exists( const SS::STRING& ID );

	BlockPtr GetNextBlock( BlockPtr );

	ScopeObjectPtr GetScopeObject_NoThrow( const SS::STRING& );
	ScopeObjectPtr GetScopeObject( const SS::STRING& );
	
	void Import( ScopePtr );
	void UnImport( ScopePtr );
	
	ScopePtr CastToScope();
	const ScopePtr CastToScope() const;

protected:
	Scope();
	Scope( const SS::STRING& Name, bool Static = false, bool Const = false );


private:
	void RegisterPredefinedVars();

	Scope& GetGlobalScope();

	

	//typedef __gnu_cxx::hash_map< SS::STRING, ScopeObjectPtr > ScopeListType;
	
	ScopeListType mList;
	
	std::vector< ScopePtr > mImportedScopes;

};


/*
Variable( const SS::STRING& Name, const Variable&,
		  bool Static = false, bool Const = false );
	
	Variable( const SS::STRING& Name, const NumType&,
			  bool Static = false, bool Const = false );
	
	Variable( const SS::STRING& Name, const StringType&, 
			  bool Static = false, bool Const = false );
	
	Variable( const SS::STRING& Name, const BoolType&, 
			  bool Static = false, bool Const = false );
*/


	
	
}
#endif

