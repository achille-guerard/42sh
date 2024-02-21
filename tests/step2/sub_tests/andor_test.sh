#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - &&/|| tests 🧪\n${Color_Off}"
test_42sh_string "echo toto || echo prout && echo caca"
test_42sh_string "false && echo foo || echo bar"
test_42sh_string 'true || echo foo && echo bar'
test_42sh_string 'true || caca && echo titi || echo mouton cheh'
delete
echo -e "\n${BLUE}RESULTS -- &&/|| test:${Color_Off} $passed/$((passed+failed)) tests passed"
