/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/


/**
	\file ParserAnomaly.hpp
	\brief Declarations for ParserAnomaly.  
	
	"To err is human, to throw is divine."
*/


#if !defined(SS_ParserAnomaly )
#define SS_ParserAnomaly

#include "DLLExport.hpp"
#include "Unicode.hpp"

namespace SS
{

/**
	\brief Error codes.
	
	An enumeration of various categories of error generated by the interpreter.
*/
enum AnomalyCode{
	ANOMALY_NONE,  ///< No specific error
	ANOMALY_PANIC, ///< Panic!!!  Something is very wrong with the interpreter.
	ANOMALY_EOF,   ///< End of file
	ANOMALY_BADFILE, ///< Can't fine the file, or there is a problem opening
	ANOMALY_UNKNOWNWORD, ///< Unknown word, very unlikely to happen.
	ANOMALY_UNKNOWNTYPE, ///< Unknown type specifier.  A problem with the interpreter not the script.
	ANOMALY_NOOPENFILE, ///< No file is open.
	ANOMALY_BADPUNCTUATION, ///< Mismatching brackets or some other punctuation out of place.
	ANOMALY_BADGRAMMAR, ///< Some word is in the wrong spot.  Like "Luke character;" or something.
	ANOMALY_BADDECLARATION, ///< Malformed syntax while declaring an object.
	ANOMALY_UNKNOWNOP, ///< An undefined operator was used.
	ANOMALY_IDNOTFOUND, ///< Referenced an id that doesn't exist.
	ANOMALY_ALREADYEXISTS, ///< Tried to register an object with an name that's already in use
	ANOMALY_NOTASCOPE, ///< Tried to register a variable to a non-scope object
	ANOMALY_NOCONVERSION, ///< Tried to do an operation involving to variable that can't be converted to the same type
	ANOMALY_BADSTRINGOP, ///< Tried to use -, *, / or something with strings
	ANOMALY_NOINTERFACE, ///< The Interpreter has no interface set.
	ANOMALY_NOLISTELEMENT, ///< Tried to remove or access a list element that doesn't exist.
	ANOMALY_UNDEFINEDOP, ///< The operator isn't defined for that type.  Like trying to divide a list.
	ANOMALY_CONSTASSIGN, ///< Someone tried to assign to constant
	ANOMALY_LISTTOOBIG,
	ANOMALY_BADPRECISION, //< Tried to set the precision of a variable too high or too low.
	ANOMALY_NOBLOCKS, //< No blocks were found in the file.  The interpreter doesn't know what to do.
	ANOMALY_NOOPERATOR //< Cannot find any operator in the expression.

};


/**
	\brief A shorthand macro for throwing ParserAnomalys.
	
	This is the preferred way the throw the ParserAnomaly class, as it takes care of
	setting the FILE, LINE, and FUNC, for you, as well as being easier to read.
	
	\param Desc A description of the error.
	\param Code The error code (AnomalyCode).
*/
#define ThrowParserAnomaly( Desc, Code ) throw ParserAnomaly( Desc, Code, SS_FILE, SS_LINE, SS_FUNC )


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Standard StoryScript exception.
	
	This is the exception class generated whenever for all language-related
	problems.  (ie. Problems created by the user's bad syntax.)	
	
	The user should throw this with ThrowParserAnomaly.
	
	\see ThrowParserAnomaly
*/
class SS_API ParserAnomaly : public std::exception
{
public:
	
	/// Default Constructor
	ParserAnomaly();

	
	/**
		Constructor	
		\param Desc A description of the error.
		\param Code The error code.
		\param SourceFile Name of file where the error occoured.
		\param SourceLine Line number where the error occurred.
	*/
	ParserAnomaly( const SS::STRING& Desc,
				   AnomalyCode Code,
				   const SS::STRING& SourceFile,
				   unsigned int SourceLine );

	/**
		Constructor	
		\param Desc A description of the error.
		\param Code The error code.
		\param SourceFile Name of file where the error occurred.
		\param SourceLine Line number where the error occurred.
		\param FuncName Name of the function where the error occurred.
	*/
	ParserAnomaly( const SS::STRING& Desc,
				   AnomalyCode Code,
				   const SS::STRING& SourceFile,
				   unsigned int SourceLine,
				   const SS::STRING& FuncName );

	/**
		Constructor	
		\param Desc A description of the error.
		\param Code The error code.
		\param SourceFile Name of file where the error occurred.
		\param SourceLine Line number where the error occurred.
		\param FuncName Name of the function where the error occurred.
		\param ScriptFile Name of the storyscript file where the error occurred.
		\param ScriptLine Line in the storyscript file where the error occurred.
	*/
	ParserAnomaly( const SS::STRING& Desc,
				   AnomalyCode Code,
				   const SS::STRING& SourceFile,
				   unsigned int SourceLine,
				   const SS::STRING& FuncName,
				   const SS::STRING& ScriptFile,
				   unsigned int ScriptLine );

	/// Destructor
	~ParserAnomaly() throw();
	
	/// Returns the error description.  Derived from std::exception.
	virtual const char* what() const throw();

	/// A description of the error.
	SS::STRING ErrorDesc;
	
	/**
		The error code.
		
		\see AnomalyCode
	*/
	AnomalyCode ErrorCode;

	/// The source file in which the exception occoured.
	SS::STRING SourceFile;
	
	/// The function in which the exception occurred.
	SS::STRING SourceFunction;
	
	/// The line in the the source file in which the exception occurred.
	unsigned int SourceLine;
	
	/// The story script file in which the error occurred.
	SS::STRING ScriptFile;
	
	/// The line in the storyscript file in which the error occurred.
	unsigned int ScriptLine;
};



} //namespace SS
#endif //#define SS_ParserAnomaly
