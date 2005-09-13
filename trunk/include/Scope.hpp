


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
	Scope();
	Scope( const SS::STRING& Name, bool Static = false, bool Const = false );

	virtual ~Scope() {};

	virtual void AcceptVisitor( ScopeObjectVisitor& );

	ScopeObjectPointer operator[]( const SS::STRING& );
	
	void Register  ( ScopeObjectPointer );
	void UnRegister( const SS::STRING& );
	//void UnRegister( ScopeObjectPointer );

	void Clear();

	bool Exists( const SS::STRING& ID );

	BlockPointer GetNextBlock( BlockPointer );

	ScopeObjectPointer GetScopeObject_NoThrow( const SS::STRING& );
	ScopeObjectPointer GetScopeObject( const SS::STRING& );
	
	void Import( ScopePointer );
	void UnImport( ScopePointer );
	
	ScopePointer GetScopePtr();
	const ScopePointer GetScopePtr() const;
	
	void RegisterPredefinedVars();

private:
	Scope& GetGlobalScope();

	

	//typedef __gnu_cxx::hash_map< SS::STRING, ScopeObjectPointer > ScopeListType;
	
	ScopeListType mList;
	
	std::vector< ScopePointer > mImportedScopes;

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

