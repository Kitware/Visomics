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
