#!/bin/csh

../emboss/entrails -full entrails-full.txt 
grep 'ajNamGetValue' ../ajax/*.c >! acdsyntax.getvalue
../scripts/acdsyntax.pl entrails-full.txt 
cp test.html ~/public_html/acdtables.html
#rm acdsyntax.getvalue
