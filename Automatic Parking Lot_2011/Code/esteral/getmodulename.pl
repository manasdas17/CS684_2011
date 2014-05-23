#!/usr/bin/perl
#use Switch;
#argument1----- filename
$ip=" ";  
$argcount=@ARGV;
if($argcount != 1)
{
 print "usage:";
 print "arg1 - filename \n";
 exit(0);
}
$ip=(@ARGV)[0];
$reptext=(@ARGV)[1];
$filename=$ip;
unless(-e $filename)
{
   print "file doesnot exitst";
   return;
}
open FH,"+<$filename" or die $!;
@inputfile_contents=<FH>;
$length_inputfile=@inputfile_contents;
seek FH,0,0;
foreach $file (@inputfile_contents)
{
   if($file =~ /(\s*)module(\s*)(\S*)(\s*):/ )
   {
        print $3;   
	close FH;
	exit 0;
   }
}
close FH;
