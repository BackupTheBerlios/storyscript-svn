/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: A console abstraction.
*/


#include "Console.hpp"

//Used by CursesConsole
#include <boost/lexical_cast.hpp>
#include <curses.h>

//Used by StdConsole
#include <iostream>
#include "../ansicolor/ansicolor.h" //Thank you very much Jamie Moyers



//Feel free to tweak this if you want a smaller or larger input buffer.
const int InputBufferSize = 256;





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Conosle constructor
*/
Console::Console() : mUseColor(true)
{}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Sets the mUseColor flag.  If its on color will be used, otherwise it will
 		just use basic output.
*/
void Console::UseColor( bool Flag )
{
	mUseColor = Flag;	
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Console::Console
 NOTES: Constructor
*/
CursesConsole::CursesConsole()
{
	mColors.push_back( ColorPair( ColorWhite, ColorBlack ) );
	mCurrentTextPair = 0;
	mCurrentBGPair = 0;
	
	mpInputBuffer = new char[InputBufferSize];

	initscr();
	if(has_colors()) start_color();

	scrollok( stdscr, true );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::~Console
 NOTES: Destructor
*/
CursesConsole::~CursesConsole()
{
	endwin();
	delete[] mpInputBuffer;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~~
CursesConsole::operator<<
NOTES: The standard output operator.  Returns a reference to itself.
*/
Console& CursesConsole::operator<<( const SS::STRING& s )
{
	//TODO: Wide character compatibility
	for( unsigned int i = 0; i < s.length(); ++i ){
		addch( s[i] );
	}

	refresh();
	
	return *this;
}

Console& CursesConsole::operator<<( int i )
{
	return *this << boost::lexical_cast<SS::STRING>( i );
}

Console& CursesConsole::operator<<( unsigned int ui )
{
	return *this << boost::lexical_cast<SS::STRING>( ui );
}

Console& CursesConsole::operator<<( long l )
{
	return *this << boost::lexical_cast<SS::STRING>( l );
}

Console& CursesConsole::operator<<( unsigned long ul )
{
	return *this << boost::lexical_cast<SS::STRING>( ul );
}

Console& CursesConsole::operator<<( float f )
{
	return *this << boost::lexical_cast<SS::STRING>( f );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::operator>>
 NOTES: Standard input operator.  Asks the user for input and write it to the string.
*/
Console& CursesConsole::operator>>( SS::STRING& s )
{
	echo();
	getnstr( mpInputBuffer, InputBufferSize );

	s = SS::NormalizeString(mpInputBuffer);
	return *this;
}

Console& CursesConsole::operator>>( int& i )
{
	SS::STRING Input;
	*this >> Input;

	i = boost::lexical_cast<int>( Input );

	return *this;
}


Console& CursesConsole::operator>>( unsigned int& ui )
{
	SS::STRING Input;
	*this >> Input;

	ui = boost::lexical_cast<unsigned int>( Input );

	return *this;
}


Console& CursesConsole::operator>>( long& l )
{
	SS::STRING Input;
	*this >> Input;

	l = boost::lexical_cast<long>( Input );

	return *this;
}


Console& CursesConsole::operator>>( unsigned long& ul )
{
	SS::STRING Input;
	*this >> Input;

	ul = boost::lexical_cast<unsigned long>( Input );

	return *this;
}


Console& CursesConsole::operator>>( float& f )
{
	SS::STRING Input;
	*this >> Input;

	f = boost::lexical_cast<float>( Input );

	return *this;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::Set____
 NOTES: Sets various attributes.  Returns a ref to itself.
		You should note that these really don't work on windows.  They change
		the color, but do not really apply the effect that it claims.
*/
Console& CursesConsole::SetBold( bool Flag )
{
	if( !mUseColor ) return *this;
	Flag ? attron( A_BOLD ) : attroff( A_BOLD );
	return *this;
}


Console& CursesConsole::SetBlink( bool Flag )
{
	if( !mUseColor ) return *this;
	Flag ? attron( A_BLINK ) :
		attroff( A_BLINK );
	return *this;
} 


Console& CursesConsole::SetUnderline( bool Flag )
{
	if( !mUseColor ) return *this;
	Flag ? attron( A_UNDERLINE ) :
		attroff( A_UNDERLINE );
	return *this;
}

Console& CursesConsole::SetStandout( bool Flag )
{
	if( !mUseColor ) return *this;
	Flag ? attron( A_STANDOUT ) :
		attroff( A_STANDOUT );
	return *this;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::SetText__Color
 NOTES: Color setting functions...  Returns a ref to itself.
*/
Console& CursesConsole::SetTextBGColor( ConsoleOutColor Color )
{
	if( !mUseColor ) return *this;
	return SetTextColor( ColorPair( mColors[mCurrentTextPair].FG, Color ) );
}

Console& CursesConsole::SetTextFGColor( ConsoleOutColor Color )
{
	if( !mUseColor ) return *this;
	return SetTextColor( ColorPair( Color, mColors[mCurrentTextPair].BG ) );
}

Console& CursesConsole::SetTextColor( ColorPair Color )
{
	if( !mUseColor ) return *this;
	mCurrentTextPair = GetCursesColorPair( Color );
    attrset( COLOR_PAIR( mCurrentTextPair ) );

	return *this;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::SetBackground
 NOTES: Sets the background colors and a fill character.  Unlike SetBackgroundFull,
		these settings are only applied to the current line.
*/
Console& CursesConsole::SetBackground( ColorPair Color, char FillChar )
{
	if( !mUseColor ) return *this;
	mCurrentBGPair = GetCursesColorPair( Color );
	bkgdset( COLOR_PAIR( mCurrentBGPair ) | (short)FillChar );

	return *this;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::SetBackgroundFull
 NOTES: Similar to SetBackground except it fill the entire screen, not just the
		current line.
*/
Console& CursesConsole::SetBackgroundFull( ColorPair Color, char FillChar )
{
	if( !mUseColor ) return *this;
	mCurrentBGPair = GetCursesColorPair( Color );
	bkgd( COLOR_PAIR( mCurrentBGPair ) | (short)FillChar );

	return *this;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::GetColorColor
 NOTES: Given a member of ConsoleOutputColor, it will return a 
		color for PDCurses.  Used internally.
*/
short CursesConsole::GetCursesColor( ConsoleOutColor Color )
{
	switch( Color )
	{
	case ColorBlack   : return COLOR_BLACK;
	case ColorRed     : return COLOR_RED;
	case ColorGreen   : return COLOR_GREEN;
	case ColorYellow  : return COLOR_YELLOW;
	case ColorBlue    : return COLOR_BLUE;
	case ColorMegenta : return COLOR_MAGENTA;
	case ColorCyan    : return COLOR_CYAN;
	case ColorWhite   : return COLOR_WHITE;
	default           : return COLOR_BLACK;
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 CursesConsole::GetCursesColorPair
 NOTES: Returns an index for a Curses color pair.
*/
unsigned int CursesConsole::GetCursesColorPair( ColorPair Pair )
{
	static unsigned int NextSet = 0;

	unsigned int i;
	for( i = 0; i < mColors.size(); i++ )
	{
		if( mColors[i] == Pair ) return i;
	}

	//No matches, so set a new pair
	if( i <= (unsigned int)COLOR_PAIRS )
	{
		mColors.push_back( Pair );
		init_pair( i, GetCursesColor( Pair.FG ), GetCursesColor( Pair.BG ) );

		return i;
	}
	//Used up all the places.  Start reusing ones at the begining
	else
	{
		mColors[NextSet] = Pair;
		init_pair( NextSet, GetCursesColor( Pair.FG ), GetCursesColor( Pair.BG ) );

		unsigned int ReturnVal = NextSet;

		if( NextSet + 1 <= (unsigned int)COLOR_PAIRS ) NextSet++;
		else NextSet = 0;

		return ReturnVal;
	}
}



//----------For use with StdConsole----------------------------
#if defined(USING_UNICODE)
	#define STD_COUT std::wcout
	#define STD_CIN  std::wcin
#else
	#define STD_COUT std::cout
	#define STD_CIN  std::cin
#endif


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 StdConsole::operator<<
 NOTES: Standard output.
*/
Console& StdConsole::operator<<( const SS::STRING& s ){
	STD_COUT << s;
	return *this;
}

Console& StdConsole::operator<<( int i ){
	STD_COUT << i;
	return *this;
}

Console& StdConsole::operator<<( unsigned int ui ){
	STD_COUT << ui;
	return *this;
}

Console& StdConsole::operator<<( long l ){
	STD_COUT << l;
	return *this;
}

Console& StdConsole::operator<<( unsigned long ul ){
	STD_COUT << ul;
	return *this;
}

Console& StdConsole::operator<<( float f ){
	STD_COUT << f;
	return *this;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 StdConsole::operator>>
 NOTES: Standard input.
*/
Console& StdConsole::operator>>( SS::STRING& s ){
	SS::CHAR StringBuf[256];
	STD_CIN.getline( StringBuf, 256 );
	s = StringBuf;
	return *this;
}

Console& StdConsole::operator>>( int& i ){
	STD_CIN >> i;
	return *this;
}

Console& StdConsole::operator>>( unsigned int& ui ){
	STD_CIN >> ui;
	return *this;
}

Console& StdConsole::operator>>( long& l ){
	STD_CIN >> l;
	return *this;
}

Console& StdConsole::operator>>( unsigned long& ul ){
	STD_CIN >> ul;
	return *this;
}

Console& StdConsole::operator>>( float& f ){
	STD_CIN >> f;
	return *this;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Change to color!
*/
Console& StdConsole::SetBold( bool Flag )
{
	if( !mUseColor ) return *this;
	if( Flag )
	{
		STD_COUT << ANSIColor::bold;
		mBold = true;
	}
	else
	{
		STD_COUT << ANSIColor::reset;
		mBold = false;
		Fix();
	}
	
	return * this;	
}

Console& StdConsole::SetUnderline( bool Flag )
{
	if( !mUseColor ) return *this;
	if( Flag )
	{
		STD_COUT << ANSIColor::underline;
		mUnderline = true;
	}
	else
	{
		STD_COUT << ANSIColor::reset;
		mUnderline = false;
		Fix();
	}
	
	return * this;	
}

Console& StdConsole::SetTextFGColor( ConsoleOutColor Color )
{
	if( !mUseColor ) return *this;
	switch( Color )
	{
	case ColorBlack:
		STD_COUT << ANSIColor::black;
		break;
	case ColorRed:
		STD_COUT << ANSIColor::red;
		break;
	case ColorGreen:
		STD_COUT << ANSIColor::green;
		break;
	case ColorYellow:
		STD_COUT << ANSIColor::yellow;
		break;
	case ColorBlue:
		STD_COUT << ANSIColor::blue;
		break;
	case ColorMegenta:
		STD_COUT << ANSIColor::magenta;
		break;
	case ColorCyan:
		STD_COUT << ANSIColor::cyan;
		break;
	case ColorWhite:
		STD_COUT << ANSIColor::white;
		break;		
	}
	
	mCurrentColor = Color;	
	return *this;
}

Console& StdConsole::SetTextColor( ColorPair CP )
{
	if( !mUseColor ) return *this;
	return SetTextFGColor( CP.FG );	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This gets called to turn on setting that get unset by reset but should
 		stay set.  Or something like that.
*/
void StdConsole::Fix()
{
	if( mBold ) STD_COUT << ANSIColor::bold;
	if( mUnderline ) STD_COUT << ANSIColor::underline;
	
	SetTextFGColor( mCurrentColor );
}


