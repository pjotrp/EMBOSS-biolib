#!/usr/local/bin/perl -w

%tests = (
	  "feat1" => "seqretallfeat -auto tembl:hsfau",
	  "featmany" => "seqretallfeat -auto 'tembl:hsf*'",
	  "set1" => "seqretset -auto tsw:opsd_human",
	  "setmany" => "seqretset -auto 'tsw:opsd_*'",
	  "embl1" => "seqret -auto tembl:hsfau",
	  "emblmany" => "seqret -auto 'tembl:hsf*'",
	  "" => "",
);

if (defined($ENV{EPURE})) {
  $purepath = "$ENV{EPURE}/";
}
else {
  $purepath = "";
}

@dotest = @ARGV;

foreach $name (@dotest) {
  print "Test $name\n";

  if (defined($tests{$name})) {
  print "Running $purepath$tests{$name}\n";
    $status = system ("$purepath$tests{$name}");
    
    if ($status) {
      print STDERR "Purify test $name returned status $status\n";
    }
    else {
      print STDERR "Purify test $name OK\n";
    }
  }
  else {
      print STDERR "ERROR: Unknown test $name \n";
  }
}

exit();
