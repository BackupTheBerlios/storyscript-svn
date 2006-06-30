/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/


/**
	\file Operator.hpp
	\brief Declarations for Operator.  
*/
 
 #if !defined(SS_Operator)
 #define SS_Operator
 
 
#include "Scope.hpp"
#include "Variable.hpp"
 
 
 
 
 namespace SS{
 
 
/**
	\brief A conveniance macro for declaring operator derivatives' constructors.
	
	\param x The name of the the new operator derivative.
*/
#define SS_OP_DEFAULT_CTOR(x) x( const SS::STRING& Name, bool Static = false, bool Const = false )\
		 : Operator( Name, Static, Const ){}

/**
	\brief A conveniance macro for declaring operator derivatives.
	
	\param x The name of the the new operator derivative.
*/
#define SS_DECLARE_OPERATOR(x) \
class x : public Operator{\
public:\
	x( const SS::STRING& Name, bool Static = false, bool Const = false )\
		 : Operator( Name, Static, Const ){}\
	VariableBasePtr Operate( VariableBasePtr );\
 }
	
	
 
 
 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The base interface for object that can behave as unary operators
	(functions).
	
	To make your own built in function, simply overload this and define the
	Operate function.
	
	This can be done conveniently with the SS_DECLARE_OPERATOR or SS_OP_DEFAULT_CTOR
	macros.
*/
class SS_API Operator : public Scope
{
public:
	/// Default Constructor
	Operator();
	/// Constructor
	Operator( const SS::STRING& Name, bool Static = false, bool Const = false );
	
	void AcceptVisitor( ScopeObjectVisitor& );

	virtual OperatorPtr CastToOperator();
	virtual const OperatorPtr CastToOperator() const;
	
	/**
	\brief The function that gets called to perform the operator's operation.
	
	Make new fancy functions by defining/overriding this.
	
	\param Params The paramater list that gets passed the operator. (See List)
	*/
	virtual VariableBasePtr Operate( VariableBasePtr Params ) = 0;
	

private:
	/// Used by the contructor to initialize private data.
	void RegisterPredefinedVars();

};



 } //namespace
#endif


