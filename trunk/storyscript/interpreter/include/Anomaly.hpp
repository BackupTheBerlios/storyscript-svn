/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Anomaly.hpp
	\brief Declarations for the Anomaly class.
*/


#if !defined(SS_Anomaly)  //To prevent multiple inclusions.
#define SS_Anomaly

#include "Unicode.hpp"
#include <exception>


namespace SS{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief An exception class.
	
	Anomaly is a basic exception class that is on the verge of deprecation,
	but is still used by in a few non interpreter related errors.
	
	You can pretty much ignore this, but pay attention to ParserAnomaly.
	
	\see ParserAnomaly
*/
class Anomaly : public std::exception
{
public:
	/// Default Constructor
	Anomaly();

	/// Copy Constructor
	Anomaly( const Anomaly& );

	/**
		Constructor	
		\param Desc A description of the error.
	*/
	Anomaly( const SS::STRING& Desc );

	/**
		Constructor	
		\param Desc A description of the error.
		\param Flags User define error code.
	*/
	Anomaly( const SS::STRING& Desc,
			 unsigned int Flags );

	/**
		Constructor	
		\param Desc A description of the error.
		\param Flags User define error code.
		\param FileName Name of file where the error occurred.
		\param Line Line number where the error occurred.
		\param FuncName Name of the function where the error occurred.
	*/
	Anomaly( const SS::STRING& Desc,
			 unsigned int Flags,
			 const SS::STRING& FileName,
			 unsigned int Line,
			 const SS::STRING& FuncName );

	///Destructor
	virtual ~Anomaly() throw();

	/// Returns the error description.  Derived from std::exception.
	virtual const char* what() const throw();
			 
	/// A description of the error.
	STRING ErrorDesc;
	
	/// The function in which the exception occurred.
	STRING SourceFunction;
	
	/// The source file in which the exception occurred.
	STRING SourceFile;
	
	/// The line in the the source file in which the exception occurred.
	unsigned int SourceLine;

	/// Meaning is dependent on the user.
	unsigned int ErrorCode;
};


/**
	\brief A shorthand macro for throwing an Anomaly.
	
	This is the preferred way the throw the Anomaly class, as it takes care of
	setting the FILE, LINE, and FUNC, for you.
	
	\param Desc A description of the error.
	\param Code The (user-defined) error code.
*/
#define ThrowAnomaly( Desc, Code ) throw SS::Anomaly( Desc, Code, SS_FILE, SS_LINE, SS_FUNC )


}//end namespace SS
#endif //end: #if !defined(SS_Anomaly)


