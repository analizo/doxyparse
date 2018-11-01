#!/bin/bash

VERSION=$(cat VERSION)
ARCH=$(uname -m)

if [[ "$ARCH" == "x86_64" ]]; then
  ARCH=amd64
else if [[ "$ARCH" == "i686" ]]; then 
  ARCH=i386
else
  echo "Unsupported $ARCH architecture!"; 
fi;
fi;

# Generating tarball
tar cvf doxyparse_$VERSION-$ARCH.tar.gz *

# TODO: This command is not working yet for unknown reason
# Generating debian package
gbp buildpackage
