#!/usr/bin/perl

#For automatic code backups
#This is use to delete any unessisary file and then use 7zip to make a zipfile

#These are defaults.  Pass the proper argument to override
$PackageName = "MyProject";
$BackupDirectory = "./Backups";

#Uncomment one to select the default. Other wise pass an argument to override.

## 7-Zip (My personal favorite)
$ZipProgram = "7za";
$ZipOptions = " a -mx=9";
$ExcludeOption = "-x!";
$ZipExten = "7z";

## bzip2
#$ZipProgram = "tar";
#$ZipOptions = " -cvjf";
#$ExcludeOption = "--exclude=";
#$ZipExten = "tar.bz2";

## gzip
#$ZipProgram = "tar";
#$ZipOptions = "-cvzf";
#$ExcludeOption = "--exclude=";
#$ZipExten = "tar.gz";


#Figure out command line arguments

while( $i < scalar @ARGV )
{
	#Output Directory
	if( $ARGV[$i] eq "-d" )
	{
		$i++;
		$BackupDirectory = $ARGV[$i];	
	}
	#Compression Format
	elsif( $ARGV[$i] eq "-z" )
	{
		$i++;
		if( $ARGV[$i] eq "7zip" ){
			$ZipProgram = "7za";
			$ZipOptions = " a -mx=9";
			$ExcludeOption = "-x!";
			$ZipExten = "7z";		
		}
		elsif( $ARGV[$i] eq "gzip" ){
			$ZipProgram = "tar";
			$ZipOptions = "-cvzf";
			$ExcludeOption = "--exclude=";
			$ZipExten = "tar.gz";
		}
		elsif( $ARGV[$i] eq"bz2" ){
			$ZipProgram = "tar";
			$ZipOptions = " -cvjf";
			$ExcludeOption = "--exclude=";
			$ZipExten = "tar.bz2";		
		}
		else{
			print "Warning: $ARGV[$i] is not a proper compression format.  Staying at default.";
		}
	}
	#Package Name
	elsif( $ARGV[$i] eq "-n" )
	{
		$i++;
		$PackageName = $ARGV[$i];
	}
	#Help
	elsif( $ARGV[$i] eq "-h" or $ARGV[$i] eq "--help" )
	{
		print "\nDans Auto-Backup Script 5\n" .
			   "Usage:\n" .
			   "-h, --help       Prints this message.\n" .
			   "-z ZipFormat     Sets the compression format.\n" .
			   "                 Options are 7zip, gzip, and bz2\n" .
			   "-n PackageName   Sets the name of your project.\n" .
			   "-d OutDir        Sets the output directory\n" .
			   "-e Path/File     Excludes the file.\n";
		exit;	
	}
	#Exclusions
	elsif( $ARGV[$i] eq "-e" )
	{
		$i++;
		
		#Remove qutoes
		$ARGV[$i] =~ s/^"(.*)"$/$1/;
		
		
		
		while( $ARGV[$i] =~ /(.*)(\,|$)/g )
		{
			$1 ne "" and push( @ExcludeFiles, $1 );
		}
	}
	#Ooops
	else
	{
		print "Warning: Unrecognized argument: $ARGV[$i]";
	}
	
	$i++;
}


#These are files that should not be deleted, but
#also should not be included in the zip
push( @ExcludeFiles, $BackupDirectory );


use File::Copy;

print "\nDaniel Jones' Auto-Backup Script 6\n";
print "____________________________________________\n";


$ExcludeCommand = "";

print "Excluding Files:\n";
foreach $i (@ExcludeFiles)
{
    print "\t" . $i . "\n";
    
    $ExcludeCommand .= $ExcludeOption . $i . " ";
}

print "\n\nZipping Files...\n"; 

#Figure out the file name:
($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)
                                        = localtime(time);
$year += 1900; #Just a fix for how year is returned
$mon += 1; #Ditto

#You can set up the time stamp however you like.
$timestamp = "$year.$mon.$mday-$hour.$min.$sec";


$ZipFile = "$PackageName--$timestamp.$ZipExten";
           
$ZipFile = "\"$BackupDirectory/$ZipFile\"";

$Command = $ZipProgram . " " .
		   $ExcludeCommand . " " .
		   $ZipOptions . " " .
		   $ZipFile . " *";

system( $Command );


print "\nDone ;-)\n";
print "Backed up to: $ZipFile\n";


print "____________________________________________\n";

system("PAUSE");
