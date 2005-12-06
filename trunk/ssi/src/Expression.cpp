/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains defs for the Expression class which is responsible for
	interpreting and executing expressions.
*/


#include "Expression.hpp"
#include "List.hpp"
#include "Interpreter.hpp"
#include "Interface.hpp"
#include "Operator.hpp"
#include "Unicode.hpp"
#include "CreationFuncs.hpp"
#include <boost/lexical_cast.hpp>

using namespace SS;



std::vector<VariableBasePtr> Expression::mUnnamedVariables;




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: This is on the verge of a hack.  There are certain cases when Expression
 		needs to return an identifier name.
 		
 		Such as, "(SomeScope):Foo",
 		or "var Foo",
 		or "use Foo".
 		
 		This takes care of that.  If you try to use it like a normal VariableBase,
 		it will give you an error that the value can't be found.
 		
 		The special cases that need it need to do a dynamic_cast to get it.
*/
class LooseIdentifier : public VariableBase
{
public:
	LooseIdentifier( const STRING& LooseID ) : LooseID( LooseID )
	{}
	
	StringType GetStringData() const{
		ThrowError(); 
		return StringType();
	}
	BoolType GetBoolData() const {
		ThrowError(); 
		return BoolType();
	}
	NumType GetNumData()  const {
		ThrowError(); 
		return NumType();
	}
	
	const STRING& GetLooseIDName(){ return LooseID; };
	
				
private:
	void ThrowError() const
	{
		STRING Tmp = TXT("Cannot find object named \'");
		Tmp += LooseID;
		Tmp += TXT("\'.");
		ThrowParserAnomaly( Tmp, ANOMALY_IDNOTFOUND );
	}
	
	STRING LooseID;
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::Expression
 NOTES: constructors
*/
Expression::Expression()
	: mStatic(false), mpWordList( new WordList ),
      mLowerBounds(0), mUpperBounds(~0U)
{
}


Expression::Expression( const Expression& OtherExp,
						unsigned int LowerBounds /*=0*/,
						unsigned int UpperBounds /*=~0U*/ )
	: mStatic(OtherExp.mStatic), mpWordList( OtherExp.mpWordList ),
 	  mLowerBounds(LowerBounds), mUpperBounds(UpperBounds)
{
	if( UpperBounds < LowerBounds )
	{
		ThrowParserAnomaly( TXT("UpperBounds is less than LowerBounds in"
							" Expression initialization.  This indicates"
							" a very serious bug."), ANOMALY_PANIC );
	}
}

//This one is only for internal use
/*
Expression::Expression( std::vector<Word>::iterator First, std::vector<Word>::iterator Last )
	: mStatic(false), mWordList( First, Last )
{
}
*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::operator=
 NOTES: Assignment
*/
Expression& Expression::operator=( const Expression& OtherExp )
{
	mpWordList = OtherExp.mpWordList;
	mLowerBounds = OtherExp.mLowerBounds;
	mUpperBounds = OtherExp.mUpperBounds;
	mStatic = OtherExp.mStatic;
	return *this;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: You need to inform an expression if it is static so that it knows if
 		new objects should be created as static or non-static.
*/
bool Expression::IsStatic() const{
	return mStatic;
}

void Expression::SetStatic( bool flag /*= true*/ ) const{
	mStatic = flag;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::______
 NOTES: Wrapper's for std::vector.  All indexes are in terms of
 		mLowerBounds and mUpperBounds.
*/
Word& Expression::operator[]( unsigned long i )
{
	return (*mpWordList)[ GetAbsoluteIndex(i) ];
}

const Word& Expression::operator[]( unsigned long i ) const
{
	return (*mpWordList)[ GetAbsoluteIndex(i) ];
}

void Expression::clear()
{
	if( mpWordList.unique() ) mpWordList->clear();
	else mpWordList.reset( new WordList );

	mLowerBounds = 0;
	mUpperBounds = ~0U;

	mStatic = false;		
}

void Expression::push_back( const Word& SomeWord )
{
	RevertToLocalCopy();
	mpWordList->push_back( SomeWord );
}

void Expression::pop_back()
{
	RevertToLocalCopy();	
	mpWordList->pop_back();
}

bool Expression::empty() const
{
	if( mpWordList->empty() || mLowerBounds == mUpperBounds ) return true;
	else return false;
}

unsigned long Expression::size() const
{
	//Special case, because this give bounds of 0 otherwise
	unsigned long Bounds = mUpperBounds - mLowerBounds;

	if( Bounds < mpWordList->size() ) return Bounds;
	else return (unsigned long)mpWordList->size() - mLowerBounds;
}

void Expression::erase( unsigned long i )
{
	RevertToLocalCopy();
	mpWordList->erase( mpWordList->begin() + i ); 	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This converts relative coordinates (i.e. ones that are in terms of there
 		upper and lower bounds) to absolute coordinates (one that are in terms
 		of the actual vector).
*/
unsigned long Expression::GetAbsoluteIndex( unsigned long i ) const
{
	unsigned long ProposedIndex = i + mLowerBounds;
	
	if( ProposedIndex >= mUpperBounds ||
		ProposedIndex >= mpWordList->size() ){
		ThrowParserAnomaly( TXT("Expression index is out of range.  This is a serious bug."),
							ANOMALY_PANIC );
	}
	else return ProposedIndex;		
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: In a Expression using someone elses WordList, this will convert it
		to a local copy.
*/
void Expression::RevertToLocalCopy()
{
	if( !mpWordList.unique() )
	{
		boost::shared_ptr<WordList> pNewList( new WordList );

		unsigned long i;
		for( i = mLowerBounds; i < mUpperBounds && i < mpWordList->size(); i++ )
		{
            pNewList->push_back( (*mpWordList)[i] );
		}

		mpWordList = pNewList;
		mLowerBounds = 0;
		mUpperBounds = ~0U;
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::Interpret
 NOTES: Calls RealInterpret.  This a saftey layer between RealInterpret and
		the user, so the user doesn't try to pass parameter to it and screw
		it up.
*/
VariableBasePtr Expression::Evaluate() const
{
	return RealInterpret();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MONOLITHIC~FUNCTION~~~~~~
 Expression::RealInterpret
 NOTES: Where the real work is done.  This uses a divide and conqueror approach
		to figure out the the result of the expression.

		INPUTS:
		TopLevel - True if another function is calling ParseExpression, false if its calling itself
		Before - The word that comes before the given "slice" of the expression
		After - The word that comes after.
		PreValue - Used if for some reason a value needs to be inserted before WordBuffer (in cases where
				   expressions beging with a binary operator).
		PostValue - Used if a value needs to be added after WordBuffer (in cases with hanging operators)

		OUTPUT:
		Returns the result of the expression.
*/
VariableBasePtr Expression::RealInterpret( bool TopLevel /*=true*/,
											   Word Before /*=NULL_WORD*/,
											   Word After /*=NULL_WORD*/,
											   VariableBasePtr PostValue /*=VariableBasePtr()*/ ) const
{
	//Verify that the expression has proper syntax.
	//Note that this does not check for attempting to assign to a constant.
	//Nor does it check for identifier that do not exist.

	if( size() == 0 ){
		ThrowParserAnomaly( 
			TXT("Tried to parse an empty expression.  This means there "
			"is a serious bug in the interpreter.  Please, please, "
			"please report this to the author!"),
			ANOMALY_PANIC );
	}

    StripOutlyingParenthesis();
	
	if( size() == 1 )
	{
		if( (*this)[0].Type == WORDTYPE_IDENTIFIER )
		{
			//This is a bit of a hack to allow the : operator to function.
			//Any identifier after it will be converted to a string variable.
			if( Before.Extra == EXTRA_BINOP_ScopeResolution  )
			{
				VariableBasePtr pTempVar( new LooseIdentifier( (*this)[0].String ) );
				pTempVar->SetSharedPtr( pTempVar );
				mUnnamedVariables.push_back( pTempVar );
				return pTempVar;				
			}
			else
			{
				/*
				If an identifier exists, it is returns as a VariableBase,
				If it does not, it is returned as a special LooseIdentifier object, for use
				by special cases such as object declarations.
				*/
				try{
					return Interpreter::Instance().GetScopeObject( (*this)[0].String )->CastToVariableBase();
				}
				catch( ParserAnomaly E )
				{
					if( E.ErrorCode == ANOMALY_IDNOTFOUND )
					{
						VariableBasePtr pTempVar( new LooseIdentifier( (*this)[0].String ) );
						pTempVar->SetSharedPtr( pTempVar );
						mUnnamedVariables.push_back( pTempVar );
						return pTempVar;												
					}
					else throw;
				}
			}
		}
		else if( (*this)[0].Extra == EXTRA_BOOLLITERAL_True )
		{
			VariablePtr pTempVar( CreateVariable<Variable>( UNNAMMED, false, true, true ) );
			mUnnamedVariables.push_back( pTempVar );
			return pTempVar;
		}
		else if( (*this)[0].Extra == EXTRA_BOOLLITERAL_False )
		{
			VariablePtr pTempVar( CreateVariable<Variable>( UNNAMMED, false, true, false ) );
			mUnnamedVariables.push_back( pTempVar );
			return pTempVar;
		}
		else if( (*this)[0].Type == WORDTYPE_STRINGLITERAL ||
				 (*this)[0].Type == WORDTYPE_FLOATLITERAL )
		{
			//Here is where the effectiveness of my autoconversions get tested.
			VariablePtr pTempVar( CreateVariable<Variable>( UNNAMMED, false, true, (*this)[0].String ) );
			mUnnamedVariables.push_back( pTempVar );

			if( (*this)[0].Type == WORDTYPE_FLOATLITERAL ) {
				pTempVar->ForceConversion( VARTYPE_NUM );
			}

			return pTempVar;
		}
		else if( (*this)[0].Type == WORDTYPE_EMPTYLISTLITERAL )
		{
			return gpEmptyList->CastToVariableBase();
		}
		else
		{
			ThrowParserAnomaly( TXT("Expression::Interpret is confused as hell."), ANOMALY_BADGRAMMAR );
		}
	}
	
	//This should weed out anything really fatal
	CheckSyntax( (bool)PostValue );	
	

	//Find the low precedence operator.
	bool LastWasOperator = false;
	unsigned int i;
	unsigned int ParenthesisCount = 0;
	unsigned int LowPrecedenceOpIndex = ~0;
	for( i = 0; i < size(); i++ )
	{
		if( (*this)[i].Extra == EXTRA_PARENTHESIS_Left ){
			ParenthesisCount++;
			LastWasOperator = false;
			continue;
		}
		else if( (*this)[i].Extra == EXTRA_PARENTHESIS_Right ){
			ParenthesisCount--;
			LastWasOperator = false;
			continue;
		}

		if( ParenthesisCount != 0 ) continue;

		//Intercept ambiguous operators and figure out whether they
		//should be binary or unary.
		if( (*this)[i].Type == WORDTYPE_AMBIGUOUSOPERATOR )
		{
			if( i == 0 || LastWasOperator == true ||
				(i != 0 && (*this)[i-1].Extra == EXTRA_PARENTHESIS_Right ) )
			{
				(*this)[i].InterpretAsUnaryOp();
			}
			else
			{
				(*this)[i].InterpretAsBinaryOp();
			}
		}
		//Test if it is a user defined or SLib operator
		if( (*this)[i].Type == WORDTYPE_IDENTIFIER )
		{
			ScopeObjectType ObjType;

			try{
				ObjType = GetScopeObjectType(
					Interpreter::Instance().GetScopeObject( (*this)[i].String ) );
			}
			//If its an unkown ID, we just ignore it here and it will get dealt with later.
			catch( ParserAnomaly E )
			{
				if( E.ErrorCode == ANOMALY_IDNOTFOUND ){
					LastWasOperator = false;
					continue;
				}
				else throw;
			}

			if( ObjType == SCOPEOBJ_OPERATOR || ObjType == SCOPEOBJ_BLOCK )
			{
				//This part figures out whether to interpret this thing
				//as an operator or just as a block
				if( i == size()- 1 ||
					(i < size() - 1 && 
					((*this)[i+1].Type == WORDTYPE_BINARYOPERATOR ||
					(*this)[i+1].Type == WORDTYPE_AMBIGUOUSOPERATOR ||
					(*this)[i+1].Extra == EXTRA_PARENTHESIS_Right)) )
				{
					LastWasOperator = false;
					continue;
				}
				
				//Translation; If "foo" is the last word, or is followed by binop, ambigop, or ),
				//			   then interpret it as an ID.  Otherwise its an operator.




				if( LowPrecedenceOpIndex == ~0 ||
					Precedence((*this)[i]) < Precedence((*this)[LowPrecedenceOpIndex]) )
				{
					LowPrecedenceOpIndex = i;
				}

				LastWasOperator = true;
				continue;
			}
			else
			{
				LastWasOperator = false;
				continue;
			}
		}
		else if( (*this)[i].Type != WORDTYPE_BINARYOPERATOR &&
				 (*this)[i].Type != WORDTYPE_UNARYOPERATOR )
		{
			LastWasOperator = false;
			continue;
		}
		else LastWasOperator = true;
	

		//This is very important:  If the comparison is <=, it will execute left->right,
		//		if it's just <, it will execute right->left.
		if( LowPrecedenceOpIndex == ~0 ||
			Precedence((*this)[i]) <= Precedence((*this)[LowPrecedenceOpIndex]) )
		{
			LowPrecedenceOpIndex = i;
		}
	}

	//I'm not really sure if this is possible, but
	//it is better error on the side of caution
	if( LowPrecedenceOpIndex == ~0 ){
		ThrowExpressionAnomaly( TXT("Can't find a low precedence operator.  This means"
							" that there is a serious bug in the interpreter.  Please"
							" contact the author."), ANOMALY_PANIC );		
	}


	const ExtraDesc& LowPrecedenceOp = (*this)[LowPrecedenceOpIndex].Extra;

	//ONLY FOR DEBUGGING
	//STRING Dump = DumpToString();
	
	const Expression Left( *this, GetAbsoluteIndex(0), GetAbsoluteIndex(LowPrecedenceOpIndex) );

	unsigned long RightLower, RightUpper;
	if( LowPrecedenceOpIndex + 1 < size() )	{ 
		RightLower = GetAbsoluteIndex(LowPrecedenceOpIndex + 1);
		RightUpper = mUpperBounds;		
	}
	else{
		RightLower = 0;
		RightUpper = 0;
	}

	const Expression Right( *this, RightLower, RightUpper );

	if( IsStatic() ){
		Left.SetStatic();
		Right.SetStatic();
	}
	

	VariableBasePtr pLeftVar, pRightVar, pResultant;
	
	
	//SPECIAL CASES for Short-circuting in logical operators
	// This prevents the right side from being evaluated if the left side is true/false.
	if( LowPrecedenceOp == EXTRA_BINOP_LogicalOr ||
		LowPrecedenceOp == EXTRA_BINOP_LogicalAnd )
	{
		pLeftVar =
			Left.RealInterpret( false,
							Before, //Before Word
							(*this)[LowPrecedenceOpIndex] ); //After Word
		
		
		if( LowPrecedenceOp == EXTRA_BINOP_LogicalOr && pLeftVar->GetBoolData() == true ){
			return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, true );
		}
		else if( LowPrecedenceOp == EXTRA_BINOP_LogicalAnd && pLeftVar->GetBoolData() == false ){
			return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
		}
		
		//Shit, now we have to deal with the right side!
		//Don't worry, if we just let it fall through,
		//and stop it from re-evaluating the left-side, we'll be fine.
				

	}
	
	
	


	if( !Right.empty() )
	{
		pRightVar = 
			Right.RealInterpret( false,
								 (*this)[LowPrecedenceOpIndex], //Before Word
								 After, //After Word
								 PostValue ); //Remeber to pass this on.
	}
	else if( PostValue )
	{
		pRightVar = PostValue;
	}
	//UserOps at the end of an expression sometimes get flagged as the low-precedence op
	//even though they should be interpreted as Variables.
	else if( (*this)[LowPrecedenceOpIndex].Type == WORDTYPE_IDENTIFIER )
	{
		pResultant = Interpreter::Instance().GetScopeObject( (*this)[LowPrecedenceOpIndex].String )->CastToVariableBase();

		//Now we handle the left part.
		if( !Left.empty() )
		{
			return Left.RealInterpret( false, Before, (*this)[LowPrecedenceOpIndex],
									   pResultant );
		}
		else
		{
			//It should under no circumstances be able to get here.
			ThrowExpressionAnomaly( TXT("Catastraphic expression parsing error."), ANOMALY_PANIC );
		}
	}
	else
	{
		ThrowExpressionAnomaly( TXT("Trailing operator with no argument."),
								ANOMALY_BADGRAMMAR );
	}


	//EXAUSTIVE LISTING OF UNARY OPERATORS
	if( (*this)[LowPrecedenceOpIndex].Type == WORDTYPE_UNARYOPERATOR ||
		(*this)[LowPrecedenceOpIndex].Type == WORDTYPE_AMBIGUOUSOPERATOR ||
		(*this)[LowPrecedenceOpIndex].Type == WORDTYPE_IDENTIFIER )
	{
		// not
		if( LowPrecedenceOp == EXTRA_UNOP_Not ){
			pResultant = pRightVar->op_not();
		}
		//-
		else if( LowPrecedenceOp == EXTRA_UNOP_Negative || 
				 ((*this)[LowPrecedenceOpIndex].Type == WORDTYPE_AMBIGUOUSOPERATOR &&
				  LowPrecedenceOp == EXTRA_BINOP_Minus ) )
		{
			pResultant = pRightVar->op_neg();			
		}
		//import
		else if( LowPrecedenceOp == EXTRA_UNOP_Import ){
			//Identifiers will always start with ':'
			if( pRightVar->GetStringData()[0] == LC_ScopeResolution[0] )
			{
				Interpreter::Instance().ImportIntoCurrentScope(
						pRightVar->GetStringData() );
			}
			else
			{
				Interpreter::Instance().ImportFileIntoCurrentScope(
						pRightVar->GetStringData() );								
			}			
						
			pResultant = pRightVar;
		}
		//Declarations
		else if( LowPrecedenceOp == EXTRA_UNOP_Var ||
				 LowPrecedenceOp == EXTRA_UNOP_List || 
				 LowPrecedenceOp == EXTRA_UNOP_Character || 
				 LowPrecedenceOp == EXTRA_UNOP_Player )
		{
			boost::shared_ptr<LooseIdentifier> pLooseID;
			if( pLooseID = boost::dynamic_pointer_cast<LooseIdentifier>( pRightVar ) )
			{
				if( LowPrecedenceOp == EXTRA_UNOP_Var )
				{
					pResultant = 
						Interpreter::Instance().MakeScopeObject( 
						SCOPEOBJ_VARIABLE, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
				}
				else if( LowPrecedenceOp == EXTRA_UNOP_List )
				{
					pResultant = 
						Interpreter::Instance().MakeScopeObject( 
						SCOPEOBJ_LIST, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
				}
				else if( LowPrecedenceOp == EXTRA_UNOP_Player )
				{
					//HACK: This says player, but currently it just creates a standard character.
					//		At this point there is no spereation between players and characters.
					pResultant = 
						Interpreter::Instance().MakeScopeObject( 
						SCOPEOBJ_CHARACTER, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
				}
				else if( LowPrecedenceOp == EXTRA_UNOP_Character )
				{
					pResultant = 
						Interpreter::Instance().MakeScopeObject( 
						SCOPEOBJ_CHARACTER, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
				}
				else
				{
					//This will generate an error saying that the identifier cannot be found.
					pRightVar->GetStringData();
				}
			}
			//Trying to declare a variable with something other than an unused identifier.
			else
			{
				ThrowExpressionAnomaly( TXT("Bad declaration syntax.  Either you tried to declare"
										" an object with an identifier already being used, or you"
										" tried to declare an object using a literal."),
										ANOMALY_BADDECLARATION );
			}
		}
		//Try to interpret it as a function
		else
		{
			OperatorPtr pOp = 
				Interpreter::Instance().GetScopeObject( (*this)[LowPrecedenceOpIndex].String )->CastToOperator();
			pResultant = pOp->Operate( pRightVar );
		}

		//Now we handle the left part.
		if( !Left.empty() )
		{
			return Left.RealInterpret( false, Before, (*this)[LowPrecedenceOpIndex],
									   pResultant );
		}

        return pResultant;
	}
			

	//From here on we assume that we are dealing with a binary operator.
	


	//..and we only need the leftvar if it is a binary op.
	
	if( !Left.empty() ) 
	{
		//Remember, the left side may have already been evaluated if we're dealing with
		//the and/or operators.  Thats what the !pLeftVar test is for.
		if( !pLeftVar ){
			pLeftVar = 
				Left.RealInterpret( false,
								Before, //Before Word
								(*this)[LowPrecedenceOpIndex] ); //After Word
		}
	}
	else
	{
		ThrowExpressionAnomaly( TXT("Expression beginning with binary operator.  No left-side value."),
								ANOMALY_BADGRAMMAR );
	}
	
	
	//EXAUSTIVE LISTING OF BINARY OPERATORS

	// -
	if( LowPrecedenceOp == EXTRA_BINOP_Minus ){
		pResultant = *pLeftVar - *pRightVar;
	}
	// +
	else if( LowPrecedenceOp == EXTRA_BINOP_Plus ){
		pResultant = *pLeftVar + *pRightVar;
	}
	// /
	else if( LowPrecedenceOp == EXTRA_BINOP_Divide ){
		pResultant = *pLeftVar / *pRightVar;
	}
	// *
	else if( LowPrecedenceOp == EXTRA_BINOP_Times ){
		pResultant = *pLeftVar * *pRightVar;
	}
	// **
	else if( LowPrecedenceOp == EXTRA_BINOP_Exponent ){
		pResultant = pLeftVar->operator_pow( *pRightVar );
	}
	// >
	else if( LowPrecedenceOp == EXTRA_BINOP_LargerThan ){
		pResultant = *pLeftVar > *pRightVar;
	}
	// <
	else if( LowPrecedenceOp == EXTRA_BINOP_LessThan ){
		pResultant = *pLeftVar < *pRightVar;
	}
	// >=
	else if( LowPrecedenceOp == EXTRA_BINOP_LargerThanOrEqual ){
		pResultant = *pLeftVar >= *pRightVar;
	}
	// <=
	else if( LowPrecedenceOp == EXTRA_BINOP_LessThanOrEqual ){
		pResultant = *pLeftVar <= *pRightVar;
	}
	// ==
	else if( LowPrecedenceOp == EXTRA_BINOP_Equals ){
		pResultant = *pLeftVar == *pRightVar;
	}
	// !=
	else if( LowPrecedenceOp == EXTRA_BINOP_NotEquals ){
		pResultant = *pLeftVar != *pRightVar;
	}
	// &=
	else if( LowPrecedenceOp == EXTRA_BINOP_LogicalAnd ){
		pResultant = *pLeftVar && *pRightVar;
	}
	// ||
	else if( LowPrecedenceOp == EXTRA_BINOP_LogicalOr ){
		pResultant = *pLeftVar || *pRightVar;
	}
	// -=
	else if( LowPrecedenceOp == EXTRA_BINOP_MinusAssign ){
		pResultant = (*pLeftVar = *(*pLeftVar - *pRightVar));
	}
	// +=
	else if( LowPrecedenceOp == EXTRA_BINOP_PlusAssign ){
		pResultant = (*pLeftVar = *(*pLeftVar + *pRightVar));
	}
	// /=
	else if( LowPrecedenceOp == EXTRA_BINOP_DivideAssign ){
		pResultant = (*pLeftVar = *(*pLeftVar / *pRightVar));
	}
	// *=
	else if( LowPrecedenceOp == EXTRA_BINOP_TimesAssign ){
		pResultant = (*pLeftVar = *(*pLeftVar * *pRightVar));
	}
	// **=
	else if( LowPrecedenceOp == EXTRA_BINOP_ExponentAssign ){
		pResultant = (*pLeftVar = *(pLeftVar->operator_pow( *pRightVar ) ) );
	}
	else if( LowPrecedenceOp == EXTRA_BINOP_Concat ){
		pResultant = pLeftVar->operator_concat( *pRightVar );
	}
	else if( LowPrecedenceOp == EXTRA_BINOP_ConcatAssign ){
		pResultant = (*pLeftVar = *(pLeftVar->operator_concat( *pRightVar ) ) );
	}
	// =
	else if( LowPrecedenceOp == EXTRA_BINOP_Assign ){
		pResultant = (*pLeftVar = *pRightVar);
	}
	// []
	else if( LowPrecedenceOp == EXTRA_BINOP_ListAccess )
	{		
		pResultant = (*(pLeftVar->CastToList()))[ pRightVar ];
	}
	// +[]
	else if( LowPrecedenceOp == EXTRA_BINOP_ListAppend )
	{
		pResultant = pLeftVar->CastToList()->Insert( pRightVar );
	}
	// -[]
	else if( LowPrecedenceOp == EXTRA_BINOP_ListRemove )
	{
		pResultant = pLeftVar->CastToList()->Remove( pRightVar );
	}
	// ,
	else if( LowPrecedenceOp == EXTRA_BINOP_ListSeperator )
	{
		
		/*
		ListPtr pNewList( Creator::CreateObject<List>( SS_BASE_ARGS_DEFAULTS ) );
		mUnnamedVariables.push_back( pNewList );

		*pNewList = *(pLeftVar->CastToList());
		pNewList->Append( pRightVar->CastToList() );
		pNewList->SetConst();

		pResultant = pNewList;
		*/
		
		/*
		if( GetScopeObjectType( pLeftVar ) == SCOPEOBJ_LIST &&
		    pLeftVar->CastToList()->IsConst() &&
			pLeftVar->GetName() == UNNAMMED )
		{
			pLeftVar->CastToList()->Push( pRightVar );
			pResultant = pLeftVar;			
		}
		else
		{
			ListPtr pNewList( CreateGeneric<List>( SS_BASE_ARGS_DEFAULTS ) );
			
			pNewList->AppendWithoutCopy( pLeftVar );
			pNewList->AppendWithoutCopy( pRightVar );
			pNewList->SetConst();
			
			pResultant = pNewList;
		}
		*/
		
		//Third try's the charm.
		
		//For debugging only:
		//ScopeObjectType TempLeft = GetScopeObjectType( pLeftVar );
		//ScopeObjectType TempRight = GetScopeObjectType( pRightVar );
		
		if( GetScopeObjectType( pLeftVar ) == SCOPEOBJ_LIST && //If pLeftVar is a list
			pLeftVar->IsConst() && 						   //that is constant
			pLeftVar->GetName() == UNNAMMED ) 			   //and unnamed.
		{
			//If the right var is an actual variable and not something special.
			if( GetScopeObjectType( pRightVar ) == SCOPEOBJ_VARIABLE )
			{
				pLeftVar->CastToList()->PushWithoutCopy( pRightVar );
			}
			//If it is something special, just make a new variable and copy the value of rightvar.
			else
			{
				pLeftVar->CastToList()->Push( pRightVar );
			}
			
			pResultant = pLeftVar;
		}
		//pLeftVar is not a list, so we have to create a new one.
		else
		{
			ListPtr pNewList( CreateGeneric<List>( UNNAMMED, false, true ) );
			
			//If its a real variable, append the actual pointer,
			//otherwise make a copy of the value.
			if( GetScopeObjectType( pLeftVar ) == SCOPEOBJ_VARIABLE ){
				 pNewList->PushWithoutCopy( pLeftVar );
			}
			else pNewList->Push( pLeftVar );
			
			if( GetScopeObjectType( pRightVar ) == SCOPEOBJ_VARIABLE ){
				 pNewList->PushWithoutCopy( pRightVar );
			}
			else pNewList->Push( pRightVar );
			
			pResultant = pNewList->CastToVariableBase();			
		}
		
		//Otherwise create a new a list and append the actual pointers
		
	}
	// :
	else if( LowPrecedenceOp == EXTRA_BINOP_ScopeResolution )
	{
		//pResultant = (pLeftVar->GetScopeObject( pRightVar->GetStringData() ))->CastToVariableBase();
		
		//Try to interpret it as a LooseID
		boost::shared_ptr<LooseIdentifier> pRightLooseID = boost::dynamic_pointer_cast<LooseIdentifier>(pRightVar);
		if( pRightLooseID )
		{
			pResultant = (pLeftVar->GetScopeObject( pRightLooseID->GetLooseIDName() ))->CastToVariableBase();
		}
		//Otherwise just assume its a string literal of some
		//TODO: Is this really a good idea?  What if someone puts a variable after a scope resolutio operator?
		//		Won't it read the string from the variable and try to access that variable.  Is that what I want?
		else
		{
			//TODO: Maybe get the partial name instead???
			pResultant = (pLeftVar->GetScopeObject( pRightVar->GetStringData() ))->CastToVariableBase();
		}
	}
	//No operator found!
	else
	{
		STRING tmp = TXT("Binary operator \'");
		tmp += LowPrecedenceOp;
		tmp += TXT("\' is unknown.");
		ThrowParserAnomaly( tmp, ANOMALY_UNKNOWNOP );
	}

	if( TopLevel ) mUnnamedVariables.clear();
	return pResultant;

}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::CheckSyntax
 NOTES: Checks the expression for obvious syntax errors.
*/
void Expression::CheckSyntax( bool IgnoreTrailingOps /*=false*/ ) const
{
	Word PrevWord;
	Word CurrentWord;

	unsigned int i;
	unsigned int ParenthesisCount = 0;
	for( i = 0; i < size(); i++, PrevWord = CurrentWord )
	{
		CurrentWord = (*this)[i];

		//RULE: All ')' must have a matching '('
		if( CurrentWord.Extra == EXTRA_PARENTHESIS_Left ) ParenthesisCount++;
		else if( CurrentWord.Extra == EXTRA_PARENTHESIS_Right )
		{
			if( ParenthesisCount == 0 )
			{
				ThrowExpressionAnomaly( TXT("Extra \')\'.  Can't find the matching \'(\'."),
										ANOMALY_BADPUNCTUATION );
			}
			else ParenthesisCount--;
		}

		//RULE: Expressions must begin with an identifier, literal, or '(', or (unary) operator
		if( PrevWord.Type == WORDTYPE_UNKNOWN && 
			!(CurrentWord.Type == WORDTYPE_IDENTIFIER || 
			CurrentWord.IsLiteral() ||
			CurrentWord.Extra == EXTRA_PARENTHESIS_Left ||
			CurrentWord.Type == WORDTYPE_UNARYOPERATOR ||
			CurrentWord.Type == WORDTYPE_AMBIGUOUSOPERATOR )  )
		{
			ThrowExpressionAnomaly( TXT("Expressions must begin with an identifier, literal, or unary operator."),
								    ANOMALY_BADGRAMMAR );
		}
		//RULE: Either an identifier, literal, or '(' must follow an operator
		else if( (PrevWord.Type == WORDTYPE_BINARYOPERATOR || 
				  PrevWord.Type == WORDTYPE_UNARYOPERATOR ||
				  PrevWord.Type == WORDTYPE_AMBIGUOUSOPERATOR ) &&
				  !( CurrentWord.IsLiteral() || CurrentWord.Type == WORDTYPE_IDENTIFIER ||
				 CurrentWord.Extra == EXTRA_PARENTHESIS_Left || CurrentWord.Type == WORDTYPE_UNARYOPERATOR ) )
		{
			ThrowExpressionAnomaly( TXT("Either an identifier, literal, or a unary operator must follow an operator"),
								    ANOMALY_BADGRAMMAR );
		}
		//RULE: operators can not be followed by ')' or preceded by '('.  (i.e. No "x (+) b" stuff)
		else if( PrevWord.Extra == EXTRA_PARENTHESIS_Left &&
				 CurrentWord.Type == WORDTYPE_BINARYOPERATOR )
		{
			ThrowExpressionAnomaly( TXT("Operators cannot be preceded by a \'(\'."), ANOMALY_BADGRAMMAR );
		}
		else if( (PrevWord.Type == WORDTYPE_BINARYOPERATOR ||
				  PrevWord.Type == WORDTYPE_UNARYOPERATOR ||
				  PrevWord.Type == WORDTYPE_AMBIGUOUSOPERATOR) &&
				 CurrentWord.Extra == EXTRA_PARENTHESIS_Right )
		{
			ThrowExpressionAnomaly( TXT("Operators cannot be followed by a \')\'."), ANOMALY_BADGRAMMAR );
		}
		
		
	
	}

	//RULE: The last word must be an identifier, literal, or ')'
	if( !IgnoreTrailingOps )
	{
		if( !( CurrentWord.IsLiteral() || CurrentWord.Type == WORDTYPE_IDENTIFIER ||
			CurrentWord.Extra == EXTRA_PARENTHESIS_Right ) )
		{
			ThrowExpressionAnomaly( TXT("Last word in an expression must be an identifier or literal."),
									ANOMALY_BADGRAMMAR );
		}
	}

	//RULE: All '(' must have a matching ')'
	if( ParenthesisCount != 0 )
	{
		ThrowExpressionAnomaly( TXT("Extra \'(\'. Can't find the matching \')\', or the matching \')\' is outside of the expression"),
							   ANOMALY_BADGRAMMAR );
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::StryOutlyingParenthesis
 NOTES: Does pretty much what it says. 
		Won't complain if there are no outlying parenthesis.
		Will complain if there is a parenthesis mismatch.
*/
void Expression::StripOutlyingParenthesis() const
{
	int ParenthesisCount = 0; //Gets reused later down
	if( (*this)[0].Extra == EXTRA_PARENTHESIS_Left )
	{
		unsigned int i;
		for( i = 0; i < size(); i++ )
		{
			if( (*this)[i].Extra == EXTRA_PARENTHESIS_Left ) ParenthesisCount++;
			else if( (*this)[i].Extra == EXTRA_PARENTHESIS_Right ) ParenthesisCount--;

			//Found a matching ")"
			if( ParenthesisCount == 0 ) break;
		}

		if( i == size() )//Parenthesis mismatch
		{ 
			ThrowParserAnomaly( TXT("Parenthesis mismatch."), ANOMALY_BADGRAMMAR );
		}
		else if( i == size() - 1 )//Outlying parenthesis, ok to strip
		{
			mLowerBounds++;
			mUpperBounds = GetAbsoluteIndex( size() - 1 );
		}
		//else parenthesis match, but no outlying parenthesis
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::ThrowExpressionAnomaly
 NOTES: Formats an anomaly description specifically regarding expressions.
*/
void Expression::ThrowExpressionAnomaly( const STRING& Desc, AnomalyCode Code ) const
{
	STRING FullDesc = TXT("Bad expression: \'");
	FullDesc += DumpToString();
	FullDesc += TXT("\' --- ");
	FullDesc += Desc;
	
	ThrowParserAnomaly( FullDesc, Code );	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::DumpToSTring
 NOTES: Dumps the expression to a human readable format.
*/
STRING Expression::DumpToString() const
{
	STRING Out;
	unsigned int i;
	for( i = 0; i < size(); i++ )
	{
		//TODO: This is now broken since I no longer dump everything into the
		//		string member. (i.e. "foo + bar" will end up looking like "foo  bar")

		const Word& Temp = (*this)[i];

		switch( Temp.Type )
		{
		case WORDTYPE_BINARYOPERATOR:
			Out += gBinaryOperatorReverseMap[ Temp.Extra ];
			break;

		case WORDTYPE_UNARYOPERATOR:
			Out += gUnaryOperatorReverseMap[ Temp.Extra ];
			break;

		case WORDTYPE_STRINGLITERAL:
			Out += TXT("\"");
			Out += Temp.String;
			Out += TXT("\"");
			break;

		case WORDTYPE_BOOLLITERAL:
			Out += Temp.Extra == EXTRA_BOOLLITERAL_True ? LC_True : LC_False;
			break;

		case WORDTYPE_EMPTYLISTLITERAL:
			Out += TXT("()");
			break;

		//These shouldn't every be in an expression, but I might as well...
		case WORDTYPE_BRACKET:
			Out += Temp.Extra == EXTRA_BRACKET_Left ? TXT("{") : TXT("}");
			break;

		case WORDTYPE_PARENTHESIS:
			Out += Temp.Extra == EXTRA_PARENTHESIS_Left ? TXT("(") : TXT(")");
			break;

		default:
			Out += Temp.String;
		}

		Out += TXT(" ");
	}
	
	return Out;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Expression::Precedence
 NOTES: Returns the precedence of the given operator.
*/
Expression::OperatorPrecedence Expression::Precedence( const Word& W ) const
{
		//A list of operators in order of lowest to highest precedence

	static bool FirstRun = true;
	static std::map< ExtraDesc, int > PrecedenceList;

	if( FirstRun )
	{
		PrecedenceList[EXTRA_UNOP_Import]             = 0;

		PrecedenceList[EXTRA_BINOP_Assign]            = 1;
		PrecedenceList[EXTRA_BINOP_MinusAssign]       = 2;
		PrecedenceList[EXTRA_BINOP_PlusAssign]        = 3;
		PrecedenceList[EXTRA_BINOP_DivideAssign]      = 4;
		PrecedenceList[EXTRA_BINOP_TimesAssign]       = 5;
		PrecedenceList[EXTRA_BINOP_ExponentAssign]    = 6;
		PrecedenceList[EXTRA_BINOP_LogicalAnd]        = 7;
		PrecedenceList[EXTRA_BINOP_LogicalOr]         = 8;
		PrecedenceList[EXTRA_BINOP_Equals]            = 9;
		PrecedenceList[EXTRA_BINOP_NotEquals]         = 9;
		PrecedenceList[EXTRA_BINOP_LessThanOrEqual]   = 10;
		PrecedenceList[EXTRA_BINOP_LargerThanOrEqual] = 10;
		PrecedenceList[EXTRA_BINOP_LessThan]          = 11;
		PrecedenceList[EXTRA_BINOP_LargerThan]        = 11;
		PrecedenceList[EXTRA_BINOP_ListSeperator]     = 12;
		PrecedenceList[EXTRA_BINOP_Minus]             = 13;
		PrecedenceList[EXTRA_BINOP_Plus]              = 14;
		PrecedenceList[EXTRA_BINOP_Divide]            = 15;
		PrecedenceList[EXTRA_BINOP_Times]             = 16;
		PrecedenceList[EXTRA_BINOP_Exponent]          = 17;

		PrecedenceList[EXTRA_UNOP_Negative]           = 18;
        PrecedenceList[EXTRA_UNOP_Not]                = 18;

		PrecedenceList[EXTRA_UNOP_Var]				  = 19;
		PrecedenceList[EXTRA_UNOP_List]				  = 19;
		PrecedenceList[EXTRA_UNOP_Character]		  = 19;
		PrecedenceList[EXTRA_UNOP_Player]			  = 19;		

		PrecedenceList[EXTRA_BINOP_ListAccess]        = 20; 
		PrecedenceList[EXTRA_BINOP_ListAppend]        = 20;
		PrecedenceList[EXTRA_BINOP_ListRemove]        = 20;

		//Always keep these at the highest
		PrecedenceList[EXTRA_BINOP_ScopeResolution]   = 21;
	}


	//Here we assume it is a function (an Operator or Block).
	if( W.Type != WORDTYPE_BINARYOPERATOR &&
		W.Type != WORDTYPE_UNARYOPERATOR &&
		W.Type != WORDTYPE_AMBIGUOUSOPERATOR )
	{
		return 0;
	}

	return PrecedenceList[ W.Extra ];
}
