#!/bin/sh

. ../../functions.sh -- source-only

echo -e "\n${BLUE}\tSTEP 3 - Dot 🧪\n${Color_Off}"

echo "echo toto\necho titi" > temp
test_error2 ". temp"
test_42sh_string ". ./temp"
rm -rf temp

test_42sh_string "PATH=`pwd`/dot_tests && . test_redir_1"
test_42sh_string "PATH=`pwd`/dot_tests && . test_redir_2"
test_42sh_string "PATH=`pwd`/dot_tests && . test_redir_3"

delete
echo -e "\n${BLUE}RESULTS -- Dot 🧪 test:${Color_Off} $passed/$((passed+failed)) tests passed"
