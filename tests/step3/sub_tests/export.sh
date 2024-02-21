#!/bin/sh

. ../../functions.sh -- source-only
echo -e "\n${BLUE}\tSTEP 3 - Export 🧪\n${Color_Off}"

test_42sh_string "export toto=tata"
test_42sh_string "export toto=tata ; env | grep toto"
test_42sh_string "alala=2 ; export alala ; env | grep alala"
test_42sh_string "export toto=tata("
test_42sh_string "export toto="
test_42sh_string "export toto=''"
delete
echo -e "\n${BLUE}RESULTS -- Export 🧪 test:${Color_Off} $passed/$((passed+failed)) tests passed"

