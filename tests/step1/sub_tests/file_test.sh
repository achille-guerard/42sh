#!/bin/sh

. ../../functions.sh --source-only

#File tests
echo -e "\n${BLUE}\tSTEP 1 - File tests 🧪\n${Color_Off}"
test_42sh_file "../files/test1.sh"
test_42sh_file "../files/test2.sh"
test_42sh_file "../files/test3.sh"
test_42sh_file "../files/test4.sh"
test_42sh_file "../files/test5.sh"
test_42sh_file "../files/test6.sh"
test_42sh_file "../files/tiger.sh"

#STDIN tests
echo -e "\n${BLUE}\n\t STEP 1 - Stdin Tests V1 : < Edition 🧪\n${Color_Off}"
test_42sh_stdin "../files/test1.sh"
test_42sh_stdin "../files/test2.sh"
test_42sh_stdin "../files/test3.sh"
test_42sh_stdin "../files/test4.sh"
test_42sh_stdin "../files/test5.sh"
test_42sh_stdin "../files/test6.sh"

echo -e "\n${BLUE}\n\t STEP 1 - Stdin Tests V1 : | Edition 🧪\n${Color_Off}"
test_42sh_stdin2 "../files/test1.sh"
test_42sh_stdin2 "../files/test2.sh"
test_42sh_stdin2 "../files/test3.sh"
test_42sh_stdin2 "../files/test4.sh"
test_42sh_stdin2 "../files/test5.sh"
test_42sh_stdin2 "../files/test6.sh"
delete

echo -e "\n${BLUE}RESULTS -- File tests:${Color_Off} $passed/$((passed+failed)) tests passed"
