#!/bin/sh

#This script is dumb, and should be used through a build system (ant).
#Ant however is also dump and won't execute my command correctly.

install -v libstoryscript.so /usr/lib/
mkdir -p /usr/include/storyscript
install -v include/*.hpp /usr/include/storyscript/
install -v include/*.h /usr/include/storyscript/

