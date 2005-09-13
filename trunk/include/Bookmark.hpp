
#if !defined(SS_Bookmark)
#define SS_Bookmark

#include "Types.hpp"
#include "ReaderSource.hpp"

namespace SS{

/*~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bookmark
NOTES: Stores the a specific location in a script file to allow the interpreter
to jump back and forth without breaking anything.
*/
struct SS_API Bookmark
{
	Bookmark();
	Bookmark( const SS::STRING& FileName, ReaderPos Position,
			  unsigned long Line, ScopePointer CurrentScope = ScopePointer(),
			  ScopePointer CurrentStaticScope = ScopePointer() );

	bool IsVoid() const;

	SS::STRING FileName;
	ReaderPos Position;
	unsigned long Line;
	ScopePointer CurrentScope;
	ScopePointer CurrentStaticScope;
};

}//namespace SS
#endif

