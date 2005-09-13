/*
	Anomaly.hpp
	----------------------
	Created: Mon, Dec 29 2003, 19:13
	Description: 
	
	TODO:
	
	Changelog:
	---------------------------------
	Thu, Jan 29 2004, 1:59
	--Updated for the new DCJ.
	---------------------------------

	
*/


#if !defined(SS_Anomaly)  //To prevent multiple inclusions.
#define SS_Anomaly

#include "Unicode.hpp"
#include <exception>


namespace SS{

//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Anomaly
// NOTES: Holds information about an Anomaly
//
class Anomaly : public std::exception
{
public:
	Anomaly();

	Anomaly( const Anomaly& );

	Anomaly( const SS::STRING& Desc );

	Anomaly( const SS::STRING& Desc,
			 unsigned int Flags );

	Anomaly( const SS::STRING& Desc,
			 unsigned int Flags,
			 const SS::STRING& FileName,
			 unsigned int Line,
			 const SS::STRING& FuncName );

	virtual ~Anomaly() throw();

	//From std::exception
	virtual const char* what() const throw();
			 

	STRING ErrorDesc;
	STRING SourceFile;
	STRING SourceFunction;
	unsigned int SourceLine;

	//This means different things depending on the context
	unsigned int ErrorCode;
};


//Use this!
#define ThrowAnomaly( Desc, Code ) throw SS::Anomaly( Desc, Code, SS_FILE, SS_LINE, SS_FUNC )


}//end namespace SS
#endif //end: #if !defined(SS_Anomaly)


