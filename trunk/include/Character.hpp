#if !defined(SS_Character)
#define SS_Character


#include "Scope.hpp"

namespace SS{

//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Character
// NOTES: The internal representation of a character.  This is a stub, it has
//		  yet to be implemented.
//
class SS_API Character : public Scope
{
public:
	Character();
	Character( const SS::STRING& Name, bool Static = false, bool Const = false );

	void AcceptVisitor( ScopeObjectVisitor& );

private:
	//void RegisterPredefinedVars();
};



} //namespace
#endif

