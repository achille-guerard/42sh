#!/bin/sh

. ../../functions.sh --source-only

#break tests
echo -e "\n${BLUE}\tSTEP 3 - Break Tests 🧪\n${Color_Off}"
test_42sh_file "../files/break1.sh"
test_42sh_file "../files/break2.sh"
test_42sh_file "../files/break3.sh"
test_42sh_file "../files/break4.sh"

#continue tests
echo -e "\n${BLUE}\n\t STEP 3 - Continue Tests 🧪\n${Color_Off}"
test_42sh_stdin "../files/con1.sh"
test_42sh_stdin "../files/con2.sh"
test_42sh_stdin "../files/con3.sh"
test_42sh_stdin "../files/con4.sh"

echo -e "\n${BLUE}RESULTS -- File tests:${Color_Off} $passed/$((passed+failed)) tests passed"
