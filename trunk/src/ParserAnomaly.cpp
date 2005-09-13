/*
ParserAnomaly.cpp
----------------------

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


ParserAnomaly::ParserAnomaly( const STRING& Desc,
							  AnomalyCode Code,
							  const STRING& SourceFile,
							  unsigned int SourceLine )
: ErrorDesc( Desc ),
  ErrorCode( Code ),
  SourceFile( SourceFile ),
  SourceLine( SourceLine )
{}


ParserAnomaly::ParserAnomaly( const STRING& Desc,
							  AnomalyCode Code,
							  const STRING& SourceFile,
							  unsigned int SourceLine,
							  const STRING& FuncName )
: ErrorDesc( Desc ),
  ErrorCode( Code ),
  SourceFile( SourceFile ),
  SourceLine( SourceLine ),
  SourceFunction( FuncName )
{}


ParserAnomaly::ParserAnomaly( const STRING& Desc,
							  AnomalyCode Code,
							  const STRING& SourceFile,
							  unsigned int SourceLine,
							  const STRING& FuncName,
							  const STRING& ScriptFile,
							  unsigned int ScriptLine )
: ErrorDesc( Desc ),
  ErrorCode( Code ),
  SourceFile( SourceFile ),
  SourceLine( SourceLine ),
  SourceFunction( FuncName ),
  ScriptFile( ScriptFile ),
  ScriptLine(ScriptLine )
{}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ParserAnomaly::~ParserAnomaly
 NOTES: Destructor.  Doesn't really do anything.
*/
SS::ParserAnomaly::~ParserAnomaly() throw()
{
}
