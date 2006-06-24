AUTOMAKE_OPTIONS = foreign



bin_PROGRAMS = storyscript

storyscript_SOURCES = \
ansicolor/ansicolor.cc \
Console.cpp \
ConsoleInterface.cpp \
ConsoleReaderSource.cpp \
ReadlineReaderSource.cpp \
main.cpp

storyscript_LDADD = $(top_srcdir)/interpreter/src/.libs/libstoryscript.la -lreadline -lcurses

storyscript_CPPFLAGS = -I../include -Iansicolor -I$(top_srcdir)/interpreter/include

