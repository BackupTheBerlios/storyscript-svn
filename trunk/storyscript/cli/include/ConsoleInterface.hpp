/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The SSI command line interface.
*/

#include "Interface.hpp"
#include "Interpreter.hpp"
#include "ParserAnomaly.hpp"
#include "Console.hpp"




class ConsoleInterface : public SS::Interface
{
public:
	ConsoleInterface( Console& CON, SS::Interpreter& I, bool StartupBanner = true );
	~ConsoleInterface();
	
	void StartConversation();
	void StartConversation(  const SS::String& FileName, const SS::String& BlockName = SS::String() );
	
	void SetGCCStyleErrors( bool Flag = true );
	
	//All inherited
	unsigned int PresentChoice( const std::vector<SS::BlockPtr>& Choices );

	void HandleParserAnomaly( SS::ParserAnomaly );
	void LogMessage( const SS::String&, bool UserOutput = false );

private:
	Console& CON;

	SS::ReaderSource* mpLineReader;

	void SayBlock( const SS::BlockPtr );

	void PrintPosition();

	void SetDefaultColor();
	void SetErrorColor();
	void SetDebugColor();
	void SetLineColor();
	void SetCharacterColor();
	void SetChooseColor();
	
	bool mGCCStyleErrors;
};
