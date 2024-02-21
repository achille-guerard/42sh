#!/bin/sh

RED='\033[1;91m'
GREEN='\033[1;32m'
YELLOW='\033[0;93m'
BLUE='\033[0;36m'
Color_Off='\033[0m'

echo -e "\n${BLUE}------------------------------\nSTEP 1 TESTS :\n${Color_Off}"
    cd sub_tests
    ./echo_test.sh
    ./builtin.sh
    ./if_else_test.sh
    ./file_test.sh
    ./error_test.sh
    cd ..
echo -e "\n${BLUE}\nSTEP 1 TESTS -- COMPLETED\n------------------------------\n${Color_Off}"
