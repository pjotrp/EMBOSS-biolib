#!/usr/local/bin/perl

use English;
$pubout = "public";
$locout = "local";
$infile = "";
$lib = "unknown";
@lattrs = ();

if ($ARGV[0]) {$infile = $ARGV[0];}
if ($ARGV[1]) {$lib = $ARGV[1];}

$source = "";
$lasttoken = "";

if ($infile) {
    ($file) = ($infile =~ /([^\/]+)$/);
    ($dummy, $dir, $pubout, $ext) = ($infile =~ /^(([^\/.]*)\/)*([^\/.]+)(\.\S+)?$/);
    $local = $pubout;
    if ($dir) {$lib = $dir}
    if ($ext eq ".h") { $exttype = "include"}
    elsif ($ext eq ".c") { $exttype = "source"}
    else { $exttype = "other"}
    print "set pubout '$pubout' lib '$lib' type '$exttype'\n";
    open (INFILE, "$infile") || die "Cannot open $infile";
    while (<INFILE>) {$source .= $_}
}
else {
    $file = "xxx.c";
    while (<>) {$source .= $_}
}

open (HTML, ">$pubout.html");
open (HTMLB, ">$pubout_static.html");
open (SRS, ">$pubout.srsdata");

$title = "$infile";

print HTML  "<html><head><title>$title</title></head><body bgcolor=\"#ffffff\">\n";
print HTMLB  "<html><head><title>$title</title></head><body bgcolor=\"#ffffff\">\n";

print HTML  "<h1>$pubout</h1>\n";
print HTMLB  "<h1>$pubout</h1>\n";

foreach $x ("new", "delete", "del", "ass", "mod", "use", "set", "cast", "use", "other", "alias", "attr", "cc") {
    $tables{$x} = 1;
}

##############################################################
## $source is the entire source file as a string with newlines
## step through each comment
## looking for extended JavaDoc style formatting
## $ccfull is the comment
## $rest is the rest of the file
##############################################################

while ($source =~ m"[\/][*][^*]*[*]+([^\/*][^*]*[*]+)*[\/]"gos) {
    $ccfull = $&;
    $rest = $POSTMATCH;

    ($cc) = ($ccfull =~ /^..\s*(.*\S)*\s*..$/gos);
    $cc =~ s/[* ]*\n[* ]*/\n/gos;

    $type = "";
    $acnt = 0;
    $rtype = "";
    @largs = ();
    while ($cc =~ m/@((\S+)([^@]+))/gos) {
	$data = $1;
	$token = $2;

	if ($token ne $lasttoken) {
	    if ($tables{$lasttoken}) {print $OFILE "</table>\n"}
	    $intable = 0;
	}

	if ($token eq "data")  {
	    $nattr = 0;
	    $OFILE = HTML;
	    $type = $token;
	    ($name, $frest) = ($data =~ /\S+\s+(\S+)\s*(.*)/gos);
	    $dtypedefname = "";
	    ($dtype, $dattrs, $dtypeb, $ddefine) =
		$rest =~ /^\s*([^\{]*)\{([^\}]+)[\}]([^;]*)[;]\s*([\#]define[^\n]+)?/os;

	    print "Data type $name\n";
	    print $OFILE "<hr><h2>Data type ".srsdref($name)."</h2>\n";
	    $srest = $frest;
	    $frest =~ s/\n\n/\n<p>\n/gos;
	    print $OFILE "$frest\n";

	    print SRS "ID $name\n";
	    print SRS "TY public\n";
	    print SRS "MO $pubout\n";
	    print SRS "LB $lib\n";
	    print SRS "XX\n";

	    if ($exttype ne "include") {
		print "bad scope 'data' in $exttype file\n";
	    }
	    if (!$frest) {print "bad data type '$name', no description\n"}

	    $srest =~ s/\n\n+$/\n/gos;
	    $srest =~ s/\n\n\n+/\n\n/gos;
	    $srest =~ s/\n([^\n])/\nDE $1/gos;
	    $srest =~ s/\n\n/\nDE\n/gos;
	    print SRS "DE $srest";
	    print SRS "XX\n";

	    $icc=0;
	    @lattrs = ();
	    while ($dattrs =~ /([^;]+)[;]\s*([\/][*][^*]*[*]+([^\/*][^*]*[*]+)*[\/][^\n]*[\n])?/gos) {
		$dattr = $1;
		$dcc = $2;
		$dattr =~ s/^\s+//gos;
		$dattr =~ s/\s+$//gos;
		$dattr =~ s/\s+/ /gos;
		$dattr =~ s/ ([*]+)/\1 /gos;
		push @lattrs, $dattr;
		if ($dcc ne "") {
		    $icc++;
		}
	    }
	    if ($icc) {
		print "bad data definition $name, has $icc comments\n";
	    }
	}

	if ($token eq "datastatic")  {
	    $nattr = 0;
	    $OFILE = HTMLB;
	    $type = $token;
	    ($name, $frest) = ($data =~ /\S+\s+(\S+)\s*(.*)/gos);
	    print "Static data type $name\n";
	    ($dtype, $dattrs, $dtypeb, $ddefine) =
		$rest =~ /^\s*([^\{]*)\{([^\}]+)[\}]([^;]*)[;]\s*([\#]define[^\n]+)?/os;

	    print $OFILE "<h2>Static data type $name</h2>\n";
	    $srest = $frest;
	    $frest =~ s/\n\n/\n<p>\n/gos;
	    print $OFILE "$frest\n";

	    print SRS "ID $name\n";
	    print SRS "TY static\n";
	    print SRS "MO $pubout\n";
	    print SRS "LB $lib\n";
	    print SRS "XX\n";

	    if ($exttype ne "source") {
		print "bad scope 'datastatic' in $exttype file\n";
	    }
	    if (!$frest) {print "bad data type '$name', no description\n"}

	    $srest =~ s/\n\n+$/\n/gos;
	    $srest =~ s/\n\n\n+/\n\n/gos;
	    $srest =~ s/\n([^\n])/\nDE $1/gos;
	    $srest =~ s/\n\n/\nDE\n/gos;
	    print SRS "DE $srest";
	    print SRS "XX\n";

	    $icc=0;
	    @lattrs = ();
	    while ($dattrs =~ /([^;]+)[;]\s*([\/][*][^*]*[*]+([^\/*][^*]*[*]+)*[\/][^\n]*[\n])?/gos) {
		$dattr = $1;
		$dcc = $2;
		$dattr =~ s/^\s+//gos;
		$dattr =~ s/\s+$//gos;
		$dattr =~ s/\s+/ /gos;
		$dattr =~ s/ [*]/* /gos;
		push @lattrs, $dattr;
		if ($dcc ne "") {
		    $icc++;
		}
	    }
	    if ($icc) {
		print "bad data definition $name, has $icc comments\n";
	    }
	}

	if ($token eq "datatype")  {
	    $nattr = 0;
	    $OFILE = HTMLB;
	    $type = $token;
	    ($name, $frest) = ($data =~ /\S+\s+(\S+)\s*(.*)/gos);
	    $dattrs = "";
	    $dtypeb = "";
	    $ddefine = "";
	    ($dattrtype, $dtypedefname) =
		$rest =~ /^\s*typedef\s*([^\;]+\S)\s+(\S+)\s*[;]/os;

	    print "Typedef data type $name\n";
	    print $OFILE "<h2>Typedef data type $name</h2>\n";
	    $srest = $frest;
	    $frest =~ s/\n\n/\n<p>\n/gos;
	    print $OFILE "$frest\n";

	    print SRS "ID $name\n";
	    print SRS "TY typedef\n";
	    print SRS "MO $pubout\n";
	    print SRS "LB $lib\n";
	    print SRS "XX\n";

	    if ($dtypedefname ne $name) {
		print "bad data type name '$name' typedef is '$dtypedefname'\n";
	    }

	    if (!$frest) {print "bad data type '$name', no description\n"}

	    $srest =~ s/\n\n+$/\n/gos;
	    $srest =~ s/\n\n\n+/\n\n/gos;
	    $srest =~ s/\n([^\n])/\nDE $1/gos;
	    $srest =~ s/\n\n/\nDE\n/gos;
	    print SRS "DE $srest";
	    print SRS "XX\n";

	}

	if ($token eq "new")  {
	    if (!$intable) {
		print $OFILE "<h3>Constructor(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);
	    
	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nND $1/gos;
	    $drest =~ s/\n\n/\nND\n/gos;
	    print SRS "NN $fname\n";
	    print SRS "ND $drest";
	    print SRS "NX\n";

	    if (!$prest) {print "bad new spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "delete" || $token eq "del")  {
	    if (!$intable) {
		print $OFILE "<h3>Destructor(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nDD $1/gos;
	    $drest =~ s/\n\n/\nDD\n/gos;
	    print SRS "DN $fname\n";
	    print SRS "DD $drest";
	    print SRS "DX\n";

	    if (!$prest) {print "bad delete spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "use")  {
	    if (!$intable) {
		print $OFILE "<h3>Operator(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nOD $1/gos;
	    $drest =~ s/\n\n/\nOD\n/gos;
	    print SRS "ON $fname\n";
	    print SRS "OD $drest";
	    print SRS "OX\n";

	    if (!$prest) {print "bad use spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "ass" || $token eq "set")  {
	    if (!$intable) {
		print $OFILE "<h3>Assignment(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nED $1/gos;
	    $drest =~ s/\n\n/\nED\n/gos;
	    print SRS "EN $fname\n";
	    print SRS "ED $drest";
	    print SRS "EX\n";

	    if (!$prest) {print "bad assignment spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "mod")  {
	    if (!$intable) {
		print $OFILE "<h3>Modifier(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nMD $1/gos;
	    $drest =~ s/\n\n/\nMD\n/gos;
	    print SRS "MN $fname\n";
	    print SRS "MD $drest";
	    print SRS "MX\n";

	    if (!$prest) {print "bad modifier spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "cast")  {
	    if (!$intable) {
		print $OFILE "<h3>Cast(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nCD $1/gos;
	    $drest =~ s/\n\n/\nCD\n/gos;
	    print SRS "CN $fname\n";
	    print SRS "CD $drest";
	    print SRS "CX\n";

	    if (!$prest) {print "bad cast spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "other")  {
	    if (!$intable) {
		print $OFILE "<h3>Other related data structure(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nRD $1/gos;
	    $drest =~ s/\n\n/\nRD\n/gos;
	    $drest =~ s/^$/\n/gos;
	    print SRS "RN $fname\n";
	    print SRS "RD $drest";
	    print SRS "RX\n";

	    if (!$prest) {print "bad other spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsdref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "alias")  {
	    if (!$intable) {
		print $OFILE "<h3>Alias name(s)</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($fname,$prest) = ($data =~ m/\S+\s*(\S*)\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nRD $1/gos;
	    $drest =~ s/\n\n/\nRD\n/gos;
	    $drest =~ s/^$/\n/gos;
	    print SRS "TN $fname\n";
	    print SRS "TD $drest";
	    print SRS "TX\n";

	    ###if (!$prest) {print "bad alias spec '$fname', no description\n"}
	    print $OFILE "<tr><td>".srsdref($fname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "attr")  {
	    $nattr++;
	    if (!$intable) {
		print $OFILE "<h3>Attributes</h3>\n";
		print $OFILE "<p><table border=3>\n";
		print $OFILE "<tr><th>Name</th><th>Type</th><th>Description</th></tr>\n";
		$intable = 1;
	    }
	    ($aname,$atype, $prest) = ($data =~ m/\S+\s*(\S*)\s*[\[]([\[\]A-Za-z0-9_* \(\)]+)[\]]\s*(.*)/gos);

	    $drest = $prest;
	    $drest =~ s/\n\n+$/\n/gos;
	    $drest =~ s/\n\n\n+/\n\n/gos;
	    $drest =~ s/\n([^\n])/\nAD $1/gos;
	    $drest =~ s/\n\n/\nAD\n/gos;
	    $drest =~ s/^$/\n/gos;
	    print SRS "AN $aname\n";
	    print SRS "AT $atype\n";
	    print SRS "AD $drest";
	    print SRS "AX\n";

	    if (!$atype) {print "bad attribute spec '$aname', no type\n"}
	    if (!$prest) {print "bad attribute spec '$aname', no description\n"}
	    if ($#lattrs >= 0) {
		$dattr = shift @lattrs;
###	    print STDERR "dattr: '$dattr'\n";
		if ($dattr =~ /.*[\(][*][^\)]+\)/) {
		    ($dattrtype,$dattrname) = $dattr =~ /(.*\S)\s*[\(][*]([^\)]+)\)/;
		    $dattrtype="(".$dattrtype."*)";
		}
		else {
		    ($dattrtype,$dattrname) = $dattr =~ /(.*\S)\s+(\S+)$/;
		}
	    }
	    else {
		$dattrname = "typedef";
		if ($type eq "datatype") {
		    if ($name ne $dtypedefname) {
			print "bad datatype name '$name' typedef is '$dtypedefname'\n";
		    }
		    if ($atype ne $dattrtype) {
			print "bad cast <$atype> <$dattrtype>\n";
		    }
		}
	    }
	    if ($dattrname =~ /([^\[]+)([\[].*[\]])/) {
		$dattrtype .= $2;
		$dattrname = $1;
	    }
###	    print "attr: '$dattrname' type: '$dattrtype'\n";

	    if ($atype ne $dattrtype) {
		print "bad cast <$atype> <$dattrtype>\n";
	    }
	    if ($aname ne $dattrname) {
		print "bad attribute  <$aname> <$dattrname>\n";
	    }

	    print $OFILE "<tr><td>".srsdref($aname)."</td><td>$prest</td></tr>\n";
	}

	if ($token eq "@")  {
	    break;
	}

	$lasttoken = $token;
    }
    if ($type) {
	print "=============================\n";
	print SRS "//\n";

	($body,$a,$apart,$c) = ($rest =~ /((.*?\n([\s\{][^\n]*\n)*\S[^;]*;[^\n]*\n)([\s\n]*[\#]define\s+\S+\s+\S+[*]\s*?\n)?)/gos);
	$body =~ s/^\s+//m;
##    print "SRS:\n$body\n";
##    print "a: $a\n";
##    print "c: $c\n";

	print SRS $body;

	if (!$nattr) {
	    print "bad attributes - none found\n";
	}
	if ($#lattrs != -1) {
	    print "bad attributes - ".($#lattrs+1)." definition(s) missing\n";
	}
    }
}

print HTML "</body></html>\n";
print HTMLB "</body></html>\n";
close HTML;
close HTMLB;

sub srsref {
    return "<a href=\"/srs6bin/cgi-bin/wgetz?-e+[EFUNC-ID:$_[0]]\">$_[0]</a>";
}
sub srsdref {
    return "<a href=\"/srs6bin/cgi-bin/wgetz?-e+[EDATA-ID:$_[0]]\">$_[0]</a>";
}
