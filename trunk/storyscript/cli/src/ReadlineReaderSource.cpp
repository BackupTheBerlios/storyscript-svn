#include "ReadlineReaderSource.hpp"

extern "C"{
#include <readline/history.h>
#include <readline/readline.h>
}


using namespace SS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
ReadlineReaderSource::ReadlineReaderSource()
{
	using_history();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Uses realine to get feed user input into the interpreter.
*/
STRING ReadlineReaderSource::GetNextLine()
{
	char* Buffer = 0;

	//Gotta love that python prompt.
	Buffer = readline("\n>>> ");
	
	if( Buffer == 0 ) return STRING();
	
	add_history( Buffer );
	
	STRING StringizedBuffer = NormalizeString( Buffer );
	
	//if( StringizedBuffer.size() != 0 ) add_history( Buffer );
	free( Buffer );
	
	StringizedBuffer += TXT("\n");
	return StringizedBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns a name, one that cannot be confused with a file.
*/
STRING ReadlineReaderSource::GetName() const
{
	return STRING( TXT("*stdin*") );
}

