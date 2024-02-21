#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - Loop tests 🧪\n${Color_Off}"

test_42sh_string "while false; do echo toto; done"
test_42sh_string "until true; do echo toto; done"

echo -e "\n${BLUE}RESULTS -- Loop tests:${Color_Off} $passed/$((passed+failed)) tests passed"
