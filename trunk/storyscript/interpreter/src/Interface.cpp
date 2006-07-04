/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains the abstrct base class Inteface, and and declarations for
	the NullInterface class.
*/

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
Interface::Interface( Interpreter& I )
	: mI( I )
{
	mI.SetInterface( *this );
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
	return mI;
}

const Interpreter& Interface::GetInterpreter() const{
	return mI;
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

		if( FileName.length() != 0 ) mI.Open( FileName );
		else
		{
			ThrowParserAnomaly( TXT("No file specified.  Exiting."), ANOMALY_PANIC );
		}

		if( BlockName.length() != 0 ) mI.Parse( BlockName );
		else mI.Parse( mI.GetFirstBlock() );
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOBLOCKS )
		{
			if( mI.IsVerbose() ){
				LogMessage( TXT("\nNo blocks to execute.  Exiting...\n") );
			}
			return;
		}

		HandleParserAnomaly( E );
	}							
}

