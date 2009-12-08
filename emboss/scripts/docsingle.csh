#!/bin/csh -f

set file = $argv[1]
set name = $file:r

embossdoc.pl ~/devemboss/ajax/graphics/$file > ! $name.out
embossdocreport.pl $name.out
mv embossdocreport.log $name.report

