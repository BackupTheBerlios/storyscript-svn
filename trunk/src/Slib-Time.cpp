/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are used to maniuplate time.
*/

#include "Slib-Time.hpp"
#include "LanguageConstants.hpp"

//Used by TimeVar, ElapsedSeconds
#include <ctime>

//Used by ElapsedSeconds
#include <boost/timer.hpp>

//Used by ElapsedSeconds
#include <boost/lexical_cast.hpp>

using namespace SS;
using namespace SS::SLib;



//This is used only by ElapsedSeconds.  This should get initialized when the program starts.
boost::timer gProgramTimer;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Time
 NOTES: A special scope that holds all the Time library vars/funcs.
*/
Time::Time()
 : Scope( LC_SL_Time, true )
{
	RegisterPredefined();
	SetConst();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Time::RegisterPredefined
 NOTES: Registers any special stuff that need registering.
*/
void Time::RegisterPredefined()
{
	Register( ScopeObjectPointer( new TimeVar       ( TXT("time"), true, true ) ) );
	Register( ScopeObjectPointer( new ElapsedSeconds( TXT("elapsed_seconds"), true, true ) ) );
	Register( ScopeObjectPointer( new DateVar       ( TXT("date"), true, true ) ) );
	Register( ScopeObjectPointer( new TimeZoneVar   ( TXT("timezone"), true, true ) ) );
}




//--------TimeVar::mBufferSize---------
const int TimeVar::mBufferSize = 64;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeVar::TimeVar/~TimeVar
NOTES: Do I really have to write some stupid "Ctor & Dtor" shit for every one
of these.
*/
TimeVar::TimeVar()
{
	mpBuffer = new char[mBufferSize];
}


TimeVar::TimeVar( const STRING& Name,
				 bool Static /*= false*/, bool Const /*= false*/ )
				 : SpecialVarBase( Name, Static, Const )
{
	mpBuffer = new char[mBufferSize];
}

TimeVar::~TimeVar()
{
	delete[] mpBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeVar::GetVariableType
NOTES: The timer prefers returning itself as a string.  But if that not the,
case its ok.
*/
VarType TimeVar::GetVariableType() const{
	return VARTYPE_STRING;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeVar::Get____Data
NOTES: Where the real work is done.
*/
StringType TimeVar::GetStringData() const{
	//This is the prefered way for TimeVar to manifest itself

	time_t TmpTime = time(NULL);

	//This is for vulcan time (ie. 23:32:15)
	//strftime( mpBuffer, mBufferSize, "%X",localtime( &TmpTime ) );

	//And this is for human time (ie. 9:32:15 PM )
	strftime( mpBuffer, mBufferSize, "%I:%M:%S %p", localtime( &TmpTime ) );

	return NormalizeString( mpBuffer );
}

NumType TimeVar::GetNumData() const
{
	//Well, I guess I might as well take advantage of all my fancy conversion stuff
	time_t TmpTime = time(NULL);
	strftime( mpBuffer, mBufferSize, "%H%M%S", localtime( &TmpTime ) );

	Variable IntTime( SS_BASE_ARGS_DEFAULTS, NormalizeString( mpBuffer ) );
	IntTime.ForceConversion( VARTYPE_NUM );

	return IntTime.GetNumData();
}

BoolType TimeVar::GetBoolData() const{
	return true;
}


//------------DateVar::mBufferSzie--------------
const int DateVar::mBufferSize = 64;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
DateVar::DateVar/~DateVar
NOTES: 
*/
DateVar::DateVar()
{
	mpBuffer = new char[mBufferSize];
}

DateVar::DateVar( const STRING& Name,
				 bool Static /*= false*/, bool Const /*= false*/)
				 : SpecialVarBase(Name, Static, Const)
{
	mpBuffer = new char[mBufferSize];
}

DateVar::~DateVar()
{
	delete[] mpBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
DateVar::GetVariableType
NOTES: Returns the prefered type, in this case a string.
*/
VarType DateVar::GetVariableType() const{
	return VARTYPE_STRING;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
DateVar::Get____Data
NOTES: Where the real work is done.
*/
StringType DateVar::GetStringData() const
{
	time_t TmpTime = time(NULL);
	strftime( mpBuffer, mBufferSize, "%d/%m/%Y", localtime( &TmpTime ) );

	return NormalizeString(mpBuffer);
}

NumType DateVar::GetNumData() const
{
	time_t TmpTime = time(NULL);
	strftime( mpBuffer, mBufferSize, "%d%m%Y", localtime( &TmpTime ) );

	Variable TempVar( SS_BASE_ARGS_DEFAULTS, NormalizeString(mpBuffer) );
	TempVar.ForceConversion( VARTYPE_NUM );

	return TempVar.GetNumData();
}


BoolType DateVar::GetBoolData() const{
	return true;
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ElapsedSeconds::ElapsedSeconds/~ElapsedSeconds
NOTES: Nope, I'm not going to do it.  I'm sick of writing descriptions for
what everyone knows are constructors and destructors.
*/
ElapsedSeconds::ElapsedSeconds()
{
}

ElapsedSeconds::ElapsedSeconds( const STRING& Name,
							   bool Static /*= false*/, bool Const /*= false*/ )
							   : SpecialVarBase(Name, Static, Const)
{
}

ElapsedSeconds::~ElapsedSeconds()
{
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ElapsedSeconds::GetVariableType
NOTES: Elapsed seconds works best in number varieties.
*/
VarType ElapsedSeconds::GetVariableType() const{
	return VARTYPE_NUM;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ElapsedSeconds::Get____Data
NOTES: The guts.
*/
NumType ElapsedSeconds::GetNumData() const{
	return NumType( gProgramTimer.elapsed() );
}

StringType ElapsedSeconds::GetStringData() const{
	return NumType2StringType(this->GetNumData());
}

BoolType ElapsedSeconds::GetBoolData() const{
	return true;
}


//-----------TimeZoneVar::mBufferSize--------------
const int TimeZoneVar::mBufferSize = 64;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeZoneVar::TimeZoneVar/~TimeZoneVar
NOTES: 
*/
TimeZoneVar::TimeZoneVar(){
	mpBuffer = new char[mBufferSize];
}

TimeZoneVar::TimeZoneVar( const STRING& Name,
						 bool Static /*= false*/, bool Const /*= false*/ )
						 : SpecialVarBase( Name, Static, Const )
{
	mpBuffer = new char[mBufferSize];
}

TimeZoneVar::~TimeZoneVar()
{
	delete[] mpBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeZoneVar::GetVariableType
NOTES: It prefers string.
*/
VarType TimeZoneVar::GetVariableType() const{
	return VARTYPE_STRING;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeZoneVar::Get___Data
NOTES: 
*/
StringType TimeZoneVar::GetStringData() const
{
	time_t TmpTime = time(NULL);
	strftime( mpBuffer, mBufferSize, "%Z", localtime(&TmpTime) );

	return NormalizeString( mpBuffer );
}

NumType TimeZoneVar::GetNumData() const{
	return NumType( 1 );
}

BoolType TimeZoneVar::GetBoolData() const{
	return true;
}


