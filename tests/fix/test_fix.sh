#!/bin/sh
. ../functions.sh --source-only

echo -e "\n${BLUE}FIX - TESTS\n------------------------------\n${Color_Off}"

cd step1
./step1.sh
cd ..
