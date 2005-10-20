/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: This little program is to provide a console based interface for the S^2 interpreter.  It is very useful for debugging, but not much else.
*/


#include <iostream>
#include <ctime>
#include <vector>

#include "Console.hpp"
#include "ConsoleInterface.hpp"




int main( int argc, char* argv[] )
{
	//I sometimes use this to give me an opportunity to attach a debugger.
	//__asm int 3;


	std::vector<SS::STRING> Argument;

	int i;

	//Read it all into strings for easy handling.
	for( i = 0; i < argc; i++ )
	{
		if( i == 0 ) continue;
		Argument.push_back( SS::NormalizeString( argv[i] ) );
	}

	SS::STRING BlockName;
	SS::STRING FileName;
	bool Verbose = false;

/*
#if defined(_DEBUG)
	Verbose = true;
#endif
*/
	
	bool UseFancyOutput = true;
	bool GCCErrors = false;
	bool Quiet = false;
	
	//Figure out what to do with it now.
	//bool ExpectFileName  = false;
	bool ExpectBlockName = false;

	for( i = 0; i < (int)Argument.size(); i++ )
	{
		if( ExpectBlockName ){
			BlockName = Argument[i];
			ExpectBlockName = false;
		}
		else if( Argument[i].length() > 1 &&
				(Argument[i][0] == '/'  || Argument[i][0] == '-') )
		{
			//Verbose Flag
			if( Argument[i][1] == 'v' ||
				Argument[i] == TXT("-verbose") )
			{
				Verbose = true;
			}
			//Block-name Flag
			else
			if( Argument[i][1] == 'b' ||
				Argument[i] == TXT("-block") )
			{
				ExpectBlockName = true;
			}
			else
			if( Argument[i][1] == 'g' ||
				Argument[i] == TXT("-gcc-style") )
			{
				GCCErrors = true;
			}
			//Help Flag
			else
			if( Argument[i][1] == 'h' ||
				Argument[i] == TXT("--help") )
			{
				StdConsole CON;
				
				CON << TXT("SS Console Interpreter\n");
				CON << TXT("Usage: ssi [OPTIONS] FILE\n");
				CON << TXT("Options are:\n");
				CON << TXT("(Note that \'-\' is interchangeable with \'/\'.)\n");
				CON << TXT(" -v, -verbose            Verbose output.  Useful for debugging.\n");
				CON << TXT(" -b, -block BLOCKNAME    Specifies the block in the file that you want to start on.\n");
				CON << TXT(" -h, --help              Prints the message you are looking at right now.\n");
				CON << TXT(" -p, -plain              Uses standard console I/O rather than the default curses library.\n");
				CON << TXT(" -g, -gcc-style          Print any errors in the same format that GCC uses.\n");
				CON << TXT(" -q, -quiet              Doesn't print unnecessary stuff.");

				
				return 0;
			}			
			//Use Plain Console Flag
			else
			if( Argument[i][1] == 'p' ||
				Argument[i] == TXT("-plain") )
			{
				UseFancyOutput = false;
			}
			else
			if( Argument[i][1] == 'q' ||
				Argument[i] == TXT("-quiet") )
			{
				Quiet = true;
			}
		}
		//File Name
		else if( i == (int)Argument.size() - 1 )
		{
			FileName = Argument[i];
		}

	}
	
	Console* pCON = UseFancyOutput ? (Console*)new CursesConsole : (Console*)new StdConsole;
	Console& CON = *pCON;
	
	
//
//	if( FileName.empty()  )
//	{
//		CON << TXT("File Name?\n>");
//		CON >> FileName;
//	}

	/*
	if( BlockName.empty() )
	{
		CON << TXT("Block Name?\n>");
		CON >> BlockName;
	}
	*/
	

	
	ConsoleInterface Test( CON, !Quiet );
	
	if( GCCErrors ) Test.SetGCCStyleErrors();
	

	if( Verbose ) Test.GetInterpreter().SetVerbose( true );
		
	Test.StartConversation( FileName, BlockName );

	CON.SetTextFGColor( ColorCyan );
	
	if( !Quiet )
	{
		CON << TXT("\n\nTotal run time: ") << (float)(((double)clock()) / ((double)CLOCKS_PER_SEC)) << TXT(" seconds.\n\n");


		//This is ridiculous that I need this much code for something so trivial.
		CON << TXT("Press [Enter] to continue...");
		if( UseFancyOutput ){
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
	
	return 0;
}
