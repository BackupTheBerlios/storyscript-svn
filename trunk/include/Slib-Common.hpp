/*
    Slib-Common.hpp
    --------------------------
    Standard library functions that are automatically
	imported into the global scope.

	Created:	20:06:2005    (DONT KNOW THE DAMN TIME!)
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
 print
 NOTES: Prints something to the console.
*/
class print : public Operator
{
public:
	print( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ){}

	VariableBasePointer Operate( VariableBasePointer );
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Randomly chooses one element from the list and returns it.
*/
SS_DECLARE_OPERATOR(PickOne);


}} //namespaces
#endif 

