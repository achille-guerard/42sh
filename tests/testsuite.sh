#!/bin/sh
RED='\033[1;91m'
GREEN='\033[1;32m'
YELLOW='\033[0;93m'
BLUE='\033[0;36m'
Color_Off='\033[0m'

legend()
{
    echo -e "\n${BLUE}COLOR CODE :\n${Color_Off}"
    echo -e "👍 ${GREEN}PASSED${Color_Off} : As expected"
    echo -e "🤏 ${YELLOW}BAD ERROR CODE${Color_Off} : Error with the exit code"
    echo -e "👎 ${RED}FAILED${Color_Off} : Error with the output\n"
}


if [ ! -x ../src/42sh ]; then
    echo "Error: Cannot execute the binary"
else
    echo -e "\n${BLUE}WELCOME TO : 42SH - TEST SUITE\n------------------------------\n${Color_Off}"
    legend 
    cd fix
    ./test_fix.sh
    cd ..
    
    cd step1
    ./step1.sh
    cd ..

    cd step2
    ./step2.sh
    cd ..
    
    cd step3
    ./step3.sh
    cd ..

    cd step4
    ./step4.sh
    cd ..


    echo -e "\n${BLUE}\n42SH - TEST SUITE - COMPLETED, HOPE YOU DID WELL 👌\n${Color_Off}"
fi
