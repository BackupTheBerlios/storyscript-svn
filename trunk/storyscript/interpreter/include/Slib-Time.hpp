/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are used to maniuplate time.
*/

#if !defined(SS_Slib_Time)
#define SS_Slib_Time


#include "Scope.hpp"
#include "SpecialVars.hpp"

namespace SS{
namespace SLib{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 SLib-Time
 NOTES: A special scope that holds a single instance of all the special time
		functions/variables.
*/
class Time : public Scope
{
public:
	Time();

private:
	void RegisterPredefined();

};





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
TimeVar
NOTES: Special variable that always reports the current time.
*/
class TimeVar : public SpecialVarBase
{
public:
	TimeVar();
	TimeVar( const SS::STRING& Name,
		bool Static = false, bool Const = false );
	~TimeVar();



	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	char* mpBuffer;
	static const int mBufferSize;

};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
DateVar
NOTES: Reports the data.  As a string "03/21/05" or as a number "032105"
*/
class DateVar : public SpecialVarBase
{
public:
	DateVar();
	DateVar( const SS::STRING& Name, 
		bool Static = false, bool Const = false );
	~DateVar();

	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	char* mpBuffer;
	static const int mBufferSize;
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ElapsedSeconds
NOTES: This is a special var which returns the number of seconds that the
Interprete has been running.  It is useful only for comparisons to
test if a certain ammount of time has gone by.
*/
class ElapsedSeconds : public SpecialVarBase
{
public:
	ElapsedSeconds();
	ElapsedSeconds( const SS::STRING& Name,
		bool Static = false, bool Const = false);
	~ElapsedSeconds();


	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:

};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
TimeZone
NOTES: Returns a string with the current time zone name.
*/
class TimeZoneVar : public SpecialVarBase
{
public:
	TimeZoneVar();
	TimeZoneVar( const SS::STRING& Name,
		bool Static = false, bool Const = false);
	~TimeZoneVar();

	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	char* mpBuffer;
	static const int mBufferSize;
};



}}
#endif

