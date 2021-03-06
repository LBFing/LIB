#!/bin/sh

chmod -x *.cpp *.h
set -x
SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-build}
mkdir -p $BUILD_DIR \
	&& cd $BUILD_DIR \
	&& cmake -DCMAKE_BUILD_TYPE=Debug $SOURCE_DIR \
	&& make $*
