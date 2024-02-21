#!/bin/sh

. ../../functions.sh --source-only

echo -e "\n${BLUE}\tSTEP 2 - Simple Negation with And/Or 🧪\n${Color_Off}"
test_42sh_string "! true || false" 
test_42sh_string "! true || echo toto"
test_42sh_string "! echo foo && caca || echo toto"

echo -e "\n${BLUE}\tSTEP 2 - Simple Negation with Pipes 🧪\n${Color_Off}"
test_42sh_string "! echo a | cat -e" 
test_42sh_string "! echo aaa | cat -e"
test_42sh_string "! echo toto | wc"

echo -e "\n${BLUE}\tSTEP 2 - Negation with Nested Pipes 🧪\n${Color_Off}"
test_42sh_string "! echo toto | tr -d o | cat -e | wc "
test_42sh_string "! true | false"
test_42sh_string "! true | false | tree"
test_error2 "! true | false | tree | tri"

delete
echo -e "\n${BLUE}RESULTS -- Pipe Tests:${Color_Off} $passed/$((passed+failed)) tests passed"
