#!/usr/bin/env bash
set -e
testdir="$(cd "$(dirname "$0")" && pwd)"
testrun="$testdir/testrun"

CXX="c++ -std=c++11 -Wall"
# FRAMEWORKS="-framework CoreFoundation -framework Foundation -framework Cocoa"

rm -f "$testrun"

$CXX $CXXFLAGS "$testdir/tests.cpp" -o "$testrun"
"$testrun"
rm -f "$testrun"
