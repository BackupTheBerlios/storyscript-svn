/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The SSI command line interface.
*/

#include "ConsoleInterface.hpp"
#include "Console.hpp"

#if !defined(_WIN32)
#include "ReadlineReaderSource.hpp"
#endif

#include "ConsoleReaderSource.hpp"

#include <Interpreter.hpp>
#include <Block.hpp>
#include <LanguageConstants.hpp>
#include <VersionInfo.hpp>

#include <DLLExport.hpp>

//#include <iostream>


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::ConsoleInterface
 NOTES: constructor
*/
ConsoleInterface::ConsoleInterface( Console& CON, SS::Interpreter& I, bool StartupBanner /*=true*/ )
	: Interface(I), CON(CON), mGCCStyleErrors(false)
{
	//Create backup reader source
	
	//if we are using curses we can't use the readline source.
	if( dynamic_cast<CursesConsole*>(&CON) )
	{
		mpLineReader = new SS::ConsoleReaderSource( &CON );
	}
	else
	{
#if defined(_WIN32)
		mpLineReader = new SS::ConsoleReaderSource( &CON );
#else
		mpLineReader = new SS::ReadlineReaderSource;
#endif
	}


	SetDefaultColor();
	
	if( StartupBanner )
	{
		SetDebugColor();
		
		CON << TXT("SS Console Interpreter.\nVersion: ") << SS::GetVersionString() << TXT(" \n");
		CON << TXT("This is Alpha Software!  Please report anything suspicious to: DanielCJones@gmail.com.\n");
		#if defined(_DEBUG)
		CON << TXT("DEBUG MODE ON, ");
		#else
		CON << TXT("DEBUG MODE OFF, ");
		#endif
		#if defined(USING_UNICODE)
		CON << TXT("UNICODE MODE ON, ");
		#else
		CON << TXT("UNICODE MODE OFF, ");
		#endif
		//CON << TXT("Last compile: ") << SS::COMPILEDATA << TXT(" \n");
		CON << TXT("-------------------------------------------------\n\n");

		SetDefaultColor();
	}

	mI.SetVerbose(false);	
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::~ConsoleInterface
 NOTES: destructor
*/
ConsoleInterface::~ConsoleInterface()
{
	delete mpLineReader;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: StartConversation overloads to support interactive interpreter use.
*/
void ConsoleInterface::StartConversation()
{
	SetDebugColor();
	try{
	mI.SetSource( *mpLineReader );
	}
	catch( SS::ParserAnomaly E )
	{
		HandleParserAnomaly( E );
	}
	SetDefaultColor();
}

void ConsoleInterface::StartConversation(  const SS::String& FileName, const SS::String& BlockName )
{
	if( FileName.empty() ) StartConversation();
	else Interface::StartConversation( FileName, BlockName );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// ConsoleInterface::SetGCCStyleErrors
// NOTES: Toggles the use os GCC style error output.
//
void ConsoleInterface::SetGCCStyleErrors( bool Flag /*=true*/ ){
	mGCCStyleErrors = Flag;
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::PresentChoice
 NOTES: The should only be called by the Interpreter.
		It presents the player with a choice in dialogue.  When the player picks,
		the functions returns the index for that choice.
*/
unsigned int ConsoleInterface::PresentChoice( const std::vector<SS::BlockPtr>& Choices )
{
	//Print all the choices
	SetChooseColor();
	unsigned int i;
	for( i = 0; i < Choices.size(); i++ ){
		CON << TXT("(");
		CON << i;
		CON << TXT(")  ");

		SS::String tmp;
		if( (tmp = Choices[i]->GetDocString()).empty() ){
			CON << Choices[i]->GetFullName();
		}
		else CON << tmp;
		
		CON << TXT("\n");
	}

	unsigned int PlayersChoice = 0;

	//TODO: (BUG) If you type bad input, this thing may loop forever!
	do{
		CON << TXT("\n>");
		CON >> PlayersChoice;
	}while( PlayersChoice >= Choices.size() );

	SetDefaultColor();


	return PlayersChoice;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::HandleParserAnomaly
 NOTES: Prints the error message to the console and then exits.
*/
void ConsoleInterface::HandleParserAnomaly( SS::ParserAnomaly E )
{
	if( mGCCStyleErrors )
	{
		SetErrorColor();
		CON << E.ScriptFile << TXT(":") << E.ScriptLine << TXT(": ");
		CON << TXT("Error ") << E.ErrorCode << TXT(": ") << E.ErrorDesc << TXT("\n");
		SetDefaultColor();
	}
	else
	{	
		SetErrorColor();
		CON << TXT("\nCaught a ParserAnomaly:\n");
		CON << TXT("\"") << E.ErrorDesc << TXT("\"");
		CON << TXT("\nErrorCode: ") << E.ErrorCode;
		CON << TXT("\nSourceFile: ") << E.SourceFile;
		CON << TXT("\nSourceLine: ") << E.SourceLine;
		CON << TXT("\nSourceFunc: ") << E.SourceFunction;
		CON << TXT("\nScriptFile: ") << E.ScriptFile;
		CON << TXT("\nScriptLine: ") << E.ScriptLine;
		CON << TXT("\n\n");
		SetDefaultColor();
	}
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::LogMessage
 NOTES: Prints a message to the screen.
*/
void ConsoleInterface::LogMessage( const SS::String& Msg, bool UserOutput /*=false*/ )
{
	static bool LastWasUserOutput = false;

	SetDebugColor();

	if( UserOutput )
	{
		CON << Msg;
	}	
	else
	{
		if( LastWasUserOutput ) CON << TXT("\n");

		if( Msg[0] == '\n' )
		{
			CON << TXT("\n");
			PrintPosition();
			CON << Msg.substr( 1 );
		}
		else
		{
			PrintPosition();
			CON << Msg;
		}
	}

	LastWasUserOutput = UserOutput;
	
	SetDefaultColor();
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::SayBlock
 NOTES: Prints the line to the screen.
*/
void ConsoleInterface::SayBlock( const SS::BlockPtr pBlock )
{
	const SS::String& Line = pBlock->GetScopeObjectLocal( SS::LC_Output )->CastToVariable()->GetStringData();
	const SS::String& ParentName = pBlock->GetParent()->GetName();
	
	//Get and print name.
	if( Line.empty() )  return;

	SetCharacterColor();
	CON << ParentName << TXT("\n");
	SetDefaultColor();
	SetLineColor();
	CON << Line << TXT("\n\n");
	SetDefaultColor();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ConsoleInterface::PrintPosition
 NOTES: Prints the positions. Like: 12(134) or something.
*/
void ConsoleInterface::PrintPosition()
{
	SS::Bookmark Mark = mI.GetCurrentPos();

	CON << Mark.FileName;
	CON << TXT("; ");
	CON << Mark.Line << TXT("(");

	//This is so it will output (end) instead of (4294967295)
	if( Mark.Position == ~0U )	{
		CON << TXT("end");
	}
	else{
		CON << Mark.Position;
	}

	CON << TXT("):  ");
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ConsoleInterface::SetDefaultColor
 NOTES: Returns the color to the default
*/
void ConsoleInterface::SetDefaultColor()
{
	CON.SetDefault();
	//CON.SetUnderline( false );
	//CON.SetBold( false );
	//CON.SetTextFGColor( ColorWhite );
//	CON.SetBackgroundFull( ColorPair( ColorBlack, ColorWhite ) );
//	CON.SetTextColor( ColorPair( ColorBlack, ColorWhite ) );
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::SetErrorColor
 NOTES: 
*/
void ConsoleInterface::SetErrorColor()
{
	CON.SetTextColor( ColorPair(ColorBlack, ColorRed) );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::SetDebugColor
 NOTES: 
*/
void ConsoleInterface::SetDebugColor()
{
	//CON.SetTextColor( ColorPair(ColorCyan, ColorWhite) );
	CON.SetTextFGColor( ColorWhite );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::SetLineColor
 NOTES: 
*/
void ConsoleInterface::SetLineColor()
{
	//CON.SetTextColor( ColorPair(ColorRed, ColorWhite) );
	CON.SetTextFGColor( ColorRed );
	CON.SetBold( true );
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::SetCharacterColor
 NOTES: 
*/
void ConsoleInterface::SetCharacterColor()
{
	//CON.SetTextColor( ColorPair(ColorBlack, ColorWhite) );
	CON.SetTextFGColor( ColorBlue );
	CON.SetBold( true );
	CON.SetUnderline( true );	
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ConsoleInterface::SetChooseColor
 NOTES: 
*/
void ConsoleInterface::SetChooseColor()
{
	//CON.SetBackground( ColorPair( ColorWhite, ColorBlue ) );
	//CON.SetTextColor( ColorPair(ColorBlack, ColorRed) );
	CON.SetTextFGColor( ColorGreen );
}



