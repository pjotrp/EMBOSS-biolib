#!/usr/local/bin/perl -w

sub runtest ($) {
  my ($name) = @_;
  print "Test $name\n";
  my $defbytes = 0;
  my $posbytes = 0;
  my $rembytes = 0;
  if (defined($tests{$name})) {
    print "Running valgrind $valgopts $valgpath$tests{$name}\n";
    $status = system ("valgrind $valgopts $valgpath$tests{$name} 9> valgrind/$name.valgrind;echo '' >! $valgopts $valgpath$tests{$name}" );
    open (TEST, "valgrind/$name.valgrind") ||
	die "Failed to open valgrind/$name.valgrind";
    while (<TEST>) {
	if (/definitely lost: +(\d+) bytes in (\d+) blocks/) {
	    $defbytes=$1;
	    #$defblock=$2;
	}
	if (/possibly lost: +(\d+) bytes in (\d+) blocks/) {
	    $posbytes=$1;
	    #$posblock=$2;
	}
	if (/still reachable: +(\d+) bytes in (\d+) blocks/) {
	    $rembytes=$1;
	    #$remblock=$2;
	}
    }
    if ($status) {
	print STDERR "Valgrind test $name returned status $status\n";
    }
    else {
	if ($defbytes){
	    print STDERR "Valgrind test $name leak $defbytes (possibly $posbytes) bytes, still reachable $rembytes bytes\n";
	}
	elsif (defined($rembytes)) {
	    print STDERR "Valgrind test $name OK (still reachable $rembytes bytes)\n";
	}
	else {
	    print STDERR "Valgrind test $name OK (all clean)\n";
	}
    }
  return $status;
  }
  else {
      print STDERR "ERROR: Unknown test $name \n";
      return -1;
  }

}

%tests = ();

open (MEMTEST, "../memtest.dat");
while (<MEMTEST>) {
    if (/(\S+) += +(\S.*)/) {
	$tests{$1}=$2;
    }
}
close MEMTEST;

if (defined($ENV{EVALGRIND})) {
  $valgpath = "$ENV{EVALGRIND}/";
}
else {
  $valgpath = "";
}

$valgopts = "--leak-check=yes --show-reachable=yes --num-callers=15 --verbose --logfile-fd=9";
#$valgopts = "--leak-check=yes --show-reachable=yes";
@dotest = @ARGV;

foreach $name (@dotest) {
  if ($name =~ /^-(\S+)$/) {
    $arg = $1;
    if ($arg eq "all") {
      foreach $x (sort (keys (%tests))) {
	runtest($x);
      }
      exit;
    }
    elsif ($arg eq "list") {
      foreach $x (sort (keys (%tests))) {
	printf "%-15s %s\n", $x, $tests{$x};
      }
      exit;
    }
    elsif ($arg =~ /block=(\d+)/) {
      $block=$1;
      $i=0;
      $blocksize=10;
      $blockcount=0;
      foreach $x (sort (keys (%tests))) {
	if (!$i) {
	  $blockcount++;
	}
	$i++;
	if ($i >= $blocksize) {$i=0}
	if ($blockcount == $block) {
	  runtest($x);
	}
      }
      exit;
    }
    else {
      print STDERR "Invalid argument $name (ignored)\n";
      next;
    }
  }
  runtest ($name);
}


exit();
