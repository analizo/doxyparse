#!bin/sh

VERSION=$(cat VERSION)
 
# Generating tarball
# tar cvf doxyparse-$VERSION.tar.gz *

# Generating debian package
# cd .. && dkpg-build doxyparse
# dpkg-buildpackage -rfakeroot -d -us -uc

# Generating tarball
gitbuild package
