/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: 
"To err is human, to throw is devine"
Created: 25:8:2004   15:14
*/

#include "ParserAnomaly.hpp"

using namespace SS;



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ParserAnomaly::ParserAnomaly
 NOTES: Constructor variants.
*/
ParserAnomaly::ParserAnomaly()
: ErrorCode( ANOMALY_NONE ),
  SourceLine(0),
  ScriptLine(0)
{}


ParserAnomaly::ParserAnomaly( const String& Desc,
							  AnomalyCode Code,
							  const String& SourceFile,
							  unsigned int SourceLine )
: ErrorDesc( Desc ),
  ErrorCode( Code ),
  SourceFile( SourceFile ),
  SourceLine( SourceLine )
{}


ParserAnomaly::ParserAnomaly( const String& Desc,
							  AnomalyCode Code,
							  const String& SourceFile,
							  unsigned int SourceLine,
							  const String& FuncName )
: ErrorDesc( Desc ),
  ErrorCode( Code ),
  SourceFile( SourceFile ),
  SourceFunction( FuncName ),
  SourceLine( SourceLine )
{}


ParserAnomaly::ParserAnomaly( const String& Desc,
							  AnomalyCode Code,
							  const String& SourceFile,
							  unsigned int SourceLine,
							  const String& FuncName,
							  const String& ScriptFile,
							  unsigned int ScriptLine )
: ErrorDesc( Desc ),
  ErrorCode( Code ),
  SourceFile( SourceFile ),
  SourceFunction( FuncName ),
  SourceLine( SourceLine ),
  ScriptFile( ScriptFile ),
  ScriptLine(ScriptLine )
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
const char* ParserAnomaly::what() const throw()
{
	static std::string LowerWhat;
    LowerWhat = NarrowizeString( ErrorDesc );
	
	return LowerWhat.c_str();
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ParserAnomaly::~ParserAnomaly
 NOTES: Destructor.  Doesn't really do anything.
*/
SS::ParserAnomaly::~ParserAnomaly() throw()
{
}
