#!/bin/csh

(../../scripts/valgrind.pl -all >! valgrind.out) >&! valgrind.err
grep '^Valgrind test' valgrind.err
