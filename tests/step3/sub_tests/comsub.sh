#!/bin/sh

. ../../functions.sh --source-only

#comsub testing
echo -e "\n${BLUE}\tSTEP 3 - Commabd substitution 🧪\n${Color_Off}"

test_42sh_file "../files/comsub1.sh"
test_42sh_file "../files/comsub2.sh"
test_42sh_file "../files/comsub3.sh"
#test_42sh_file "../files/comsub4.sh"


echo -e "\n${BLUE}RESULTS -- File tests:${Color_Off} $passed/$((passed+failed)) tests passed"
