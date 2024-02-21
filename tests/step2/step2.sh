#!/bin/sh

RED='\033[1;91m'
GREEN='\033[1;32m'
YELLOW='\033[0;93m'
BLUE='\033[0;36m'
Color_Off='\033[0m'

echo -e "\n${BLUE}------------------------------\nSTEP 2 TESTS :\n${Color_Off}"
    cd sub_tests
    ./echo_test.sh
    ./loop.sh
    ./expander.sh
    ./variables.sh
    ./andor_test.sh
    ./redirections.sh
    ./pipeline.sh
    ./negation.sh
    cd ..
echo -e "\n${BLUE}\nSTEP 2 TESTS -- COMPLETED\n------------------------------\n${Color_Off}"
