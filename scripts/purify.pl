#!/usr/local/bin/perl -w

%tests = (
	  "feat1" => "seqretallfeat -auto tembl:hsfau",
	  "featmany" => "seqretallfeat -auto 'tembl:hsf*'",
	  "set1" => "seqretset -auto tsw:opsd_human",
	  "setmany" => "seqretset -auto 'tsw:opsd_*'",
	  "embl1" => "seqret -auto tembl:hsfau",
	  "emblmany" => "seqret -auto 'tembl:hsf*'",
	  "" => "",
	  "wossname" => "wossname -search codon -auto",
	  "wordmatch" => "wordmatch tembl:hsfau tembl:hsfau1 -auto",
          "wordcount" => "wordcount tembl:hsfau -auto",
	  "wobble" => "wobble tembl:hsfau -auto",
	  "water" => "water tsw:opsd_human tsw:opsd_xenla -auto",
	  "vectorstrip" => "vectorstrip tembl:hsfau -vectors $ENV{EPURE}/../test/data/data.vectorstrip -vectorfile -auto",
	  "trimseq" => "trimseq tembl:hsfau -window 20 -percent 10 -auto",
	  "transeq" => "transeq -frame r tembl:hsfau -auto",
	  "tmap" => "tmap tsw:opsd_human -auto",
	  "tfm" => "tfm tfm -nomore",
	  "textsearch" => "textsearch 'tsw:*' opsin -auto",
	  "syco" => "syco -plot tembl:hsfau -cfile Echicken.cut -auto",
	  "supermatcher" => "supermatcher tembl:hsfau tembl:hsfau1 -auto",
	  "stretcher" => "stretcher tembl:hsfau tembl:hsfau1 -auto",
	  "splitter" => "splitter -size 500 tembl:hsfau -auto",
	  "silent" => "silent tembl:hsfau -sbeg 31 -send 50 -auto",
	  "sigcleave" => "sigcleave tsw:opsd_human -auto",
	  "shuffleseq" => "shuffleseq tembl:hsfau -auto",
	  "showseq" => "showseq tembl:hsfau -auto",
	  "showorf" => "showorf tembl:hsfau -auto",
	  "showfeat" => "showfeat tembl:hsfau -auto",
	  "showdb" => "showdb",
	  "showalign" => "showalign $ENV{EPURE}/../test/data/globins.msf -auto",
	  "seqtofeat" => "seqtofeat tsw:opsd_human -pattern VIA -auto",
	  "seqretsplit" => "seqretsplit 'tembl:hsfau*' -auto",
	  "seqretset" => "seqretset $ENV{EPURE}/../test/data/globins.msf -auto",
	  "seqretfeat" => "seqretfeat tembl:hsfau -auto",
	  "seqretallfeat" => "seqretallfeat 'tembl:hsfau*' -auto",
	  "seqretall" => "seqretall 'tembl:hsfau*' -auto",
	  "seqret" => "seqret tsw:opsd_human -auto",
	  "seqmatchall" => "seqmatchall 'tembl:hsfau*' -auto",
	  "seqinfo" => "seqinfo tsw:opsd_human -auto",
	  "seealso" => "seealso fuzznuc",
	  "revseq" => "revseq tembl:hsfau -auto",
	  "restrict" => "restrict tembl:hsfau -auto",
	  "restover" => "restover tembl:hsfau -seqcomp acgt -auto",
	  "remap"    => "remap -auto",
	  "recoder" => "recoder tembl:hsfau -sbeg 31 -send 50 -auto",
	  "prophecy" => "prophecy $ENV{EPURE}/../test/data/globins.msf -type g -auto",
	  "prophet" => "prophet tsw:100k_rat $ENV{EPURE}/../test/data/prophecy.gribskov -auto",
	  "profit" => "profit $ENV{EPURE}/../test/data/globins.prophecy tsw:100k_rat -auto",
	  "primersearch" => "primersearch 'tembl:*' $ENV{EPURE}/../test/data/primers -auto",
	  "prima" => "prima tembl:hsfau -auto",
	  "prettyseq" => "prettyseq tembl:hsfau -auto",
	  "prettyplot" => "prettyplot $ENV{EPURE}/../test/data/globins.msf -auto",
	  "preg" => "preg 'tsw:*' -pattern kat -auto",
	  "ploydot" => "polydot 'tembl:hsfa*' -auto",
	  "plotorf" => "plotorf 'tembl:hsfa*' -auto",
	  "plotcon" => "plotcon $ENV{EPURE}/../test/data/globins.msf -auto",
	  "pepwindowall" => "pepwindowall tsw:opsd_human -auto",
	  "pepwindow" => "pepwindow tsw:opsd_human -auto",
	  "pepwheel" => "pepwheel tsw:opsd_human -auto",
	  "pepstats" => "pepstats tsw:opsd_human -auto",
	  "pepnet" => "pepnet tsw:opsd_human -auto",
	  "pepcoil" => "pepcoil tsw:opsd_human -auto",
	  "patmattest" => "patmattest tsw:opsd_human -expr kat -auto",
	  "patmatmotifs" => "patmatmotifs tsw:opsd_human -auto",
	  "patmatdb" => "patmatdb 'tsw:*' -motif kat -auto",
	  


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
