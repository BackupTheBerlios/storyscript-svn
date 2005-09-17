/*
    Slib-List.hpp
    --------------------------
    Standard library functions for list manipulation.

	Created:	02:09:2005    16:16:50
*/


#if !defined( SS_Slib_List )
#define SS_Slib_List

#include "Operator.hpp"


namespace SS{
namespace SLib{
	
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: The scope in which all the list related function reside
		This is a fucking stupid idea to name the class the same as SS::List.
		Just for the record.
*/
class List : public Scope
{
public:
	List();
	
private:
	void RegisterPredefined();
};



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Behavior:
 		Number of elements < 2 - Does Nothing, return null
 		Number of elements > 2 - Interprets first element as a list,
 								 then removes the first occourance of
 								 all the other arguments from the list.
*/
SS_DECLARE_OPERATOR(Remove);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Behavior: Same as Remove, but instead of removing just the first
 				  occourance, it removes every occourance
*/
SS_DECLARE_OPERATOR(RemoveAll);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Removes the last element of a list and returns it.
 		Takes exactly one argument.
*/
SS_DECLARE_OPERATOR(Pop);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Number of arguments must be >= to 2.
 		First argument is a list, the other arguments are added to the end of the list.
*/
SS_DECLARE_OPERATOR(Push);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This is the default sorting function.  I'm using quicksort for this.
 		Please bear with me as this is (believe it or not) the first time
 		I've ever written a sorting algorithim.
*/
class Sort : public Operator
{
public:
	SS_OP_DEFAULT_CTOR(Sort)
			
	VariableBasePointer Operate( VariableBasePointer );
	
private:
	ListPointer QuickSort( ListPointer, unsigned long Begin, unsigned long End );
	
	virtual int Compare( VariableBasePointer x, VariableBasePointer y );
	
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Simple reverses a list that it is given.  Doesn't modify the original.
*/
SS_DECLARE_OPERATOR(Reverse);


}} //End namespaces
#endif
