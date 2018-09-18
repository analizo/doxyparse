#!bin/sh

# Compiling
cmake -G "Unix Makefiles" -Dbuild_parse=ON
make
make install

# Generating tarball
debuild -us -uc
