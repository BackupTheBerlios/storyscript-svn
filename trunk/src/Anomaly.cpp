/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The basic anomaly (exception) class.
*/

#include "Anomaly.hpp"


using namespace SS;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Anomaly::Anomaly
 NOTES: All the constructors
*/
Anomaly::Anomaly()
: SourceLine(0),
  ErrorCode(0)
{}


Anomaly::Anomaly( const Anomaly& A )
: ErrorDesc     ( A.ErrorDesc ),
  SourceFunction( A.SourceFunction ),
  SourceFile    ( A.SourceFile ),
  SourceLine    ( A.SourceLine ),
  ErrorCode     ( A.ErrorCode )
{}

Anomaly::Anomaly( const SS::STRING& Desc, unsigned int Flags = 0 )
: ErrorDesc( Desc ),
  ErrorCode( Flags )
{}

Anomaly::Anomaly( const SS::STRING& Desc, unsigned int Flags, 
				  const SS::STRING& FileName, unsigned int Line,
				  const SS::STRING& FuncName )
: ErrorDesc     ( Desc ),
  SourceFunction( FuncName ),
  SourceFile    ( FileName ),
  SourceLine    ( Line ),
  ErrorCode     ( Flags )
{}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Anomaly::~Anomaly
 NOTES: Destructor
*/
Anomaly::~Anomaly() throw()
{}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Anomaly::what
 NOTES: Returns a (non-wide :( ) string giving a description of the error.
*/
const char* Anomaly::what() const throw()
{
	static std::string LowerWhat;
    LowerWhat = NarrowizeString( ErrorDesc );
	
	return LowerWhat.c_str();
}

