#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - Echo -E tests 🧪\n${Color_Off}"
test_42sh_string "echo -E \t"
test_42sh_string "echo -E \n"
test_42sh_string 'echo -e "\t"'
test_42sh_string 'echo -e "\n"'

echo -e "\n${BLUE}\tSTEP 2 - Echo -e tests 🧪\n${Color_Off}"


test_42sh_string 'echo -e "\n"'
test_42sh_string "echo -e \n"

echo -e "\n${BLUE}\tSTEP 2 - Echo -n tests 🧪\n${Color_Off}"
test_42sh_string 'echo -n "no newline at the end pls"'
test_42sh_string "echo -n no newline at the end pls"
test_42sh_string "echo -n \n"
test_42sh_file "../files/test_echo_backslash.sh"
delete
echo -e "\n${BLUE}RESULTS -- Echo test:${Color_Off} $passed/$((passed+failed)) tests passed"
