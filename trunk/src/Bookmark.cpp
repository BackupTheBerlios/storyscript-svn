

#include "Bookmark.hpp"
using namespace SS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: ctor
*/
Bookmark::Bookmark()
	: Position(0), Line(0)
{
}

Bookmark::Bookmark( const SS::STRING& FileName,
					ReaderPos Position,
				    unsigned long Line,
					ScopePointer CurrentScope /*= ScopePointer()*/,
					ScopePointer CurrentStaticScope /*= ScopePointer()*/ )
	: FileName(FileName),
      Position(Position),
	  Line(Line),
	  CurrentScope(CurrentScope)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns true if the bookmark is uninitialized.
*/
bool Bookmark::IsVoid() const
{
	if( FileName.empty() && Position == 0 && Line == 0 ) return true;
	else return false;	
}

