/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: 
 Basis for custom operators and built in operators/functions.
 Created: 24:5:2005   11:26
*/

 
 #if !defined(SS_Operator)
 #define SS_Operator
 
 
#include "Scope.hpp"
#include "Variable.hpp"
 
 
 
 
 namespace SS{
 
 
 /*
 For conveniance.  Use this to declare the contructor of children operators.
 */
#define SS_OP_DEFAULT_CTOR(x) x( const SS::STRING& Name, bool Static = false, bool Const = false )\
		 : Operator( Name, Static, Const ){}

//Even more conveniant.
#define SS_DECLARE_OPERATOR(x) \
class x : public Operator{\
public:\
	x( const SS::STRING& Name, bool Static = false, bool Const = false )\
		 : Operator( Name, Static, Const ){}\
	VariableBasePointer Operate( VariableBasePointer );\
 }
	
	
 
 
 
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~INTERFACE~~~~
 Operator
 NOTES: An abstract base class that defined the interface for custom operators.
 
*/
class SS_API Operator : public Scope
{
public:
	Operator();
	Operator( const SS::STRING& Name, bool Static = false, bool Const = false );
	
	void AcceptVisitor( ScopeObjectVisitor& );

	virtual OperatorPointer GetOperatorPtr();
	virtual const OperatorPointer GetOperatorPtr() const;
	
	virtual VariableBasePointer Operate( VariableBasePointer ) = 0;
	

private:
	void RegisterPredefinedVars();


};





 } //namespace
#endif


