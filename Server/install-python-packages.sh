#! /bin/bash

for package in 'FPCONST' 'SOAPPY'
do
  cd ./$package
  python setup.py install --user
  cd ..  
done
