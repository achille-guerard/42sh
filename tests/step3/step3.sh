#!/bin/sh

RED='\033[1;91m'
GREEN='\033[1;32m'
YELLOW='\033[0;93m'
BLUE='\033[0;36m'
Color_Off='\033[0m'

echo -e "\n${BLUE}------------------------------\nSTEP 3 TESTS :\n${Color_Off}"
    cd sub_tests
    # add files here
    ./break_test.sh
    ./comsub.sh
    ./command_block.sh
    ./exit.sh
    ./dot_test.sh
    ./cd.sh
    ./funcdec.sh
    ./unset.sh
    ./export.sh
    cd ..
echo -e "\n${BLUE}\nSTEP 3 TESTS -- COMPLETED\n------------------------------\n${Color_Off}"
