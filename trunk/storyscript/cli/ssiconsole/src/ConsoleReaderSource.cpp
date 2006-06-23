/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The SSI command line interface.
*/

#include "ConsoleReaderSource.hpp"

using namespace SS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NOTES: Constructor
	   */
ConsoleReaderSource::ConsoleReaderSource( Console* pCon )
: mpCon(pCon)
{
}

ConsoleReaderSource::ConsoleReaderSource()
{
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NOTES: Uses realine to get feed user input into the interpreter.
*/
STRING ConsoleReaderSource::GetNextLine()
{
	*mpCon << TXT("\n>>> ");
	STRING Buf;
	*mpCon >> Buf;
	Buf += TXT("\n");

	return Buf;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NOTES: Returns a name, one that cannot be confused with a file.
*/
STRING ConsoleReaderSource::GetName() const
{
	return STRING( TXT("*stdin*") );
}

