 
.SILENT:
 
BASEDIR = ..
LIBDIR = ../../ssi
SRCDIR = $(BASEDIR)/src

#GMPDIR = $(BASEDIR)/../gmp

VPATH = $(SRCDIR)

INCLUDES = -I$(LIBDIR)/include -I$(BASEDIR)/src -I$(BASEDIR)/include

CPP = g++

SSILIB = $(LIBDIR)/bin/SSI.a
#GMPLIB = $(GMPDIR)/lib/libgmp.a $(GMPDIR)/lib/libgmpxx.a
#GMPLIB = -lgmp -lgmpxx
GMPLIB = -lgmp -lmpfr

SSIPROG = SSIConsole

ifdef DEBUG
CPPFLAGS = -g $(INCLUDES)
else
CPPFLAGS = -O2 $(INCLUDES)
endif

.cpp.o:
	echo $<
	$(CPP) $(CPPFLAGS) -c $<
	
SRC = \
	Console.cpp \
	ConsoleInterface.cpp \
	main.cpp
	
OBJ=$(SRC:.cpp=.o)

all: pantson $(SRC) $(OBJ) $(SSIPROG)

$(SSIPROG): $(OBJ) $(SSILIB)
	echo Creating executable $(SSIPROG)
	$(CPP) $(CPPFLAGS) -lcurses -o $@ $(OBJ) $(SSILIB) $(GMPLIB)
	cp $(SSIPROG) ../bin/$(SSIPROG)
	echo All done. Have a nice day!
	
clean:
	rm -f *.o $(PROG)
	echo "    Mr. Clean gets rid of dirt and grime and grease in just a minute,"
	echo "    Mr. Clean will clean your whole house and everything in it."
	echo

install:
	cp ../bin/$(SSIPROG) /usr/local/bin/ssi
	
backup:
	cd ..;perl MakeBackup.pl
	
pantson:
	echo Using flags "$(CPPFLAGS)"
	echo Compiling, keep your pants on...
