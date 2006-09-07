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
 : Scope( LC_SL_Time, false )
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
	Register( ScopeObjectPtr( new TimeVar       ( TXT("time"),            true ) ) );
	Register( ScopeObjectPtr( new ElapsedSeconds( TXT("elapsed_seconds"), true ) ) );
	Register( ScopeObjectPtr( new DateVar       ( TXT("date"),            true ) ) );
	Register( ScopeObjectPtr( new TimeZoneVar   ( TXT("timezone"),        true ) ) );
}




//--------TimeVar::mBufferSize---------
const int TimeVar::mBufferSize = 64;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
TimeVar::TimeVar/~TimeVar
NOTES: Do I really have to write some stupid "Ctor & Dtor" shit for every one
of these.
*/
TimeVar::TimeVar( SS_DECLARE_BASE_ARGS )
				 : SpecialVarBase( SS_BASE_ARGS )
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

	VariablePtr pIntTime( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, NormalizeString( mpBuffer ) ) );
	pIntTime->ForceConversion( VARTYPE_NUM );

	return pIntTime->GetNumData();
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
DateVar::DateVar( SS_DECLARE_BASE_ARGS)
				 : SpecialVarBase(SS_BASE_ARGS)
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

	VariablePtr pTempVar( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, NormalizeString(mpBuffer) ) );
	pTempVar->ForceConversion( VARTYPE_NUM );

	return pTempVar->GetNumData();
}


BoolType DateVar::GetBoolData() const{
	return true;
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ElapsedSeconds::ElapsedSeconds/~ElapsedSeconds
NOTES: Nope, I'm not going to do it.  I'm sick of writing descriptions for
what everyone knows are constructors and destructors.
*/
ElapsedSeconds::ElapsedSeconds( SS_DECLARE_BASE_ARGS )
							   : SpecialVarBase(SS_BASE_ARGS)
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
	NumType New;
	mpfr_set_d( New.get(), gProgramTimer.elapsed(), LangOpts::Instance().RoundingMode );
	return New;
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
TimeZoneVar::TimeZoneVar( SS_DECLARE_BASE_ARGS )
						 : SpecialVarBase( SS_BASE_ARGS )
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


