/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: A reader source that reads from a simple preset string.  Very adaptable for various purposes.
*/

#include "ReaderSourceString.hpp"
#include "HelperFuncs.hpp"

using namespace SS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructors
*/
ReaderSourceString::ReaderSourceString()
{}

ReaderSourceString::ReaderSourceString( const String& InitString )
	: mString(InitString)
{
	mPos = mString.begin();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: To reinitialize or to initialize in the first place if ctor wasn't called.
*/
void ReaderSourceString::Reset( const String& InitString )
{
	mString = InitString;
	mPos = mString.begin();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Where the work is done.
*/
String ReaderSourceString::GetNextLine()
{
	String Buffer;
	
	while( mPos != mString.end() )
	{
		//handle retarted newline schemes
		
		//POSIX
		if( *mPos == '\n' )
		{
			Buffer += '\n';
			mPos++;
			
			//Windows
			if( *mPos == '\r' ){
				mPos++;
			}
				
			break;
		}
		//Apple
		else if( *mPos == '\r' )
		{
			Buffer += '\n';
			mPos++;
			
			break;
		}
		else Buffer += *mPos;
		
		mPos++;
	}
	
	return Buffer;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Throw out some BS name.  I'm purposely using characters in the name
 		that are illegal in pretty much any filesystem I know of.  That way
 		it isn't confused with a file.
*/
String ReaderSourceString::GetName() const
{
	return String( TXT("*external-string*") );
}





