#!/bin/csh -f

set app = $argv[1]
cd ~/cvsemboss/ajax 
echo "make AJAX"
make
cd ~/cvsemboss/nucleus
echo "make NUCLEUS"
make
cd ~/cvsemboss/emboss
echo "make $app"
make $app
echo "install $app"
cp $app ~/local/bin
cp acd/$app.acd ~/local/share/EMBOSS/acd/
