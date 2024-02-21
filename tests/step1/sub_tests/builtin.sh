#!/bin/sh

. ../../functions.sh --source-only

# String Tests - BUILTIN
echo -e "\n${BLUE}\tSTEP 1 - Builtin tests 🧪\n${Color_Off}"
test_42sh_string "tree ."
test_42sh_string "cat echo_test.sh"
test_42sh_string "ls"
test_42sh_string "seq 5"
test_42sh_string "                             seq 5"
test_42sh_string "seq                                                            5"
delete
echo -e "\n${BLUE}RESULTS -- Builtin:${Color_Off} $passed/$((passed+failed)) tests passed"
