#!/usr/local/bin/perl -w

$errfile = 0;
$errfunc = 0;
$errcount = 0;
$totcount = 0;
$totfile = 0;

%badfiles = ();

while (<>) {
    $newfunc = 0;
    $newfile = 0;
    if (/^Function (\S+)/) {
	$funcname = $1;
	$newfunc = 1;
    }
    elsif (/^Static function (\S+)/) {
	$funcname = $1;
	$newfunc = 1;
    }
    elsif (/^Macro (\S+)/) {
	$funcname = $1;
	$newfunc = 1;
    }
    elsif (/^Data type (\S+)/) {
	$funcname = $1;
	$newfunc = 1;
    }
    elsif (/^Static data type (\S+)/) {
	$funcname = $1;
	$newfunc = 1;
    }

    if (/^set pubout \'([^\']+)\' lib \'([^\']+)\'/) {
	$newfilename = $1;
	$newfilelib = $2;
	$newfile = 1;
    }

    if ($newfunc || $newfile) {
	if ($errcount) {
	    $totcount += $errcount;
	    $errfunc++;
	    $errfile++;
	}
	$errcount = 0;
    }

    if ($newfile) {
	if ($errfile) {
	    $badfiles{$filelib."_".$filename}=$errfile;
	    $totfile++;
	}
	$errfile = 0;
	$filename = $newfilename;
	$filelib = $newfilelib;
    }

    if (/^bad/) {
	$errcount++;
    }
}

if ($errcount) {
    $totcount += $errcount;
    $errfunc++;
    if (!$errfile) {
	$totfile++;
    }
    $errfile++;
}

if ($errfile) {$badfiles{$filelib."_".$filename}=$errfile}

foreach $x (sort (keys (%badfiles))) {
    printf "%4d %s\n", $badfiles{$x}, $x;
}

print STDERR "$totcount errors in $errfunc functions in $totfile files\n";
