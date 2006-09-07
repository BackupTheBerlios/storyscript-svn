/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are used for more advanced math operations.
*/

#if !defined(SS_Slib_Math)
#define SS_Slib_Math

#include "Scope.hpp"
#include "Operator.hpp"
#include "SpecialVars.hpp"

namespace SS{
namespace SLib{


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Math
 NOTES: Special scope that creates an instance of all the math funcs/vars.
*/
class Math : public Scope
{
public:
	Math();

private:
	void RegisterPredefined();
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~
// sqrt
// NOTES: The square root function.
//
SS_DECLARE_OPERATOR(sqrt);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 abs
 NOTES: The absolute value function
*/
SS_DECLARE_OPERATOR(abs);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 mean
 NOTES: Find the mean of a list of numbers
*/
SS_DECLARE_OPERATOR(mean);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 sin
 NOTES: The sine function.  (Input is in radians)
*/
SS_DECLARE_OPERATOR(sin);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 cos
 NOTES: The cosine function.  (Input is in radians)
*/
SS_DECLARE_OPERATOR(cos);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 tan
 NOTES: The tangent function.  (Input is in radians.)
*/
SS_DECLARE_OPERATOR(tan);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
sin
NOTES: The arc-sine function.  (Output is in radians)
*/
SS_DECLARE_OPERATOR(asin);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
cos
NOTES: The arc-cosine function.  (Output is in radians)
*/
SS_DECLARE_OPERATOR(acos);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
tan
NOTES: The arc-tangent function.  (Output is in radians.)
*/
SS_DECLARE_OPERATOR(atan);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 max
 NOTES: Returns the maximum from a list of elements.
*/
SS_DECLARE_OPERATOR(max);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
min
NOTES: Returns the minimum from a list of elements.
*/
SS_DECLARE_OPERATOR(min);


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Rounds to the nearest integer.  Rounds halfways away from 0.
*/
SS_DECLARE_OPERATOR(Int);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Rounds to the next lowest integer.
*/
SS_DECLARE_OPERATOR(Floor);



//class MathConstPrec;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Abstract Base Class that provides the basis for mathematical constant
 		magic vars.
*/
class MathConst : public SpecialVarBase
{
public:
	MathConst( SS_DECLARE_DEFAULTED_BASE_ARGS );
	
	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;
	
	friend class MathConstPrec;
	
protected:
	virtual void Generate() const = 0;
	
	mutable NumType mBufferValue;
	
};

/*
#define SS_DECLARE_MAGIC_NUMBER(x) \
class x : public MathConst{\
public:\
x( SS_DECLARE_DEFAULTED_BASE_ARGS );\
private:\
void Generate() const;\
}
*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: A special version of precision made for MathConst.
*/
class MathConstPrec : public SpecialVarBase
{
	public:
		MathConstPrec( const SS::STRING& Name, bool Const, MathConst& Parent );

		VariableBasePtr operator=( const VariableBase& );
	
		VarType GetVariableType() const;

		NumType GetNumData() const;
		BoolType GetBoolData() const;
		StringType GetStringData() const;
	
	private:
		MathConst& mParent;		
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Magical Pi!
*/
class Pi : public MathConst
{
public:
	Pi( SS_DECLARE_BASE_ARGS );
	
private:
	void Generate() const;
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Magical e!
*/
class Euler : public MathConst
{
public:
	Euler( SS_DECLARE_BASE_ARGS );
	
private:
	void Generate() const;
};
		
		
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Magical log2!
*/




}} //namespace(s)
#endif


