#!/usr/local/bin/perl -w

##########################
# THINGS TO DO
##########################
# Timeout (TI line to set) default 60secs, diffseq needs more
# Preprocess (PP line) command
# Postprocess (QQ line) command - e.g. testing database builds, reusing output
#
##########################
# THINGS TO DO
##########################
#
# EMBOSS_RC variable to read and extra .embossrc file (for new test dbs)
#

# EMBOSS QA Test Processing
#
# Test line types:
# ID Test name (for the test directory)
# AP Application name (for the command line and for statistics)
# CL Command line (rest of the command line)
# ER error return code
# ## Comment (required double #)
# CC Comment
# IN Line(s) of standard input
# FI File name (stdout and stderr assumed to exist and be empty unless stated)
# FP File pattern - /regexp/ to be found. Optional count to check exact number.
# FZ [<=>]number File size test. Implicit test for zero size stdout/stderr unless stated
# FC [<=>]number File linecount test
# // End of test entry
# 
# Return codes: see %retcode definition
#

$id = "";
$lastid = "";
$testdef = "";
$tcount=0;
$tfail=0;

$timeoutdef=60;			# default timeout in seconds

$SIG{ALRM} = sub { die "timeout" };

%retcode = (
	    "1" => "Bad definition line ",
	    "2" => "Failed to run",
	    "3" => "Unknown file",
	    "4" => "Failed pattern",
	    "5" => "Unexpected success",
	    "6" => "Not empty file",
	    "7" => "Failed size",
	    "8" => "Failed linecount",
	    "9" => "Failed unwanted pattern",
	    "10" => "Failed counted pattern",
	    "11" => "Timeout"
);

open (LOG, ">qatest.log") || die "Cannot open qatest.log";
open (IN, "../qatest.dat") || die "Cannot open qatest.dat";

while (<IN>) {
  if (/^ID\s+(\S+)/) {
    $lastid = $id;
    $id = $1;
    $testdef = "";
  }
  $testdef .= $_;

  if (/^\/\//) {
    $tcount++;
    $result = runtest ($testdef);
    if ($result) {
      print STDERR "$id test failed code $result\n\n";
      print LOG "$id test failed code $result\n";
      $tfail++;
    }
    else {print LOG "test $id success\n"}
    print LOG "\n";
    undef %outfile;
    undef %outfilepatt;
    undef %pattest;
    undef %patcount;
    undef %patcode;
    undef %outcount;
    undef %outsize;

  }
}

$tpass = $tcount - $tfail;

print STDERR "Tests total: $tcount pass: $tpass fail: $tfail\n";

exit;

sub runtest ($) {

  my ($testdef) = @_;
  # print $testdef;
  my $ifile = 0;
  my $ipatt = 0;
  my $ret = 0;
  my $odata = "";
  my $ip = $iq = 0;
  my $i = $j = $k = 0;
  my $testerr = "";
  my $cmdline = "";
  my $testret = 0;
  my $testid = "";
  my $testin = "";
  my $timeout = $timeoutdef;
  my $ppcmd = "";
  my $qqcmd = "";

  foreach $line  (split (/^/, $testdef)) {
    ###print "<$line>\n";
    if ($line =~ /^ID\s+(\S+)/) {
      $testid = $1;
      $testin = "";
      $cmdline = "";
      $ifile=0;
      print LOG "Test <$testid>\n";
      $sysstat = system( "rm -rf $testid");
      $status = $sysstat >> 8;
      if ($status) {
	$testerr = "failed to delete old directory $1, status $status\n";
	print STDERR $testerr;
      }
      mkdir ("$testid", 0777);
      open (SAVEDEF, ">$1/testdef") || die "Cannot save $1/testdef";
      print SAVEDEF $testdef;
      close (SAVEDEF);
    }
    elsif ($line =~ /^\#\#/) {next}
    elsif ($line =~ /^CC/) {next}
    elsif ($line =~ /^TI\s+(\d+)/) {$timeout = $1}
    elsif ($line =~ /^ER\s+(\d+)/) {$testret = $1}
    elsif ($line =~ /^AP\s+(\S+)/) {$testapp = $1}
    elsif ($line =~ /^PP\s+(\S+)/) {$ppcmd .= "$1;"}
    elsif ($line =~ /^QQ\s+(\S+)/) {$qqcmd .= "$1;"}
    elsif ($line =~ /^IN\s+(\S*)/) {$testin .= "$1\n"}
    elsif ($line =~ /^CL\s+(.*)/) {
      if ($cmdline ne "") {$cmdline .= " "}
      $cmdline .= $1;
    }
    elsif ($line =~ /^FI\s+(\S+)/) {
      $outfile{$1} = $ifile;
      $outfilepatt{$ifile} = $ipatt;
      print LOG "Known file [$ifile] <$1>\n";
      $ifile++;
    }
    elsif ($line =~ /^FP\s+((\d+)\s+)?(([si]+)\s+)?\/(.*)\/$/) {
      if (defined($2)) {$patcount{$ipatt}=$2}
      if (defined($4)) {$patcode{$ipatt}=$4}
      $pat = $5;
      $pattest{$ipatt} = $pat;
      $ipatt++;
    }
    elsif ($line =~ /^FC\s+([<>=]\s*\d+)/) {
      $outcount{$ifile-1} = $1;
    }
    elsif ($line =~ /^FZ\s+([<>=]\s*\d+)/) {
      $outsize{$ifile-1} = $1;
    }
    elsif ($line =~ /^\/\/$/) {next}
    else {
      $testerr = "$retcode{1} $line: $line";
      print STDERR $testerr;
      return 1;
    }
  }
  chdir $testid;
  if ($testin ne "") {
    open (TESTIN, ">stdin");
    print TESTIN $testin;
    close TESTIN;
    $stdin = "< stdin";
  }
  else {$stdin = ""}
  $timealarm=0;
  $starttime = time();

  eval {
    alarm($timeout);
    $sysstat = system( "$ppcmd $testapp $cmdline > stdout 2> stderr $stdin $qqcmd");
    alarm(0);
    $status = $sysstat >> 8;
  };
  if ($@) {
    if ($@ =~ /timeout/) {
      $timealarm = 1;
    }
    else {
      die;
    }
  }

  if ($timealarm) {
    $testerr = "$retcode{11} ($timeout secs) '$testapp $cmdline $stdin', status $status/$testret\n";
    print STDERR $testerr;
    chdir ("..");
    return 11;
  }
  elsif ($status) {
    if ($status != $testret) {
      $testerr = "$retcode{2} '$testapp $cmdline $stdin', status $status/$testret\n";
      print STDERR $testerr;
      chdir ("..");
      return 2;
    }
  }
  else {
    if ($testret) {
      $testerr = "$retcode{5} '$testapp $cmdline $stdin', status $status/$testret\n";
      print STDERR $testerr;
      chdir ("..");
      return 5;
    }
  }

  $endtime = time();
  $runtime = $endtime - $starttime;

  opendir (DIR, ".");
  @allfiles = readdir(DIR);
  closedir DIR;
  foreach $file (@allfiles) {
    if ($file eq ".") {next}
    if ($file eq "..") {next}
    if ($file eq "stdin") {next if $testin ne ""}
    if ($file eq "testdef") {next}
    if ($file eq "stdout" || $file eq "stderr") {
      if (!defined($outfile{$file})){
	$size = -s $file;
	print LOG "Test empty file $testid/$file\n";
	if (testnum("=0", $size)) {
	  $testerr = "$retcode{6} $testid/$file\n";
	  print STDERR $testerr;
	  chdir ("..");
	  return 6;
	}
	next;
      }
    }
    if (defined($outfile{$file})) {
      $i =  $outfile{$file};
      print LOG "file [$i] <$file>\n";

      if (defined($outsize{$i})) {
	$size = -s $file;
	print LOG "Test size $size '$outsize{$i}' $testid/$file\n";
	if (testnum($outsize{$i}, $size)) {
	  $testerr = "$retcode{7} $size '$outsize{$i}' $testid/$file\n";
	  print STDERR $testerr;
	  chdir ("..");
	  return 7;
	}
      }
      open (OFIL, $file) || die "Cannot open $testid/$file";
      $odata = "";
      $linecount=0;
      while (<OFIL>) {$odata .= $_; $linecount ++;}
      close OFIL;
      if (defined($outcount{$i})) {
	print LOG "Test linecount $linecount '$outcount{$i}' $testid/$file\n";
	if (testnum($outcount{$i}, $linecount)) {
	  $testerr = "$retcode{8} $linecount '$outcount{$i}' $testid/$file\n";
	  print STDERR $testerr;
	  chdir ("..");
	  return 8;
	}
      }
      $ip = $outfilepatt{$i};
      $j = $i + 1;
      if ($j >= $ifile) {$iq = $ipatt}
      else {$iq = $outfilepatt{$j}}
      ###print LOG "Patterns $ip .. ", $iq-1, "\n";
      for ($k=$ip; $k < $iq; $k++) {
	if (defined($patcode{$k})) {
	  ##print STDERR "special /m$patcode{$k} pattern '$pattest{$k}'\n";
	  if ($patcode{$k} eq "s") {$qpat = qr/$pattest{$k}/ms}
	  elsif ($patcode{$k} eq "i") {$qpat = qr/$pattest{$k}/mi}
	  elsif ($patcode{$k} eq "is") {$qpat = qr/$pattest{$k}/mis}
	  elsif ($patcode{$k} eq "si") {$qpat = qr/$pattest{$k}/mis}
	}
	else {
	  ##print STDERR "standard /m pattern '$pattest{$k}'\n";
	  $qpat = qr/$pattest{$k}/m;
	}
	if (defined($patcount{$k})) {
	  $pcount = $patcount{$k};
	  ##print STDERR "Test pattern [pat $k] '$pattest{$k}' ($pcount times) $testid/$file\n";
	  print LOG "Test pattern [pat $k] '$pattest{$k}' ($pcount times) $testid/$file\n";
	  $pc = 0;
	  while ($odata =~ /$qpat/g) {
	    $pc++;
	  }
	  if ($pc && !$pcount) {
	    print LOG "$retcode{9} [pat $k] '$pattest{$k}' $testid/$file\n";
	    $testerr = "$retcode{9} [pat $k] '$pattest{$k}' $testid/$file\n";
	    print STDERR $testerr;
	    chdir ("..");
	    return 9;
	  }
	  elsif ($pc != $pcount) {
	    print LOG "$retcode{10} [pat $k] '$pattest{$k}' found $pc/$pcount times $testid/$file\n";
	    $testerr = "$retcode{10} [pat $k] '$pattest{$k}' found $pc/$pcount times $testid/$file\n";
	    print STDERR $testerr;
	    chdir ("..");
	    return 10;
	  }
	}
	else {
	  print LOG "Test pattern [pat $k] '$pattest{$k}' $testid/$file\n";
	  ###print STDERR "Test pattern [pat $k] '$pattest{$k}' $testid/$file\n";
	  if ($odata !~ $qpat) {
	    print LOG "$retcode{4} [pat $k] '$pattest{$k}' $testid/$file\n";
	    print LOG "\$odata: '\n$odata'\n";
	    $testerr = "$retcode{4} [pat $k] '$pattest{$k}' $testid/$file\n";
	    print STDERR $testerr;
	    chdir ("..");
	    return 4;
	  }
	}
      }
    }
    else {
      $testerr = "$retcode{3} <$testid/$file>\n";
      print STDERR $testerr;
      chdir ("..");
      return 3;
    }
  }

  open (TESTLOG, ">testlog") || die "Cannot open $testid/testlog";
  print TESTLOG $testerr;
  print TESTLOG "Runtime $runtime seconds\n";
  close TESTLOG;
  chdir "..";

  return $ret;
}

sub testnum ($$) {
  my ($test, $val) = @_;
  my ($oper, $num) = ($test =~ /([<>=])\s*(\d+)/);
  if ($oper eq "=") {
    if ($val == $num) {return 0}
  }
  elsif ($oper eq ">") {
    if ($val > $num) {return 0}
  }
  elsif ($oper eq "<") {
    if ($val < $num) {return 0}
  }

  return 1;
}
