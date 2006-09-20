/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: A console abstraction.
*/


#include <vector>
#include "Unicode.hpp"
#include "DLLExportString.hpp"


#if !defined(SS_Console)
#define SS_Console


//ConsoleOutColor -- Defines different colors that can be outputted
enum ConsoleOutColor
{
	ColorBlack,
	ColorRed,
	ColorGreen,
	ColorYellow,
	ColorBlue,
	ColorMegenta,
	ColorCyan,
	ColorWhite
};



struct ColorPair{
	ColorPair( ConsoleOutColor FG, ConsoleOutColor BG )
		: FG(FG), BG(BG) {}

	bool operator==( const ColorPair& Y ){
		if( FG == Y.FG && BG == Y.BG ) return true;
		else return false;
	}


	ConsoleOutColor FG;
	ConsoleOutColor BG;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Console
 NOTES: The interface for console i/o.  Nothing fancy but it gets the job done.
*/
class Console
{
public:
	Console();
	virtual ~Console() {}

	//These ones must be defined.
	virtual Console& operator << ( const SS::String& s ) = 0;
	virtual Console& operator << ( int i ) = 0;
	virtual Console& operator << ( unsigned int ui ) = 0;
	virtual Console& operator << ( long l ) = 0;
	virtual Console& operator << ( unsigned long ul ) = 0;
	virtual Console& operator << ( float f ) = 0;
	
	virtual Console& operator >> ( SS::String& s ) = 0;
	virtual Console& operator >> ( int& i ) = 0;
	virtual Console& operator >> ( unsigned int& ui ) = 0;
	virtual Console& operator >> ( long& l ) = 0;
	virtual Console& operator >> ( unsigned long& ul ) = 0;
	virtual Console& operator >> ( float& f ) = 0;

	//All the fancy style/color functions will do nothing by default.
	virtual Console& SetBold( bool Flag ) { return *this; }
	virtual Console& SetBlink( bool Flag ) { return *this; }
	virtual Console& SetUnderline( bool Flag ) { return *this; }
	virtual Console& SetStandout( bool Flag ) { return *this; }

	virtual Console& SetTextBGColor( ConsoleOutColor Color ) { return *this; }
	virtual Console& SetTextFGColor( ConsoleOutColor Color ) { return *this; }
	virtual Console& SetTextColor( ColorPair ) { return *this; }

	virtual Console& SetBackground( ColorPair, char FillChar = 0 ) { return *this; }
	virtual Console& SetBackgroundFull( ColorPair, char FillChar = 0 ) { return *this; }
	
	void UseColor( bool Flag );
	
protected:
	bool mUseColor;
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 StdConsole
 NOTES: A simple wrapper for the std::cout and std::cin
*/
class StdConsole : public Console
{
public:
	StdConsole() : mBold(false), mUnderline(false), mCurrentColor(ColorBlack) {}
	~StdConsole() {}

	Console& operator << ( const SS::String& s );
	Console& operator << ( int i );
	Console& operator << ( unsigned int ui );
	Console& operator << ( long l );
	Console& operator << ( unsigned long ul );
	Console& operator << ( float f );
	
	Console& operator >> ( SS::String& s );
	Console& operator >> ( int& i );
	Console& operator >> ( unsigned int& ui );
	Console& operator >> ( long& l );
	Console& operator >> ( unsigned long& ul );
	Console& operator >> ( float& f );
	
	Console& SetBold( bool Flag );
	Console& SetUnderline( bool Flag );
	Console& SetTextFGColor( ConsoleOutColor Color );
	Console& SetTextColor( ColorPair );
	
private:
	void Fix();

	bool mBold;
	bool mUnderline;
	ConsoleOutColor mCurrentColor;

};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 CursesConsole
 NOTES: A C++ wrapper for the PDCurses (http://pdcurses.sourceforge.net/) library.
*/
class CursesConsole : public Console
{
public:
	CursesConsole();
	~CursesConsole();

	Console& operator << ( const SS::String& s );
	Console& operator << ( int i );
	Console& operator << ( unsigned int ui );
	Console& operator << ( long l );
	Console& operator << ( unsigned long ul );
	Console& operator << ( float f );
	
	Console& operator >> ( SS::String& s );
	Console& operator >> ( int& i );
	Console& operator >> ( unsigned int& ui );
	Console& operator >> ( long& l );
	Console& operator >> ( unsigned long& ul );
	Console& operator >> ( float& f );

	Console& SetBold( bool Flag );
	Console& SetBlink( bool Flag );
	Console& SetUnderline( bool Flag );
	Console& SetStandout( bool Flag );

	Console& SetTextBGColor( ConsoleOutColor Color );
	Console& SetTextFGColor( ConsoleOutColor Color );
	Console& SetTextColor( ColorPair );

	Console& SetBackground( ColorPair, char FillChar = 0 );
	Console& SetBackgroundFull( ColorPair, char FillChar = 0 );
	

private:
	short GetCursesColor( ConsoleOutColor );
	unsigned int GetCursesColorPair( ColorPair ); 
	
	char* mpInputBuffer;

	unsigned int mCurrentTextPair;
	unsigned int mCurrentBGPair;
	std::vector<ColorPair> mColors;
};


#endif

