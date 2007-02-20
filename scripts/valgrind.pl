#!/usr/bin/perl -w

# Runs valgrind, a Linux utility to check for memory leaks.
# Alternatives are third (3rd degree) on OSF
# or purify (purify.pl) on Irix and Solaris
#
# Valgrind is free software, with documentation online at
# http://devel-home.kde.org/~sewardj/docs/index.html
#
# Valgrind requires EMBOSS built without shared libraries

%precommands = (
		"domainalign-qa1" => "mkdir daf",
		"seqnr-qa1" => "mkdir hitsnr;mkdir hitsred",
		"sigscanlig-qa1" => "mkdir lhf;mkdir aln;mkdir results",
		"ehmmindex-qa1" => "cp ../../qa/ehmmcalibrate-ex2-keep/myhmmso .",
		"ohmmindex-qa1" => "cp ../../qa/ohmm-own-keep/myhmms .",
		"ohmmcalibrate-qa1" => "cp ../../qa/ohmmbuild-ex-keep/globin.hmm .",

		"ohmmcalibrate-qa2" => "cp ../../qa/ohmm-own-keep/myhmms .",
		"ohmmcalibrate-qa3" => "cp ../../qa/ohmm-own-keep/myhmms .",
		"intconv-qa1" => "cp ../../qa/intconv-check/stdin .",
		"fmove-qa1" => "cp ../../data/fmove.in .",
		"" => ""
		);

sub runtest ($) {
    my ($name) = @_;
    print "Run valgrind test $name\n";
    my $defbytes = 0;
    my $posbytes = 0;
    my $rembytes = 0;
    my $errcount = 0;
    my $timeout = 1800;
    my $timealarm = 0;
    my $testkeep = $dokeep;
    my $infile = "";
    if (defined($tests{$name})) {
	if (defined($testcheck{$name})) {
#	    $myvalgpath = "../../emboss/";
	    $myvalgpath = "/homes/pmr/check/bin/";
	}
	else {
	    $myvalgpath = $valgpath;
	}
	    print "Running valgrind $valgopts $myvalgpath$tests{$name}\n";

	eval {
	    $sysstat = system( "rm -rf $name");
	    $status = $sysstat >> 8;
	    if ($status) {
		$testerr = "failed to delete old directory $name, status $status\n";
		print STDERR $testerr;
	    }
	    if(-e "$name" && -d "$name") { system ("rm -rf $name");}
	    mkdir ("$name", 0777);
	    chdir $name;
	    if(defined($precommands{$name})) {
		system ("$precommands{$name}");
	    }
	    if(-e "stdin") { $infile = "< stdin" }
	    $status = 0;
	    alarm($timeout);
	    $sysstat = system ("EMBOSSRC=../.. ;export EMBOSSRC ;EMBOSS_RCHOME=N ;export EMBOSS_RCHOME ;valgrind $valgopts $myvalgpath$tests{$name} $infile 9> ../valgrind/$name.valgrind" );
	    alarm(0);
	    $status = $sysstat >> 8;
	};

	if ($@) {			# error from eval block
	    if ($@ =~ /memtest timeout/) {	# timeout signal handler
		$timealarm = 1;		# set timeout flag and continue
	    }
	    else {			# other signal - fail
		die;
	    }
	}
    
	chdir "..";
	if ($timealarm) {
	    print STDERR "Valgrind test $name timed out\n";
	    return -1;
	}

	open (TEST, "valgrind/$name.valgrind") ||
	    die "Failed to open valgrind/$name.valgrind";
	while (<TEST>) {
	    if (/definitely lost: +([\d,]+) bytes in ([\d,]+) blocks/) {
		$defbytes=$1;
		$defblocks=$2;
	    }
	    if (/possibly lost: +([\d,]+) bytes in ([\d,]+) blocks/) {
		$posbytes=$1;
		$posblocks=$2;
	    }
	    if (/still reachable: +([\d,]+) bytes in ([\d,]+) blocks/) {
		$rembytes=$1;
		$remblocks=$2;
	    }
	    if (/ERROR SUMMARY: +([\d,]+) errors from ([\d,]+) contexts/) {
		$errcount=$1;
		$errcontexts=$2;
	    }
	}
	if ($status) {
	    print STDERR "Valgrind test $name returned status $status\n";
	    $testkeep=1;
	}
	else {
	    if ($errcount){
		print STDERR "Valgrind test $name errors $errcount [$errcontexts]\n";
		$testkeep=1;
	    }
	    elsif ($defbytes){
		print STDERR "Valgrind test $name leak $defbytes [$defblocks] (possibly $posbytes [$posblocks]) bytes, still reachable $rembytes bytes [$remblocks]\n";
	    }
	    elsif ($rembytes) {
		print STDERR "Valgrind test $name OK (still reachable $rembytes bytes [$remblocks])\n";
	    }
	    else {
		print STDERR "Valgrind test $name OK (all clean)\n";
	    }
	}
	if(!$testkeep) {
	    system "rm -rf $name";
	}
	return $status;
    }
    else {
	print STDERR "ERROR: Unknown test $name \n";
	return -1;
    }

}

%tests = ();
%testcheck = ();

if (defined($ENV{EVALGRIND})) {
    $valgpath = $ENV{EVALGRIND}."/";
}
else {
    $valgpath = "";
}


$testfile = "../memtest.dat";
@dotest = ();
$blocksize=1;
$blockcount = 0;
$block = 1;
$dolist=0;
$doall = 0;
$dokeep=0;
foreach $test (@ARGV) {
    if ($test =~ /^-(.*)/) {
	$arg=$1;
	if ($arg =~ /testfile=(\S+)/) {$testfile=$1}
#    elsif ($arg =~ /logfile=(\S+)/) {$logfile=">$1"} # append to logfile
	elsif ($arg eq "list") {
	    $dolist = 1;
	}
	elsif ($arg eq "all") {$doall=1;}
	elsif ($arg eq "keep") {$dokeep=1;}
	elsif ($arg =~ /block=(\d+)/) {
	    $block=$1;
	    $i=0;
	    $blocksize=10;
	    $blockcount=0;
	}
	else {print STDERR "+++ unknown option '$arg'\n"; usage()}
    }
    else {
	$test =~ s/\/$//;
	push @dotest, $test;
    }
}

open (MEMTEST, "$testfile") || die "failed to open test file $testfile";
while (<MEMTEST>) {
    if (/^[\#]/) {next}
    if (/(\S+) += +(\S.*)/) {
	$tests{$1}="$2";
    }
    if (/(\S+) +=test= +(\S.*)/) {
	$tests{$1}="$2";
	$testcheck{$1} = 1;
    }
}
close MEMTEST;

if($doall || $dolist) {
    foreach $x (sort (keys (%tests))) {
	push @dotest, $x;
    }
}

$valgopts = "--leak-check=yes --show-reachable=yes --num-callers=15 --verbose --log-fd=9 --error-limit=no --leak-resolution=high";
## --leak-check=yes       Test for memory leaks at end
## --show-reachable=yes   Show allocated memory still reachable
## --num-callers=15       Backtrace 15 functions - use more if needed
## --verbose              
## --log-fd=9         Write to file 9 for redirect (default 2 is stderr)
## --error-limit=no       Don't stop after 300 errors
## --leak-resolution=high Report alternate backtraces

$SIG{ALRM} = sub { print STDERR "+++ timeout handler\n"; die "memtest timeout" };

$i=0;
foreach $x (@dotest) {
    if (!$i) {
	$blockcount++;
    }
    $i++;
    if ($i >= $blocksize) {$i=0}
    if ($blockcount == $block) {
	if($dolist) {
	    printf "%-15s %s\n", $x, $tests{$x};
	}
	else {
	    runtest($x);
	}
	$blockcount = 0;
    }
}

exit();
