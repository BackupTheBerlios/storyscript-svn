/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The SSI command line interface.
*/

#if !defined(SS_ConsoleReaderSource)
#define SS_ConsoleReaderSource

#include <ReaderSource.hpp>
#include "Console.hpp"

namespace SS{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Since the readline library is windows challenged, this uses just plain
		old fashionged console i/o.
*/
class ConsoleReaderSource : public ReaderSource
{
public:
	ConsoleReaderSource( Console* pCon );

	STRING GetName() const;

private:
	Console* mpCon;

	ConsoleReaderSource();
	STRING GetNextLine();
};

}



#endif

