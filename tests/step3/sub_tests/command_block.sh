#!/bin/sh

. ../../functions.sh --source-only

# if, else, true tests
echo -e "\n${BLUE}\tSTEP 3 - Command Block tests 🧪\n${Color_Off}"

test_42sh_string "{ if false; then hey; elif false; then lessgo; else echo coucou; fi }"
test_42sh_string "{ if if true; then echo toto; fi; then echo popo; fi }"
test_42sh_string "{ if false; then adlghag; else tree . ;fi }"
test_42sh_string "{ if true; then echo tata; echo toto; else echo titi; fi }"
test_42sh_string "{ if true; then echo if; else if false; then echo elif; else echo else;fi;fi }"
delete
echo -e "\n${BLUE}RESULTS -- Command Block tests:${Color_Off} $passed/$((passed+failed)) tests passed"
