#! /bin/bash

###########################################################################
#
#  Program: Visomics
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

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
