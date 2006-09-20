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
String ReadlineReaderSource::GetNextLine()
{
	char* Buffer = 0;

	//Gotta love that python prompt.
	Buffer = readline("\n>>> ");
	
	if( Buffer == 0 ) return String();
	
	add_history( Buffer );
	
	String StringizedBuffer = NormalizeString( Buffer );
	
	//if( StringizedBuffer.size() != 0 ) add_history( Buffer );
	free( Buffer );
	
	StringizedBuffer += TXT("\n");
	return StringizedBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns a name, one that cannot be confused with a file.
*/
String ReadlineReaderSource::GetName() const
{
	return String( TXT("*stdin*") );
}

