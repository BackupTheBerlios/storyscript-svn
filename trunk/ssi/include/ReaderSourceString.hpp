/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: A reader source that reads from a simple preset string.  Very adaptable for various purposes.
*/

#if !defined(SS_ReaderSourceString)
#define SS_ReaderSourceString

#include "ReaderSource.hpp"

namespace SS{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Reader specialization that reads from a set string.
*/
class ReaderSourceString
{
public:
	ReaderSourceString();
	ReaderSourceString( const STRING& );
	
	void Reset( const STRING& );
	
	
private:
	STRING GetNextLine();
	STRING::iterator mPos;
	
	STRING mString;	
};

}
#endif


