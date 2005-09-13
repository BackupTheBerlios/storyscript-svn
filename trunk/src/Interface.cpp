
#include "Interface.hpp"
#include "ParserAnomaly.hpp"
#include "HelperFuncs.hpp"
#include "Interpreter.hpp"
#include "DLLExport.hpp"

using namespace SS;



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interface::Interface
 NOTES: Constructors
*/
Interface::Interface()
	: mInterpreter( Interpreter::Instance() )
{
	mInterpreter.SetInterface( *this );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interface::~Interface
 NOTES: Destructor
*/
Interface::~Interface()
{
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interface::GetInterpreter
 NOTES: Returns a reference to the interpreter.  For direct access.
*/
Interpreter& Interface::GetInterpreter(){
	return mInterpreter;
}

const Interpreter& Interface::GetInterpreter() const{
	return mInterpreter;
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interface::CheckInterpreter
 NOTES: Makes sure that an interpreter is set.  Throws if not.
*/
/*
void Interface::CheckInterpreter()
{
	if( !mpInterpreter )
	{
		throw ParserAnomaly( TXT("Interface has no Interpreter."),
			ANOMALY_NOINTERFACE, FILENAME, LINE, FUNC );
	}
}
*/


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interface::StartConversation
 NOTES: Call this with the full name of a block and it will
		begin the conversation.
*/
void Interface::StartConversation( const STRING& FileName, const STRING& BlockName )
{
	try{
		//CheckInterpreter();

		if( FileName.length() != 0 ) mInterpreter.Open( FileName );
		else
		{
			ThrowParserAnomaly( TXT("No file specified.  Exiting."), ANOMALY_PANIC );
		}

		if( BlockName.length() != 0 ) mInterpreter.Parse( BlockName );
		else mInterpreter.Parse( mInterpreter.GetFirstBlock() );
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOBLOCKS )
		{
			if( mInterpreter.Instance().IsVerbose() ){
				LogMessage( TXT("\nNo blocks to execute.  Exiting...\n") );
			}
			return;
		}

		HandleParserAnomaly( E );
	}							
}

