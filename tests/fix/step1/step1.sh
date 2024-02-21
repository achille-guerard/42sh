#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}FIX - STEP 1 TESTS\n------------------------------\n${Color_Off}"

./fix_echo.sh
# add your files here

cd ..
echo -e "\n${BLUE}\nFIX - STEP 1 TESTS -- COMPLETED\n------------------------------\n${Color_Off}"
