/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are used for manipulating lists.
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
//SS_DECLARE_OPERATOR(Remove);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Behavior: Same as Remove, but instead of removing just the first
 				  occourance, it removes every occourance
*/
//SS_DECLARE_OPERATOR(RemoveAll);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Removes the last element of a list and returns it.
 		Takes exactly one argument.
*/
//SS_DECLARE_OPERATOR(Pop);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Number of arguments must be >= to 2.
 		First argument is a list, the other arguments are added to the end of the list.
*/
//SS_DECLARE_OPERATOR(Push);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This is the default sorting function.  I'm using quicksort for this.
 		Please bear with me as this is (believe it or not) the first time
 		I've ever written a sorting algorithim.
*/
class Sort : public Operator
{
public:
	SS_OP_DEFAULT_CTOR(Sort)
			
	VariableBasePtr Operate( VariableBasePtr );
	
private:
	ListPtr QuickSort( ListPtr, unsigned long Begin, unsigned long End );
	
	virtual int Compare( VariableBasePtr x, VariableBasePtr y );
	
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Simple reverses a list that it is given.  Doesn't modify the original.
*/
SS_DECLARE_OPERATOR(Reverse);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Randomly chooses one element from the list and returns it.
*/
SS_DECLARE_OPERATOR(PickOne);


}} //End namespaces
#endif
