#!/bin/bash
# Build script for rebuilding everything
set echo on

echo "Building everything..."


# pushd webserver
# source build.sh
# popd
make -f Makefile.webserver.linux.mak all

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

# pushd testsuite
# source build.sh
# popd

make -f Makefile.testsuite.linux.mak all
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

# pushd client
# source build.sh
# popd

make -f Makefile.client.linux.mak all
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && exit
fi

echo "All assemblies built successfully."

echo "Executing postbuild..."

./post-build.sh

echo "Postbuild completed successfully."