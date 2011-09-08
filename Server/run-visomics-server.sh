#!/bin/bash

PROCESS=$(ps x | grep -v 'grep' | grep 'python ./visomics-server.py')
if [ -n "$PROCESS" ]
then
  exit 0
else
  echo "SOAP server started `date`"
  python ./visomics-server/visomics-server.py
fi
