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

Visomics Server Installation Instructions
-----------------------------------------


On a Debian machine:
  - Run 'build-python-packages.sh'
  - Copy the created directories 'FPCONST', 'SOAPPY', and the file 'install-python-packages.sh' to a common location on the machine that will host the server
  - Delete the directories 'FPCONST', 'SOAPPY' from this machine

On the server machine:
  - Run 'install-python-packages.sh'
  - Delete the directories 'FPCONST', 'SOAPPY', and the file 'install-python-packages.sh' from this machine
  - Copy the directory 'Server/visomics-server' and the file 'Server/run-visomics-server.sh' from the Visomics Git repository to a location on on this machine
  - Run the command 'crontab -e' and add the line (without single quotes):
      '@hourly $HOME/run-visomics-server.sh >> $HOME/visomics-server.log 2>&1'
    replace '$HOME' with the location of the files, if they are not directly under the home directory
