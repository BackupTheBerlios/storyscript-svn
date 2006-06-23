#!/usr/bin/perl

system( "scons --clean" );
system( "make clean" );
system( "rm bin/*" );
system( "rm win32/Release/*" );
system( "rm win32/Debug/*" );
system( "rm \"win32/Release (Static)\"/*" );
system( "rm \"win32/Debug (Static)\"/*" );

