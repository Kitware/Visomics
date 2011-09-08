#! /bin/bash

FPCONST_CONTROL='http://ftp.de.debian.org/debian/pool/main/f/fpconst/fpconst_0.7.2-4.dsc'
FPCONST_SOURCE='http://ftp.de.debian.org/debian/pool/main/f/fpconst/fpconst_0.7.2.orig.tar.gz'
FPCONST_PATCH='http://ftp.de.debian.org/debian/pool/main/f/fpconst/fpconst_0.7.2-4.diff.gz'

SOAPPY_CONTROL='http://ftp.de.debian.org/debian/pool/main/p/python-soappy/python-soappy_0.12.0-4.dsc'
SOAPPY_SOURCE='http://ftp.de.debian.org/debian/pool/main/p/python-soappy/python-soappy_0.12.0.orig.tar.gz'
SOAPPY_PATCH='http://ftp.de.debian.org/debian/pool/main/p/python-soappy/python-soappy_0.12.0-4.diff.gz'

WGET_OPTIONS='--timestamping --no-verbose'

for package in 'FPCONST' 'SOAPPY'
do
  for file in 'CONTROL' 'SOURCE' 'PATCH'
  do
    eval URL=\${${package}_${file}}
    wget $WGET_OPTIONS $URL
  done

  CONTROL_FILE=$(eval echo \${${package}_CONTROL} |  awk -F / '{print $NF}')
  dpkg-source -x $CONTROL_FILE $package
  cd ./$package
  dpatch apply-all
  cd ..

  for file in 'CONTROL' 'SOURCE' 'PATCH'
  do
    FILE_NAME=$(eval echo \${${package}_${file}} |  awk -F / '{print $NF}')
    rm $FILE_NAME
  done
done

#dpatch --chdir --workdir ./$package apply-all
