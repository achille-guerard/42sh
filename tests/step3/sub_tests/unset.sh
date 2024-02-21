#!/bin/sh

. ../../functions.sh -- source-only
echo -e "\n${BLUE}\tSTEP 3 - Unset 🧪\n${Color_Off}"
test_42sh_string "unset a"
test_42sh_string "export toto=tata; env | grep toto; unset toto; env | grep toto"
delete
echo -e "\n${BLUE}RESULTS -- Unset 🧪 test:${Color_Off} $passed/$((passed+failed)) tests passed"

