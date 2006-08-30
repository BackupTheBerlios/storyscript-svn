/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: This little program is to provide a console based interface for the S^2 interpreter.  It is very useful for debugging, but not much else.
*/


#include <iostream>
#include <ctime>
#include <vector>

#include "GetPot.hpp"

#include "Console.hpp"
#include "ConsoleInterface.hpp"




int main( int argc, char* argv[] )
{
	//I sometimes use this to give me an opportunity to attach a debugger.
	//__asm int 3;
	
	GetPot cl( argc, argv );

	
	
	
	//Test if we are using curses output
	bool UseCurses = false;
	if( cl.search( 2, "--curses", "-c" ) ) UseCurses = 1;
	
	Console* pCON = UseCurses ?
					(Console*)new CursesConsole : (Console*)new StdConsole;
	Console& CON = *pCON;

	//Win32 doesn't really support this right now.
#if defined(PLAT_WIN32)
	if( !UseCurses ) CON.UseColor( false );
#endif
	
	//Test if we should print help message and quit
	if( cl.search( 2, "--help", "-h" ) )
	{
		CON << TXT("SS Console Interpreter, \n");
		CON << TXT("Usage: storyscript [OPTIONS] FILE\n");
		CON << TXT("Options are:\n");
		CON << TXT(" -u, -ungodly-verbose    Rediculously verbose output.\n");
		CON << TXT(" -b, -block BLOCKNAME    Specifies the starting block.\n");
		CON << TXT(" -h, --help              Prints the message you are looking at right now.\n");
		CON << TXT(" -c, --curses            Use curses for output (ie. Fancy, colorful text).\n");
		CON << TXT("                         Currently a little buggy, but works well for files.\n");
		CON << TXT(" -n, --no-color          Don't print any color at all.\n");
		CON << TXT(" -v, --verbose       	 Adds some extra info, mainly with error output.\n");
		CON << TXT(" --version               Prints the version number, contact info, etc.\n");
		
		delete pCON;
		exit(0);	
	}
	
	//Test if we should print the version and exit
	if( cl.search( "--version" ) )
	{
		CON << TXT("SS Console Interpreter\n");
		CON << TXT("Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)\n");
		CON << SS::GetVersionString();
		CON << TXT("\n");
		//CON << TXT("\nPlease send comments and report bugs to: DanielCJones@gmail.com.\n");
		
		delete pCON;
		exit(0);
	}
	
	//Test for gcc-styles errors or not (I'm defaulting to yes, just because they look so much better).
	bool GCCErrors = true;
	bool Quiet = true;
	if( cl.search(2, "--verbose", "-v") )
	{
		Quiet = false;
		GCCErrors = false;
	}
	
	//Test for ungoldly verbosity
	bool Verbose = false;
	if( cl.search( 2, "--ungodly-verbose", "-u" ) )
	{
		Verbose = true;
		Quiet = false;
		GCCErrors = false;
	}
	
	//Shall we use color?
	//bool NoColor = false;
	if( cl.search( 2, "--no-color", "-n" ) ){
		CON.UseColor( false );		
	}
	
	//Test for block name
	SS::STRING BlockName;
	if( cl.search( 2, "--block", "-b" ) )
	{
		BlockName = SS::NormalizeString( cl.next("") );
	}
	
	//Try to get a file name
	SS::STRING FileName;
	if( cl.size() > 1 ){
		const char* Temp = cl[ cl.size() - 1];
		if( Temp[0] != '-' ) FileName = SS::NormalizeString( Temp );
	}
	
	
	

	CON.SetBackgroundFull( ColorPair( ColorCyan, ColorBlack ), ' ' );
	
	SS::Interpreter* pI = new SS::Interpreter;
	ConsoleInterface Test( CON, *pI, !Quiet );
	
	if( GCCErrors ) Test.SetGCCStyleErrors();
	

	if( Verbose ) Test.GetInterpreter().SetVerbose( true );
		
	Test.StartConversation( FileName, BlockName );

	
	CON.SetTextFGColor( ColorCyan );
	
	if( !Quiet || UseCurses )
	{
		CON << TXT("\nTotal run time: ") << (float)(((double)clock()) / ((double)CLOCKS_PER_SEC)) << TXT(" seconds.\n\n");


		//This is ridiculous that I need this much code for something so trivial.
		CON << TXT("Press [Enter] to continue...");
		if( UseCurses ){
			SS::STRING Dummy;
			CON >> Dummy;
		}
		else{

	#if defined(PLAT_WIN32)
			system("PAUSE");
	#elif defined(PLAT_LINUX)
			system("read dummy");
	#endif
		}
	}

		
	delete pCON;
	delete pI;
	
	return 0;
}
