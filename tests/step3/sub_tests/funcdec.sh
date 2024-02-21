#!/bin/sh

. ../../functions.sh -- source-only
echo -e "\n${BLUE}\tSTEP 3 - Fun dec 🧪\n${Color_Off}"
test_42sh_file "../files/funcdec/funcdec_1.sh"
test_42sh_file "../files/funcdec/funcdec_2.sh"
test_42sh_file "../files/funcdec/funcdec_3.sh"
test_42sh_file "../files/funcdec/funcdec_4.sh"
test_42sh_file "../files/funcdec/funcdec_5.sh"
delete
echo -e "\n${BLUE}RESULTS -- Fun dec 🧪 test:${Color_Off} $passed/$((passed+failed)) tests passed"
