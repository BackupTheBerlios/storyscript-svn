#include "Character.hpp"
#include "DLLExport.hpp"

using namespace SS;




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Character::Character
NOTES: Adds all the predefined variables
*/
Character::Character()
{
}

Character::Character( const STRING& Name,
					  bool Static /*= false*/, bool Const /*= false*/ )
: Scope(Name, Static, Const)
{
}




//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Character::AcceptVisitor
// NOTES: ...
//
void Character::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitCharacter(this);
}
