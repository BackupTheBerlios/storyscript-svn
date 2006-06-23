/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are miscilaneous and used often.
*/

#if !defined(SS_Slib_Common)
#define SS_Slib_Common

#include "Scope.hpp"
#include "Operator.hpp"

namespace SS{
namespace SLib{
	

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Common
 NOTES: Special scope that creates an instance of all the math funcs/vars.
*/
class Common : public Scope
{
public:
	Common();

private:
	void RegisterPredefined();
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Contains magic variable that change the behavior of the language in
 		various ways.
*/
class LangOpts : public Scope
{
public:
	LangOpts();

private:
	void RegisterPredefined();
};




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 print
 NOTES: Prints something to the console.
*/
class print : public Operator
{
public:
	print( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ){}

	VariableBasePtr Operate( VariableBasePtr );
};




}} //namespaces
#endif 

