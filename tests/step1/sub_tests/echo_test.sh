#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 1 - Echo tests 🧪\n${Color_Off}"
test_42sh_string "echo"
test_42sh_string "echo Hello, World!"
test_42sh_string "echo fi"
test_42sh_string "echo if true then echo a else echo b fi"
test_42sh_string "echo papa toto trololo"

echo -e "\n${BLUE}\tSTEP 1 - Echo -E tests 🧪\n${Color_Off}"
test_42sh_string "echo -E toto"


echo -e "\n${BLUE}\tSTEP 1 - Echo -e tests 🧪\n${Color_Off}"
test_42sh_string "echo -e"
test_42sh_string "echo -e toto"

echo -e "\n${BLUE}\tSTEP 1 - Echo -n tests 🧪\n${Color_Off}"
test_42sh_string "echo -n"
test_42sh_string "echo -n no newline at the end pls"
delete
echo -e "\n${BLUE}RESULTS -- Echo:${Color_Off} $passed/$((passed+failed)) tests passed"
