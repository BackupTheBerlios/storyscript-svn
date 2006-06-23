
#include "ansicolor.h"

//The unkown escape sequence warning
#if defined(_MSC_VER)
#pragma warning (disable : 4129)
#endif

using namespace ANSIColor;
using namespace std;

namespace ANSIColor {
bool emitColor = true;


ostream&
reset(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[0m";
   return outs;
}

ostream&
bold(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[1m";
   return outs;
}

ostream&
underline(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[4m";
   return outs;
}

ostream&
underscore(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[4m";
   return outs;
}

ostream&
blink(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[5m";
   return outs;
}

ostream&
reverse(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[7m";
   return outs;
}

ostream&
concealed(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[8m";
   return outs;
}

ostream&
black(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[30m";
   return outs;
}

ostream&
red(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[31m";
   return outs;
}

ostream&
green(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[32m";
   return outs;
}

ostream&
yellow(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[33m";
   return outs;
}

ostream&
blue(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[34m";
   return outs;
}

ostream&
magenta(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[35m";
   return outs;
}

ostream&
cyan(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[36m";
   return outs;
}

ostream&
white(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[37m";
   return outs;
}

ostream&
on_black(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[40m";
   return outs;
}

ostream&
on_red(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[41m";
   return outs;
}

ostream&
on_green(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[42m";
   return outs;
}

ostream&
on_yellow(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[43m";
   return outs;
}

ostream&
on_blue(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[44m";
   return outs;
}

ostream&
on_magenta(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[45m";
   return outs;
}

ostream&
on_cyan(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[46m";
   return outs;
}

ostream&
on_white(ostream& outs)
{
   if (ANSIColor::emitColor)
      outs << "\e[47m";
   return outs;
}

};  // namespace ANSIColor
