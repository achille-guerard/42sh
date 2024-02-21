#!/bin/sh

. ../../functions.sh -- source-only
echo -e "\n${BLUE}\tSTEP 3 - Exit 🧪\n${Color_Off}"
test_42sh_file "../files/test_exit/test_exit_1.sh"
test_42sh_file "../files/test_exit/test_exit_2.sh"
test_42sh_file "../files/test_exit/test_exit_3.sh"
test_error2 "exit 3 1"
delete
echo -e "\n${BLUE}RESULTS -- Exit 🧪 test:${Color_Off} $passed/$((passed+failed)) tests passed"

