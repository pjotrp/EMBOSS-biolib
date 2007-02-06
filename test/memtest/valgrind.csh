#!/bin/csh

setenv EMBOSSRC ../
setenv EMBOSS_OUTDIRECTORY output
(../../scripts/valgrind.pl -all >! valgrind.out) >&! valgrind.err
(../../scripts/valgrind.pl -testfile=qatestcmd.dat -all >! valgrindqa.out) >&! valgrindqa.err
grep '^Valgrind test' valgrind.err valgrindqa.err
