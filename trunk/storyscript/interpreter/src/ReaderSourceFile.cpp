/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: A specialization of ReaderSource that reads from files.
*/

#include "ReaderSourceFile.hpp"
#include "ParserAnomaly.hpp"

using namespace SS;

//Thats 1K - 2K per file extra, which isn't too bad I guess.
const int BUFFER_SIZE = 1024;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: ctors
*/
ReaderSourceFile::ReaderSourceFile()
	: mpBuffer( new Char[BUFFER_SIZE] )
{
}

ReaderSourceFile::ReaderSourceFile( const String& FileName )
	: mpBuffer( new Char[BUFFER_SIZE] )
{
	Open( FileName );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: dtor
*/
ReaderSourceFile::~ReaderSourceFile()
{
	Close();
	delete [] mpBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Opens a file.  If a file is already open, it closes it.
*/
void ReaderSourceFile::Open( const String& FileName )
{
	if( IsOpen() ) Close();

	mFile.open( NarrowizeString( FileName ).c_str() /*, std::ios::binary*/ );

	if( !mFile )
	{
		String tmp = TXT("Cannot find file \'");
		tmp += FileName;
		tmp += TXT("\'. Check your spelling.");
		ThrowParserAnomaly( tmp, ANOMALY_BADFILE );
	}

	mFileName = FileName;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Closes the file.  Does nothing if there is no file open.
*/
void ReaderSourceFile::Close()
{
	if( mFile.is_open() )
	{
		mFile.close();
		mFileName.clear();		
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns true if the file is open.
*/
bool ReaderSourceFile::IsOpen()
{
	return mFile.is_open();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns the current file name.  Or an empty string if no file is open.
*/
String ReaderSourceFile::GetName() const
{
	return mFileName;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns the next line from the file including all
 		line-breaks/carriage-returns.  Returns a blank string if eof reached.
*/
String ReaderSourceFile::GetNextLine()
{
	if( mFile.eof() ) return String();
	mFile.getline( mpBuffer, BUFFER_SIZE );

	//I have to add the newline since getline won't store it.
	return String( mpBuffer ) + String(TXT("\n"));
}


