#!/bin/sh

EXE=`which --skip-alias analyzer`
if [ -n "$EXE" ]; then
  LIB=`ldd $EXE |fgrep libet|awk '{print $3}'`
  if [ -n "$LIB" ]; then
    echo -L`dirname $LIB` -let
  fi
fi
