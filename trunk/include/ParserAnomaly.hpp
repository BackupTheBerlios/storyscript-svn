/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: 
"To err is human, to throw is devine"
Created: 25:8:2004   15:14
*/


#if !defined(SS_ParserAnomaly )
#define SS_ParserAnomaly

#include "DLLExport.hpp"
#include "Unicode.hpp"

namespace SS
{

//Add to this how you see fit
enum AnomalyCode{
	ANOMALY_NONE,  //No specific error
	ANOMALY_PANIC, //Panic!!!  Something is very wrong with the interpreter.
	ANOMALY_EOF,   //End of file
	ANOMALY_BADFILE, //Can't fine the file, or there is a problem opening
	ANOMALY_UNKNOWNWORD, //Unknown word, very unlikely to happen.
	ANOMALY_UNKNOWNTYPE, //Unknown type specifier.  A problem with the interpreter not the script.
	ANOMALY_NOOPENFILE, //No file is open.
	ANOMALY_BADPUNCTUATION, //Mismatching brackets or some other punctuation out of place.
	ANOMALY_BADGRAMMAR, //Some word is in the wrong spot.  Like "Luke character;" or something.
	ANOMALY_BADDECLARATION, //Malformed syntax while declaring an object.
	ANOMALY_UNKNOWNOP, //An undefined operator was used.
	ANOMALY_IDNOTFOUND, //Referenced an id that doesn't exist.
	ANOMALY_ALREADYEXISTS, //Tried to register an object with an name thats already in use
	ANOMALY_NOTASCOPE, //Tried to register a variable to a non-scope object
	ANOMALY_NOCONVERSION, //Tried to do an operation involving to variable that can't be converted to the same type
	ANOMALY_BADSTRINGOP, //Tried to use -, *, / or something with strings
	ANOMALY_NOINTERFACE, //The Interpreter has no interface set.
	ANOMALY_NOLISTELEMENT, //Tried to remove or access a list element that doesn't exist.
	ANOMALY_UNDEFINEDOP, //The operator isn't defined for that type.  Like tring to divide a list.
	ANOMALY_CONSTASSIGN, //Someone tried to assign to constant
	ANOMALY_LISTTOOBIG,
	ANOMALY_BADPRECISION, //Tried to set the precision of a variabl too high or too low.
	ANOMALY_NOBLOCKS //No blocks were found in the file.  The interpeter doesn't know what to do.

};


//This is the prefered method.  It reduces human error and is easier to read.
#define ThrowParserAnomaly( Desc, Code ) throw ParserAnomaly( Desc, Code, SS_FILE, SS_LINE, SS_FUNC )


//~~~~~~~STRUCTURE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ParserAnomaly
// NOTES: This is the error thrown by the parser when it runs into a problem
//		  with the S-Script file.  
//
class SS_API ParserAnomaly : public std::exception
{
public:
	ParserAnomaly();

	ParserAnomaly( const SS::STRING& Desc,
				   AnomalyCode Code,
				   const SS::STRING& SourceFile,
				   unsigned int SourceLine );

	ParserAnomaly( const SS::STRING& Desc,
				   AnomalyCode Code,
				   const SS::STRING& SourceFile,
				   unsigned int SourceLine,
				   const SS::STRING& FuncName );

	ParserAnomaly( const SS::STRING& Desc,
				   AnomalyCode Code,
				   const SS::STRING& SourceFile,
				   unsigned int SourceLine,
				   const SS::STRING& FuncName,
				   const SS::STRING& ScriptFile,
				   unsigned int ScriptLine );

	~ParserAnomaly() throw();


	//I'm leaving all this data out in the open.
	//Screw encapsulation, there really isn't any
	//point for something like this.

	SS::STRING ErrorDesc;
	AnomalyCode ErrorCode;

	SS::STRING SourceFile;
	SS::STRING SourceFunction;
	unsigned int SourceLine;

	SS::STRING ScriptFile;
	unsigned int ScriptLine;
};








} //namespace SS
#endif //#define SS_ParserAnomaly
