#ifndef INCLUDED_ANSICOLOR_H
#define INCLUDED_ANSICOLOR_H

#include <iostream>

// Author: Jamie Moyers
// Started: April 21 1999
//
// Simple set of io manipulators for emitting the escape codes that
// control text color.
//
// Idea and terminology borrowed from the very nice perl
// module "ANSIColor.pm" by:
//    Russ Allbery <rra@stanford.edu> and
//    Zenin <zenin@best.com>
//
// Usage:
//
//    Simply insert a manipulator for the desired attribute before
//    the text to be output:
//
//       cout << bold << yellow << "This is in bold yellow." << reset << '\n';
//
//    Setting the global variable ANSIColor::emitColor to false disables
//    all of the ANSIColor manipulators.

namespace ANSIColor 
{

// If false, the various manipulators do nothing.
extern bool emitColor;

std::ostream& reset(std::ostream& outs);
std::ostream& bold(std::ostream& outs);
std::ostream& underline(std::ostream& outs);
std::ostream& underscore(std::ostream& outs);
std::ostream& blink(std::ostream& outs);
std::ostream& reverse(std::ostream& outs);
std::ostream& concealed(std::ostream& outs);

std::ostream& black(std::ostream& outs);
std::ostream& red(std::ostream& outs);
std::ostream& green(std::ostream& outs);
std::ostream& yellow(std::ostream& outs);
std::ostream& blue(std::ostream& outs);
std::ostream& magenta(std::ostream& outs);
std::ostream& cyan(std::ostream& outs);
std::ostream& white(std::ostream& outs);

std::ostream& on_black(std::ostream& outs);
std::ostream& on_red(std::ostream& outs);
std::ostream& on_green(std::ostream& outs);
std::ostream& on_yellow(std::ostream& outs);
std::ostream& on_blue(std::ostream& outs);
std::ostream& on_magenta(std::ostream& outs);
std::ostream& on_cyan(std::ostream& outs);
std::ostream& on_white(std::ostream& outs);

};


#endif  // INCLUDED_ANSICOLOR_H
