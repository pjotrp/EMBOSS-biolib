#!/bin/csh

../emboss/entrails -auto -full entrails-full.txt 
grep 'ajNamGetValue' ../ajax/*.c >! acdsyntax.getvalue
../scripts/acdstats.pl >! acdsyntax.acdstats 
../scripts/acdsyntax.pl entrails-full.txt 
cp test.html ~/public_html/syntax.html
cp test.html ../doc/manuals/acd/syntax.html
cp content.html ~/public_html/content.html
cp content.html ../doc/manuals/acd/content.html
#rm acdsyntax.getvalue
