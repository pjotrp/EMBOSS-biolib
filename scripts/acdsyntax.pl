#!/usr/local/bin/perl -w

use English;

open (TABLE, ">test.html");

print TABLE "<HTML><HEAD></HEAD><BODY>\n";

# read the entrails full output

%section = ();
%acdattr = ();
%acdcalc = ();
%acdqual = ();
%acddefdefa = ();
%acddefcomm = ();
%stages = (
	   "ACD Types" => "TYPES",
	   "ACD Default attributes" => "DEFATTR",
	   "ACD Calculated attributes" => "CALCATTR",
	   "sequence input formats" => "SEQINPUT",
	   "sequence output formats" => "SEQOUTPUT",
	   "Sequence Types" => "SEQTYPE",
	   "sequence access methods" => "SEQACCESS",
	   "Database attributes" => "DBATTR",
	   "Resource attributes" => "RESATTR",
	   "alignment output formats" => "ALIGNOUTPUT",
	   "report output formats" => "REPORTOUTPUT",
	   "Graphics Devices" => "GRAPHDEV",
	   "Other" => "OTHER"
	   );

%sectionnames = (
	     "simple" => "Simple",
	     "input" => "Input",
	     "selection" => "Selection list",
	     "output" => "Output",
	     "graph" => "Graphics",
	     "other" => ""
	     );

sub doacdtypes() {
    my $attr;
    my $type;
    my $default;
    my $section;
    my $comment;

    while ($line) {
	if ($line =~ /^[\}]/) {return 1}
	if ($line =~ /^  (\S+)\s+(\S+)\s+\"([^\"]*)\"$/) {
	    $acd = $1;
	    $section = $2;
	    $comment = $3;
#	    print "\nacd '$acd' section '$section' comment '$comment'\n";
	    if (defined($section{$section})) {
		$section{$section} = "$section{$section} $acd";
	    }
	    else {
		$section{$section} = "$acd";
	    }
	    $acdcomm{$acd} = $comment;
	}
	if ($line =~ /^      (\S+)\s+(\S+)\s+\"([^\"]*)\"$/) {
	    $attr = $1;
	    $type = $2;
	    $comment = $3;
	    $default = "";
	    if ($comment =~ /default:(.+)$/) {
		$default = $1;
		$comment = $PREMATCH;
	    }
#	    print "attr '$attr' type '$type' default '$default' comment '$comment'\n";
	    if (defined($acdattr{$acd})) {
		$acdattr{$acd} = "$acdattr{$acd} $attr";
	    }
	    else {
		$acdattr{$acd} = "$attr";
	    }
	    $acdattrtype{"$acd\_$attr"} = $type;
	    $acdattrcomm{"$acd\_$attr"} = $comment;
	    $acdattrdefa{"$acd\_$attr"} = $default;
	}
	if ($line =~ /^      (\S+)\s+(\S+)\s+\"([^\"]*)\"\s+\"([^\"]*)\"$/) {
	    $qual = $1;
	    $type = $2;
	    $default = $3;
	    $comment = $4;
#	    print "qual '$qual' type '$type' default '$default' comment '$comment'\n";
	    if (defined($acdqual{$acd})) {
		$acdqual{$acd} = "$acdqual{$acd} $qual";
	    }
	    else {
		$acdqual{$acd} = "$qual";
	    }
	    $acdqualtype{"$acd\_$qual"} = $type;
	    $acdqualcomm{"$acd\_$qual"} = $comment;
	    $acdqualdefa{"$acd\_$qual"} = $default;
	}
	$line = <> ;
   }
}

sub doacddefattr() {
    my $attr;
    my $type;
    my $default;
    my $comment;

    while ($line) {
	if ($line =~ /^$/) {return 1}
	if ($line =~ /^  (\S+)\s+(\S+)\s+\"([^\"]*)\"\s+\"([^\"]*)\"$/) {
	    $attr = $1;
	    $type = $2;
	    $default = $3;
	    $comment = $4;
	    print "\ndefattr '$attr' type $type default '$default' comment '$comment'\n";
	    $acddeftype{$attr} = "$type";
	    $acddefdefa{$attr} = "$default";
	    $acddefcomm{$attr} = "$comment";
	}
	$line = <> ;
    }
}

sub doacdcalcattr() {
    my $acd;
    my $attr;
    my $type;
    my $default;
    my $comment;

    while ($line) {
	if ($line =~ /^$/) {return 1}
	if ($line =~ /^  (\S+)$/) {
	    $acd = $1;
	    print "\ncalcattr '$acd'\n";
	}
	if ($line =~ /^      (\S+)\s+(\S+)\s+\"([^\"]*)\"\s+\"([^\"]*)\"$/) {
	    $attr = $1;
	    $type = $2;
	    $default = $3;
	    $comment = $4;
	    print "\ncalcattr '$acd' attr '$attr' type $type default '$default' comment '$comment'\n";
	    if (defined($acdcalc{$acd})) {
		$acdcalc{$acd} = "$acdcalc{$acd} $attr";
	    }
	    else {
		$acdcalc{$acd} = "$attr";
	    }
	    $acdcalctype{"$acd\_$attr"} = "$type";
	    $acdcalcdefa{"$acd\_$attr"} = "$default";
	    $acdcalccomm{"$acd\_$attr"} = "$comment";
	}
	$line = <> ;
    }
}

sub dotabletypes() {
    print TABLE "
<table border=1 cellpadding=0>
";
    print TABLE "
 <tr>
  <td>
  <p align=center><b>Data type / Object</b></p>
  </td>
  <td>
  <p align=center><b>Description</b></p>
  </td>
  <td>
  <p align=center><b>Calculated Attributes</b></p>
  </td>
  <td>
  <p align=center><b>Specific Attributes</b></p>
  </td>
  <td>
  <p align=center><b>Command Line Qualifiers</b></p>
  </td>
 </tr>
";

    print TABLE "
 <tr>
  <td colspan=5>
  <p align=center><b>All data types</b></p>
  </td>
 </tr>
";

### The default attributes section
    print TABLE "
 <tr>
";
    print TABLE "
  <td>
  <p>&nbsp;</p>
  </td>
  <td>
  <p>All data types</p>
  </td>
  <td>
  <p>&nbsp;</p>
  </td>
";
    print TABLE "
 <td>
 <p>
";
    foreach $x (sort(keys(%acddefdefa))) {
	$defa = $acddefdefa{$x};
	$comm = $acddefcomm{$x};
	if ($defa =~ /[(](.*)[)]/) {$defa = "<i>$1</i>"}
	else {$defa = "\"$defa\""}
	print TABLE "$x: $defa<br>\n";
    }

    print TABLE "
 </p>
 </td>
";

    print TABLE "
  <td>
  <p>&nbsp;</p>
  </td>
 </tr>
";

### The sections, in order

    foreach $s ("simple", "input", "selection", "output", "graph", "other") {
	if (!defined($section{$s})) {next}
	print TABLE "
 <tr>
  <td colspan=5>
  <p align=center><b>$sectionnames{$s} types</b></p>
  </td>
 </tr>
";

	foreach $a (split(" ", $section{$s})) {
	    $comm = $acdcomm{$a};
	   print TABLE "
 <tr>
  <td>
  <p align=center><b>$a</b></p>
  </td>
  <td>
  <p>$comm</p>
  </td>
";

### Calculated attributes

	   print TABLE "
  <td>
  <p>
";
	    if (defined($acdcalc{$a})) {
		foreach $y (split(" ", $acdcalc{$a})) {
		    $defa = $acdcalcdefa{"$a\_$y"};
		    $type = $acdcalctype{"$a\_$y"};
		    $comm = $acdcalccomm{"$a\_$y"};
		    print TABLE "$y ($type)<br>\n";
		}
	    }
	    else {print TABLE "&nbsp;\n";}
	    print TABLE "
 </p>
 </td>
";

### Specific attributes

	   print TABLE "
  <td>
  <p>
";
	    if (defined($acdattr{$a})) {
		foreach $y (split(" ", $acdattr{$a})) {
		    $defa = $acddefdefa{"$a\_$y"};
		    $comm = $acddefcomm{"$a\_$y"};
		    if ($defa =~ /[(](.*)[)]/) {$defa = "<i>$1</i>"}
		    else {$defa = "\"$defa\""}
		    print TABLE "$y: $defa<br>\n";
		}
	    }
	    else {print TABLE "&nbsp;\n";}

	    print TABLE "
 </p>
 </td>
";

### Associated qualifiers

	   print TABLE "
  <td>
  <p>
";
	    if (defined($acdqual{$a})) {
		foreach $y (split(" ", $acdqual{$a})) {
		    $defa = $acdqualdefa{"$a\_$y"};
		    $comm = $acdqualcomm{"$a\_$y"};
		    if ($defa =~ /[(](.*)[)]/) {$defa = "<i>$1</i>"}
		    else {$defa = "\"$defa\""}
		    print TABLE "$y: $defa<br>\n";
		}
	    }
	    else {print TABLE "&nbsp;\n";}

	    print TABLE "
 </p>
 </td>
";
	}
	    print TABLE "
 </tr>
";
    }
	    print TABLE "
 </table>
";
}


$stage = "";
while ($line = <>) {
    chomp $line;
    if ($line =~ /^\#\s*(\S.*)/) {
	if ($stages{$1}) {
	    $stage = $stages{$1};
	}
	else {
	    $stage = "OTHER";
	}
	while ($line =~ /^\#/) {
	    $line = <>;
	}
        print "Reading $stage\n";
    }

    if ($stage eq "TYPES") {doacdtypes();print "doacdtypes done\n"}
    if ($stage eq "DEFATTR") {doacddefattr();print "doacddefattr done\n"}
    if ($stage eq "CALCATTR") {doacdcalcattr();print "doacdcalcattr done\n"}
  
}

foreach $x (sort(keys(%section))) {
    print "Section $x $section{$x}\n";
}

foreach $x (sort(keys(%acddefdefa))) {
    print "Default $x $acddeftype{$x} $acddefdefa{$x} $acddefcomm{$x}\n";
}

foreach $x (sort(keys(%acdattr))) {
    print "ACD $x\n";
    if (defined($acdcalc{$x})) {
	print "  calc $acdcalc{$x}\n";
    }
    if (defined($acdattr{$x})) {
	print "  attributes:\n";
	foreach $y (split(" ", $acdattr{$x})) {
	    $type = $acdattrtype{"$x\_$y"};
	    $defa = $acdattrdefa{"$x\_$y"};
	    $comm = $acdattrcomm{"$x\_$y"};
	    print "    $y $type $defa $comm\n"; 
	}
    }
    if (defined($acdqual{$x})) {
	print "  qualifiers:\n";
	foreach $y (split(" ", $acdqual{$x})) {
	    $type = $acdqualtype{"$x\_$y"};
	    $defa = $acdqualdefa{"$x\_$y"};
	    $comm = $acdqualcomm{"$x\_$y"};
	    print "    $y $type $defa $comm\n"; 
	}
    }
}

dotabletypes();
    print TABLE "</BODY></HTML>\n";

exit();

