AUTOMAKE_OPTIONS = foreign

MOSTLYCLEANFILES = SVNRevision.cpp

lib_LTLIBRARIES = libstoryscript.la

libstoryscript_la_CPPFLAGS = -I../include

libstoryscript_la_SOURCES = \
Anomaly.cpp \
Block.cpp \
Bookmark.cpp \
BuiltInFunctions.cpp \
Character.cpp \
CreationFuncs-impl.cpp \
CreationFuncs.cpp \
Expression.cpp \
HelperFuncs.cpp \
Interface.cpp \
Interpreter.cpp \
LanguageConstants.cpp \
List.cpp \
Operator.cpp \
ParserAnomaly.cpp \
ReaderSource.cpp \
ReaderSourceFile.cpp \
ReaderSourceString.cpp \
Scope.cpp \
ScopeObject.cpp \
ScopeObjectVisitor.cpp \
Slib-Common.cpp \
Slib-List.cpp \
Slib-Math.cpp \
Slib-Time.cpp \
SpecialVars.cpp \
Unicode.cpp \
Variable.cpp \
VersionInfo.cpp \
Word.cpp \
SVNRevision.cpp

SVNRevision.cpp: FORCE
	./gen-SVNRevision.cpp.sh
FORCE: