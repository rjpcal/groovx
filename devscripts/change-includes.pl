#!/usr/bin/perl

# $Id$

use File::Basename;
use File::Path;

$oldinclude=$ARGV[0]; shift @ARGV;
$newinclude=$ARGV[0]; shift @ARGV;

$nchanged=0;

foreach $fname (@ARGV) {

    $fnewname = "./.devscripts-tmp/${fname}.new";
    $fbkpname = "./.devscripts-tmp/${fname}.incbkp";

    $tdir = dirname($fnewname);
    if (! -d $tdir) {
	if (mkpath($tdir) <= 0) {
	    die "Couldn't mkdir $tdir\n";
	}
    }

    $tdir = dirname($fbkpname);
    if (! -d $tdir) {
	if (mkpath($tdir) <= 0) {
	    die "Couldn't mkdir $tdir\n";
	}
    }

    open(SRCFILE, $fname) or die "Can't open $fname\n";
    open(NEWSRCFILE, ">$fnewname") or die "Can't open $fnewname\n";

    $count = 0;

    while ($line = <SRCFILE>) {
	$count += ($line =~ s/^\#( *)include "$oldinclude"/\#\1include "$newinclude"/);
	print NEWSRCFILE $line;
    }

    if ($count > 0) {
	print "$count change(s) in $fname ($oldinclude to $newinclude)\n";
	rename "$fname", "$fbkpname" or die "Couldn't rename $fname\n";
	rename "$fnewname", "$fname" or die "Couldn't rename $fnewname\n";
	++$nchanged;
    }
    else {
	unlink "$fnewname" or die "Couldn't unlink $fnewname\n";
    }

    close NEWSRCFILE;
    close SRCFILE;
}

print "$nchanged file(s) changed ($oldinclude to $newinclude)\n";

exit
