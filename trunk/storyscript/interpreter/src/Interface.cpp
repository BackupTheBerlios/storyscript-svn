/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "Interface.hpp"
#include "ParserAnomaly.hpp"
#include "HelperFuncs.hpp"
#include "Interpreter.hpp"
#include "DLLExport.hpp"

using namespace SS;



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Interface::Interface( Interpreter& I )
	: mI( I )
{
	mI.SetInterface( *this );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Interface::~Interface()
{
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Interpreter& Interface::GetInterpreter(){
	return mI;
}

const Interpreter& Interface::GetInterpreter() const{
	return mI;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interface::StartConversation( const String& FileName, const String& BlockName )
{
	try{
		//CheckInterpreter();

		if( FileName.length() != 0 ) mI.OpenFile( FileName );
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
unsigned int NullInterface::PresentChoice( const std::vector<BlockPtr>& Choices )
{
		return 0;
}

void NullInterface::HandleParserAnomaly()
{
		exit(1);
}

void NullInterface::LogMessage( const SS::String&, bool )
{
}


void NullInterface::SayBlock( const BlockPtr )
{}

