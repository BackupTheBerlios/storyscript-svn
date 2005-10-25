#!/usr/bin/perl
 
 #Generates the filelists for use with kdevelop.
 #Kdevelop should be able to do this itself, but its retarded.
 
#$ProjName = "ssi";
#$FilelistName = "$ProjName.kdevelop.filelist";

$FilelistNmae = "../ssi.kdevelop.filelist";

open FILELIST, "> $FilelistName" or die "Failed to open filelist for writing.";

print FILELIST "#Auto-generated with GenFilelist.pl\n";

#Add all the files in ../src

opendir( DIR, "../src" ) or die "Failed to open ../src directory.";
@Files = grep(/\.cpp$/, readdir(DIR) );

foreach $i (@Files){
	print "src/$i\n";
	print FILELIST "src/$i\n";
}

closedir(DIR);

#Add all the files in ../include

opendir( DIR, "../include" ) or die "Failed to open ../include directory.";
@Files = grep(/\.hpp$/, readdir(DIR) );

foreach $i (@Files){
	print "include/$i\n";
	print FILELIST "include/$i\n";
}

closedir(DIR);

close FILELIST;
